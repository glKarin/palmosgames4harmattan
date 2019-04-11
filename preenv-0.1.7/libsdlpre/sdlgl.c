#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_gles.h>
#include <GLES/gl.h>

#include "config.h"

#include "SDLGL.h"

#include "override.h"
#include "private.h"
#include "debug.h"

#define DEBUG_DOMAIN "SDLGL"

static SDL_GLES_Version desired_version = SDL_GLES_VERSION_NONE;
static SDL_bool gles_inited = SDL_FALSE;
static SDL_GLES_Context *context = NULL;

static SDL_Surface *v_screen = NULL;

static SDL_bool app_active = SDL_FALSE;
static SDL_EventFilter app_filter = NULL;

static SDL_bool taskbtn_pressed = SDL_FALSE;

static int gles_init()
{
	if (!gles_inited) {
		int res = SDL_GLES_Init(desired_version);
		if (res != 0) return res;
		TRACE("SDL_GLES initialized (res=%d)", res);
		gles_inited = SDL_TRUE;

		/* Palm SDL has a 16 bit depth buffer by default,
		 * but I'm going to set it to 8 by default,
		 * because it's faster on a N900. */
		res = SDL_GLES_SetAttribute(SDL_GLES_DEPTH_SIZE, 8);
		if (res != 0) return res;
	}
	return 0;
}

static void gles_quit()
{
	if (gles_inited) {
		if (context) {
			SDL_GLES_DeleteContext(context);
			context = NULL;
		}
		SDL_GLES_Quit();
		gles_inited = SDL_FALSE;
	}
}

static inline int mini(int i1, int i2)
{
	if (i1 < i2) return i1;
	else return i2;
}

static inline int absi(int i)
{
	if (i < 0) return -i;
	else return i;
}

static void make_box(struct box *b, const struct point *p1, const struct point *p2)
{
	b->x = mini(p1->x, p2->x);
	b->y = mini(p1->y, p2->y);
	b->w = absi(p1->x - p2->x);
	b->h = absi(p1->y - p2->y);
}

static void p_scale(struct point *p) 
{
	const int ox = p->x, oy = p->y;
	switch (s_rotate) {
		case PDL_ORIENTATION_0:
			p->x = ox * s_scale.x;
			p->y = oy * s_scale.y;
			break;
		case PDL_ORIENTATION_90:
			p->x = oy * s_scale.x;
			p->y = r_size.h - ox * s_scale.y;
			break;
		case PDL_ORIENTATION_180:
			p->x = r_size.w - ox * s_scale.x;
			p->y = r_size.h - oy * s_scale.y;
			break;
		case PDL_ORIENTATION_270:
			p->x = r_size.w - oy * s_scale.x;
			p->y = ox * s_scale.y;
			break;
		default:
			break;
	}
}

static void p_unscale(struct point *p) 
{
	const int ox = p->x, oy = p->y;
	switch (s_rotate) {
		case PDL_ORIENTATION_0:
			p->x = ox / s_scale.x;
			p->y = oy / s_scale.y;
			break;
		case PDL_ORIENTATION_90:
			p->y = v_size.h - ox / s_scale.x;
			p->x = oy / s_scale.y;
			break;
		case PDL_ORIENTATION_180:
			p->x = v_size.w - ox / s_scale.x;
			p->y = v_size.h - oy / s_scale.y;
			break;
		case PDL_ORIENTATION_270:
			p->y = ox / s_scale.x;
			p->x = v_size.w - oy / s_scale.y;
			break;
		default:
			break;
	}
}

void SDLPRE_PushEvent(SDL_Event *event) {
	/* Here's to hoping the app_filter function is thread-safe. */
	if (app_filter && !app_filter(event)) {
		/* Application's event filter handled this event. */
		return;
	}
	SDL_PushEvent(event);
}

static int filter_active_event(const SDL_ActiveEvent *event) {
	SDL_Event fakeevent;
	SDL_ActiveEvent *fake = &fakeevent.active;

	fake->type = SDL_ACTIVEEVENT;
	fake->gain = event->gain;
	fake->state = 0;
	if (event->state & SDL_APPINPUTFOCUS) {
		TRACE("Sending %s focus event", fake->gain ? "gain" : "lose");
		PDL_NotifyFocus(fake->gain);
		app_active = fake->gain;
		fake->state = SDL_APPACTIVE;
	}
	if (fake->state) {
		SDLPRE_PushEvent(&fakeevent);
	}
	return 0;
}

static int filter_mouse_motion(const SDL_MouseMotionEvent *event) {
	SDL_Event fakeevent;
	SDL_MouseMotionEvent *fake = &fakeevent.motion;
	struct point p = { event->x, event->y };
	struct point prel = { event->xrel, event->yrel };

	if (event->x < TASKNAV_BTN_WIDTH && event->y < TASKNAV_BTN_HEIGHT) {
		/* Cursor still inside button area: OK */
	} else if (taskbtn_pressed) {
		/* Cursor exited button area */
		TaskBtn_Release();
		taskbtn_pressed = SDL_FALSE;
	}

	p_unscale(&p);
	p_unscale(&prel);
	*fake = *event;
	fake->x = p.x;
	fake->y = p.y;
	fake->xrel = prel.x;
	fake->yrel = prel.y;

	SDLPRE_PushEvent(&fakeevent);
	return 0;
}

static int filter_mouse_button(const SDL_MouseButtonEvent *event) {
	SDL_Event fakeevent;
	SDL_MouseButtonEvent *fake = &fakeevent.button;
	struct point p = { event->x, event->y };

	if (event->x < TASKNAV_BTN_WIDTH && event->y < TASKNAV_BTN_HEIGHT) {
		if (event->type == SDL_MOUSEBUTTONUP && taskbtn_pressed) {
			TaskBtn_Release();
			taskbtn_pressed = SDL_FALSE;
		} else if (event->type == SDL_MOUSEBUTTONDOWN && !taskbtn_pressed) {
			TaskBtn_Press();
			taskbtn_pressed = SDL_TRUE;
		}
	}

	p_unscale(&p);
	*fake = *event;
	fake->x = p.x;
	fake->y = p.y;

	SDLPRE_PushEvent(&fakeevent);
	return 0;
}

static int filter_event(const SDL_Event *event)
{
	switch (event->type) {
	case SDL_VIDEORESIZE:
		TRACE("Deleted a video resize event");
		return 0;
	case SDL_ACTIVEEVENT:
		return filter_active_event(&event->active);
	case SDL_MOUSEMOTION:
		return filter_mouse_motion(&event->motion);
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		return filter_mouse_button(&event->button);
	default:
		if (app_filter) return app_filter(event);
	}
	return 1;
}

/** Recalculate scaling multipliers */
void SDLPRE_RefreshScale()
{
	switch (s_rotate) {
		case PDL_ORIENTATION_0:
		case PDL_ORIENTATION_180:
			s_scale.x = (float)r_size.w / v_size.w;
			s_scale.y = (float)r_size.h / v_size.h;
			break;
		case PDL_ORIENTATION_90:
		case PDL_ORIENTATION_270:
			s_scale.x = (float)r_size.w / v_size.h;
			s_scale.y = (float)r_size.h / v_size.w;
			break;
	}
}

int SDL_Init(Uint32 flags)
{
	OVERRIDES(SDL_Init, (Uint32 flags), int);
	TRACE("Called sdl_init with flags = 0x%x", flags);

	int res = SUPER(flags);

	if (res == 0) {
		/* Success, hook some stuff. */
		SDL_ShowCursor(SDL_DISABLE); /* Pre disables cursor by default. */
		SDL_SetEventFilter(&filter_event);
		if (flags & SDL_INIT_VIDEO) {
			X11_Init();
		}
	}

	return res;
}

void SDL_Quit()
{
	OVERRIDES(SDL_Quit, (), void);
	TRACE("Called SDL_Quit");
	gles_quit();
	v_screen = NULL;
	app_active = SDL_FALSE;
	app_filter = NULL;
	screen = NULL;
	SUPER();
}

int SDL_GL_SetAttribute(SDL_GLattr attr, int value)
{
	gles_init();
	switch (attr) {
		case SDL_GL_CONTEXT_MAJOR_VERSION:
			TRACE("Requested GL major version %d", value);
			switch (value) {
				case 1:
					desired_version = SDL_GLES_VERSION_1_1;
					return 0;
				case 2:
					desired_version = SDL_GLES_VERSION_2_0;
					return 0;
				default:
					SDL_SetError("Invalid GLES major version: %d", value);
					return -1;
			}
			break;
		case SDL_GL_CONTEXT_MINOR_VERSION:
			TRACE("Requested GL minor version %d (ignored)", value);
			return 0;
		case SDL_GL_RED_SIZE:
			TRACE("Ignoring color buffer red size %d", value);
			return 0;
		case SDL_GL_GREEN_SIZE:
			TRACE("Ignoring color buffer green size %d", value);
			return 0;
		case SDL_GL_BLUE_SIZE:
			TRACE("Ignoring color buffer blue size %d", value);
			return 0;
		case SDL_GL_ALPHA_SIZE:
			TRACE("Ignoring color buffer alpha size %d", value);
			return 0;
		case SDL_GL_DOUBLEBUFFER:
			TRACE("Ignoring doublebuffer preference %d", value);
			/* I wouldn't know how to handle it either way. */
			return 0;
		case SDL_GL_DEPTH_SIZE:
			TRACE("Requested depth buffer size %d", value);
			SDL_GLES_SetAttribute(SDL_GLES_DEPTH_SIZE, value);
			return 0;
		case SDL_GL_STENCIL_SIZE:
			TRACE("Requested stencil buffer size %d", value);
			SDL_GLES_SetAttribute(SDL_GLES_STENCIL_SIZE, value);
			return 0;
		default:
			WARN("Application asks for unknown GL attribute %u, value %d", attr, value);
			SDL_SetError("Unknown GL attribute");
			return 0;
	}
}

int SDL_GL_GetAttribute(SDL_GLattr attr, int *value)
{
	gles_init();
	if (!value) {
		SDL_SetError("Null value");
		return -1;
	}
	switch (attr) {
		case SDL_GL_CONTEXT_MAJOR_VERSION:
			switch (desired_version) {
				case SDL_GLES_VERSION_1_1: *value = 1; break;
				case SDL_GLES_VERSION_2_0: *value = 2; break;
				default:                   *value = 0; break;
			}
			return 0;
		case SDL_GL_CONTEXT_MINOR_VERSION:
			switch (desired_version) {
				case SDL_GLES_VERSION_1_1: *value = 1; break;
				case SDL_GLES_VERSION_2_0: *value = 0; break;
				default:                   *value = 0; break;
			}
			return 0;
		case SDL_GL_RED_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_RED_SIZE, value);
		case SDL_GL_GREEN_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_GREEN_SIZE, value);
		case SDL_GL_BLUE_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_BLUE_SIZE, value);
		case SDL_GL_ALPHA_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_ALPHA_SIZE, value);
		case SDL_GL_DOUBLEBUFFER:
			*value = 0;
			return 0;
		case SDL_GL_DEPTH_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_DEPTH_SIZE, value);
		case SDL_GL_STENCIL_SIZE:
			return SDL_GLES_GetAttribute(SDL_GLES_STENCIL_SIZE, value);
		default:
			WARN("Application asks for value of unknown GL attribute %u", attr);
			SDL_SetError("Unknown GL attribute");
			return 0;
	}
}

SDL_Surface * SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
	OVERRIDES(SDL_SetVideoMode, (int width, int height, int bpp, Uint32 flags), SDL_Surface*);

	TRACE("Called SetVideoMode(%d, %d, %d, 0x%x)", width, height, bpp, flags);

	if (flags & SDL_OPENGL) {
		TRACE("Requested OpenGL video mode");
		flags &= ~SDL_OPENGL; /* Our platform SDL doesn't handle it. */

		int res = gles_init();
		if (res != 0 ) return NULL;

		if (!context) {
			context = SDL_GLES_CreateContext();
			if (!context) return NULL;
		}

		/* Always create a GL surface the size of the native screen, as per the Pre. */
		screen = SUPER(r_size.w, r_size.h, r_bpp, flags | SDL_FULLSCREEN);
		if (!screen) return NULL;

		SDLPRE_RefreshScale();

		res = SDL_GLES_MakeCurrent(context);
		if (res != 0) return NULL;

		app_active = SDL_TRUE;

		if (r_size.w != v_size.w || r_size.h != v_size.h) {
			TRACE("Creating shadow surface (w=%d, h=%d)", v_size.w, v_size.h);
			/* Assuming v_size <= r_size */
			v_screen = SDL_ConvertSurface(screen, screen->format, screen->flags);
			v_screen->w = v_size.w;
			v_screen->h = v_size.h;
		} else {
			v_screen = screen;
		}

		return v_screen;
	} else {
		if (gles_inited) {
			gles_quit();
		}

		screen = SUPER(width, height, bpp, flags);

		WARN("Cannot yet scale non OpenGL applications");

		app_active = SDL_TRUE;

		return screen;
	}
}

int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags)
{
	OVERRIDES(SDL_VideoModeOK, (int width, int height, int bpp, Uint32 flags), int);
	TRACE("Asked if video mode %dx%dx%d (flags=0x%x) is ok",
		width, height, bpp, flags);
	flags &= ~SDL_OPENGL;
	int res = SUPER(width, height, bpp, flags);
	if (res == 0) {
		WARN("It isn't");
	}
	return res;
}

const SDL_VideoInfo* SDL_GetVideoInfo(void)
{
	OVERRIDES(SDL_GetVideoInfo, (void), const SDL_VideoInfo*);
	static SDL_VideoInfo info;
	const SDL_VideoInfo* sdl_info = SUPER();
	info = *sdl_info;

	/* Lie. */
	info.current_w = v_size.w;
	info.current_h = v_size.h;
	TRACE("Reporting video information width=%u, height=%u",
		info.current_w, info.current_h);

	return &info;
}

SDL_Surface* SDL_GetVideoSurface(void)
{
	return v_screen;
}

void SDL_GL_SwapBuffers(void)
{
	if (!app_active) {
		/* Let's cap the framerate a bit when the application is not active */
		SDL_Delay(1000 / DEACTIVATED_FPS_CAP);
	}
	SDL_GLES_SwapBuffers();
}

void SDL_SetEventFilter(SDL_EventFilter filter)
{
	OVERRIDES(SDL_SetEventFilter, (SDL_EventFilter filter), void);
	if (filter != &filter_event) {
		TRACE("SetEventFilter(%p)", filter);
		app_filter = filter;
	}
	SUPER(filter);
}

Uint8 SDL_GetAppState(void)
{
	if (app_active)
		return SDL_APPMOUSEFOCUS | SDL_APPINPUTFOCUS | SDL_APPACTIVE;
	else
		return 0;
}

Uint8 SDL_GetRelativeMouseState(int *x, int *y)
{
	OVERRIDES(SDL_GetRelativeMouseState, (int *x, int *y), Uint8);
	int sx, sy;
	Uint8 state = SUPER(&sx, &sy);
	struct point p = { sx, sy };
	p_unscale(&p);
	if (x) *x = p.x;
	if (y) *y = p.y;
	return state;
}

Uint8 SDL_GetMouseState(int *x, int *y)
{
	OVERRIDES(SDL_GetMouseState, (int *x, int *y), Uint8);
	int sx, sy;
	Uint8 state = SUPER(&sx, &sy);
	struct point p = { sx, sy };
	p_unscale(&p);
	if (x) *x = p.x;
	if (y) *y = p.y;
	return state;
}

Uint8 SDL_GetRelativeMultiMouseState(int which, int *x, int *y)
{
	if (which == 0) return SDL_GetRelativeMouseState(x, y);
	return 0;
}

Uint8 SDL_GetMultiMouseState(int which, int *x, int *y)
{
	if (which == 0) return SDL_GetMouseState(x, y);
	return 0;
}

int SDL_UpperBlit(SDL_Surface *src, SDL_Rect *srcrect,
			 SDL_Surface *dst, SDL_Rect *dstrect)
{
	OVERRIDES(SDL_UpperBlit, (SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect), int);
	if (dst == screen || src == screen) {
		WARN("Someone is trying to blit from/to screen; this breaks the fourth wall");
	}
	return SUPER(src, srcrect, dst, dstrect);
}

void glLoadIdentity()
{
	OVERRIDES(glLoadIdentity, (void), void);
	SUPER();

	/* glGet calls might be costly. Skip if possible. */
	if (s_rotate == PDL_ORIENTATION_0) return;

	GLint mode;
	glGetIntegerv(GL_MATRIX_MODE, &mode);
	if (mode == GL_PROJECTION) {
		switch (s_rotate) {
		case PDL_ORIENTATION_0:
			break;
		case PDL_ORIENTATION_90:
			glRotatef(90.0f, 0.0, 0.0, -1.0f);
			break;
		case PDL_ORIENTATION_180:
			glRotatef(180.0f, 0.0, 0.0, -1.0f);
			break;
		case PDL_ORIENTATION_270:
			glRotatef(270.0f, 0.0, 0.0, -1.0f);
			break;
		default:
			break;
		}
	}
}

void glLoadMatrixf(const GLfloat * m)
{
	glLoadIdentity();
	glMultMatrixf(m);
}

void glLoadMatrixx(const GLfixed * m)
{
	glLoadIdentity();
	glMultMatrixx(m);
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	OVERRIDES(glViewport, (GLint x, GLint y, GLsizei width, GLsizei height), void);
	struct point p1 = { x, y };
	struct point p2 = { x + width, y + height };
	struct box b;
	p_scale(&p1);
	p_scale(&p2);
	make_box(&b, &p1, &p2);
	SUPER(b.x, b.y, b.w, b.h);
}

void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	OVERRIDES(glScissor, (GLint x, GLint y, GLsizei width, GLsizei height), void);
	struct point p1 = { x, y };
	struct point p2 = { x + width, y + height };
	struct box b;
	p_scale(&p1);
	p_scale(&p2);
	make_box(&b, &p1, &p2);
	SUPER(b.x, b.y, b.w, b.h);
}


#include <stdio.h>
#include <stdbool.h>

#include <GLES/gl.h>
#include <SDL.h>
#include <PDL.h>

static SDL_Surface *screen;

int main(int argc, char** argv)
{
	bool quit = false;
	SDL_Event event;
	PDL_Err err;
	SDL_Init(SDL_INIT_VIDEO);
	PDL_Init(0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);

	screen = SDL_SetVideoMode(320, 480, 0, SDL_OPENGL);

	PDL_EnableLocationTracking(SDL_TRUE);

	while (!quit) {
		PDL_Location loc;
		glClearColor(0.0, 0.5, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapBuffers();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
			break;
			case SDL_USEREVENT:
				switch (event.user.code) {
				case PDL_GPS_UPDATE:
					printf("Position received\n");
					err = PDL_GetLocation(&loc);
					printf("err=%d Lat=%f Long=%f Alt=%f horzAcc=%f vertAcc=%f head=%f vel=%f", err, loc.latitude, loc.longitude, loc.altitude, loc.horizontalAccuracy, loc.verticalAccuracy, loc.heading, loc.velocity);
				break;
				case PDL_GPS_FAILURE:
					printf("Error received\n");
				break;
				}
			break;
			}
		}
	}

	PDL_EnableLocationTracking(SDL_FALSE);

	PDL_Quit();
	SDL_Quit();
}

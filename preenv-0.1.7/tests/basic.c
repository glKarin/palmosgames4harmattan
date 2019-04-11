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
	SDL_Init(SDL_INIT_VIDEO);
	PDL_Init(0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);

	screen = SDL_SetVideoMode(320, 480, 0, SDL_OPENGL);

	while (!quit) {
		glClearColor(0.5, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapBuffers();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
			break;
			}
		}
	}

	PDL_Quit();
	SDL_Quit();
}

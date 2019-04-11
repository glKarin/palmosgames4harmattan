#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <SDL.h>
#include <PDL.h>

#define BUF_SZ 4096
static char buf[BUF_SZ];

int main(int argc, char** argv)
{
	PDL_Err err;

	SDL_Init(0);
	PDL_Init(0);

	getcwd(buf, BUF_SZ);
	printf("Cwd = '%s'\n", buf);

	printf("Asking for data file path 'stuff.dat'\n");
	err = PDL_GetDataFilePath("stuff.dat", buf, BUF_SZ);
	printf("Path = '%s', err = %d\n", buf, err);

	SDL_Delay(100);

	PDL_Quit();
	SDL_Quit();
}


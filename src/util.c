#include <SDL_rwops.h>

int SDL_fgetc(SDL_RWops *file)
{
	unsigned char c;
	if (SDL_RWread(file, &c, 1, 1) == 0) {
		return -1;
	}
	return c;
}
#include "starsquatters.h"
#include "snprintf.h"
#include "util.h"

int SDL_fgetc(SDL_RWops *file)
{
	unsigned char c;
	if (SDL_RWread(file, &c, 1, 1) == 0) {
		return -1;
	}
	return c;
}

char *SDL_fgets(SDL_RWops *file, char * const buf, size_t max)
{
	size_t c = 0;
	while (c < max) {
		int i = SDL_fgetc(file);
		if (i == -1) {
			break;
		}
		buf[c] = (char)i;
		c++;
		if (i == '\n') {
			break;
		}
	}
	if (c == 0) return NULL;
	buf[c] = 0;
	return buf;
}

int SDL_fputs(const char *string, SDL_RWops *file)
{
	int len = strlen(string);
	return SDL_RWwrite(file, string, 1, len) < len ? EOF : 0;
}

std::string itos(int i)
{
	char buf[20];
	snprintf(buf, 20, "%d", i);
	return std::string(buf);
}
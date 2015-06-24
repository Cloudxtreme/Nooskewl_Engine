#ifndef UTIL_H
#define UTIL_H

#include <SDL_rwops.h>

int SDL_fgetc(SDL_RWops *file);
char *SDL_fgets(SDL_RWops *file, char * const buf, size_t max);

#endif // UTIL_H
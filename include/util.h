#ifndef UTIL_H
#define UTIL_H

#include "starsquatters.h"

int SDL_fgetc(SDL_RWops *file);
char *SDL_fgets(SDL_RWops *file, char * const buf, size_t max);

std::string itos(int i);

#endif // UTIL_H
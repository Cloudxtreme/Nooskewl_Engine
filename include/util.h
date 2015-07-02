#ifndef UTIL_H
#define UTIL_H

#include "starsquatters.h"

int SDL_fgetc(SDL_RWops *file);
char *SDL_fgets(SDL_RWops *file, char * const buf, size_t max);
int SDL_fputs(const char *string, SDL_RWops *file);

SDL_RWops *open_file(std::string filename);

std::string itos(int i);

#endif // UTIL_H
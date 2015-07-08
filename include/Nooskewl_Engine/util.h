#ifndef UTIL_H
#define UTIL_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"

EXPORT int SDL_fgetc(SDL_RWops *file);
EXPORT char *SDL_fgets(SDL_RWops *file, char * const buf, size_t max);
EXPORT int SDL_fputs(const char *string, SDL_RWops *file);

EXPORT SDL_RWops *open_file(std::string filename) throw (Error);

EXPORT std::string itos(int i);

class EXPORT List_Directory {
public:
	List_Directory(std::string glob);
	~List_Directory();

	std::string next();

private:
#ifdef _MSC_VER
	bool got_first;
	bool done;
	HANDLE handle;
	WIN32_FIND_DATA ffd;
#endif
};

#endif // UTIL_H
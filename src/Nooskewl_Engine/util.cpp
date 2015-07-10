#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/global.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/util.h"

namespace Nooskewl_Engine {

int SDL_fgetc(SDL_RWops *file)
{
	unsigned char c;
	if (SDL_RWread(file, &c, 1, 1) == 0) {
		return EOF;
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
	unsigned int len = strlen(string);
	return SDL_RWwrite(file, string, 1, len) < len ? EOF : 0;
}

SDL_RWops *open_file(std::string filename)
{
	SDL_RWops *file;

	if (g.cpa->exists(filename)) {
		file = g.cpa->load(filename);
	}
	else {
		throw FileNotFoundError(filename);
	}

	return file;
}

std::string itos(int i)
{
	char buf[20];
	snprintf(buf, 20, "%d", i);
	return std::string(buf);
}

bool check_args(int argc, char **argv, std::string arg)
{
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], arg.c_str())) {
			return true;
		}
	}
	return false;
}

} // End namespace Nooskewl_Engine

#ifdef _MSC_VER
List_Directory::List_Directory(std::string glob) :
	got_first(false),
	done(false)
{
	handle = FindFirstFile(glob.c_str(), &ffd);
	if (handle == 0) {
		done = true;
	}
}

List_Directory::~List_Directory()
{
	FindClose(handle);
}

std::string List_Directory::next()
{
	if (done) {
		return "";
	}

	if (got_first == true) {
		if (FindNextFile(handle, &ffd) == 0) {
			done = true;
		}
	}
	else {
		got_first = true;
	}

	return ffd.cFileName;
}
#endif // _MSC_VER
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/internal.h"

using namespace Nooskewl_Engine;

#ifdef _MSC_VER
HINSTANCE dll_handle;
#else
void *so_handle;
#endif

namespace Nooskewl_Engine {

Module m;

void load_dll()
{
#ifdef _MSC_VER
	List_Directory ld("*.dll");
#else
	List_Directory ld("*.so");
#endif
	std::string filename;

	while ((filename = ld.next()) != "") {
#ifdef _MSC_VER
		dll_handle = LoadLibrary(filename.c_str());

		if (!dll_handle) {
			throw FileNotFoundError("Couldn't load DLL");
		}

		m.get_map_logic = (Map_Logic_Getter)GetProcAddress(dll_handle, "get_map_logic");
		if (m.get_map_logic != NULL) {
			infomsg("Using %s\n", filename.c_str());
			return;
		}
#else
		so_handle = dlopen(filename.c_str(), RTLD_LAZY);

		if (so_handle == NULL) {
			throw FileNotFoundError("Couldn't load shared library");
		}

		m.get_map_logic = (Map_Logic_Getter)dlsym(so_handle, "get_map_logic");
		if (m.get_map_logic != NULL) {
			infomsg("Using %s\n", filename.c_str());
			return;
		}
#endif
	}

	throw FileNotFoundError("Couldn't find a game DLL");
}

void close_dll()
{
#ifdef _MSC_VER
	// FIXME!
#else
	dlclose(so_handle);
#endif
}

#ifdef _MSC_VER

int c99_vsnprintf(char* str, int size, const char* format, va_list ap)
{
    int count = -1;

    if (size != 0)
        count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
    if (count == -1)
        count = _vscprintf(format, ap);

    return count;
}

int c99_snprintf(char* str, int size, const char* format, ...)
{
    int count;
    va_list ap;

    va_start(ap, format);
    count = c99_vsnprintf(str, size, format, ap);
    va_end(ap);

    return count;
}

#endif // _MSC_VER

void errormsg(const char *fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	printf("ERROR: ");
	vprintf(fmt, v);
	va_end(v);
}

void infomsg(const char *fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	vprintf(fmt, v);
	va_end(v);
}

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
	SDL_RWops *file = noo.cpa->open(filename);
	if (file == NULL) {
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

#ifdef _MSC_VER
List_Directory::List_Directory(std::string filespec) :
	got_first(false),
	done(false)
{
	handle = FindFirstFile(filespec.c_str(), &ffd);
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
#else
List_Directory::List_Directory(std::string filespec)
{
	gl.gl_pathv = NULL;

	int ret = glob(filespec.c_str(), 0, NULL, &gl);

	if (ret != 0) {
		i = 0;
	}
}

List_Directory::~List_Directory()
{
	free(gl.gl_pathv);
}

std::string List_Directory::next()
{
	if (i >= gl.gl_pathc) {
		i = -1;
	}

	if (i < 0) {
		return "";
	}
	return gl.gl_pathv[i++];
}
#endif // _MSC_VER

} // End namespace Nooskewl_Engine

#ifndef INTERNAL_H
#define INTERNAL_H

#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/sample.h"
#include "Nooskewl_Engine/vertex_cache.h"

#ifdef NOOSKEWL_ENGINE_BUILD

namespace Nooskewl_Engine {

typedef Map_Logic *(*Map_Logic_Getter)(std::string map_name);

void load_dll();
void close_dll();

#ifdef NOOSKEWL_ENGINE_WINDOWS
/* MSVC doesn't have snprintf */

#define snprintf c99_snprintf

int c99_vsnprintf(char* str, int size, const char* format, va_list ap);
int c99_snprintf(char* str, int size, const char* format, ...);

#endif

void errormsg(const char *fmt, ...);
void infomsg(const char *fmt, ...);
void printGLerror(const char *fmt, ...);

struct Module {
	Map_Logic_Getter get_map_logic;
	// audio
	SDL_mutex *sample_mutex;
	SDL_AudioSpec device_spec;
	std::vector<SampleInstance *> playing_samples;
	// graphics
#ifdef NOOSKEWL_ENGINE_WINDOWS
	IDirect3DDevice9 *d3d_device;
	LPD3DXEFFECT effect;
#endif
	GLuint current_shader;
	Vertex_Cache *vertex_cache;
};

class List_Directory {
public:
	List_Directory(std::string filespec);
	~List_Directory();

	std::string next();

private:
#ifdef NOOSKEWL_ENGINE_WINDOWS
	bool got_first;
	bool done;
	HANDLE handle;
	WIN32_FIND_DATA ffd;
#else
	int i;
	glob_t gl;
#endif
};

int SDL_fgetc(SDL_RWops *file);
char *SDL_fgets(SDL_RWops *file, char * const buf, size_t max);
int SDL_fputs(const char *string, SDL_RWops *file);

SDL_RWops *open_file(std::string filename);
std::string itos(int i);
bool check_args(int argc, char **argv, std::string arg);

#ifdef NOOSKEWL_ENGINE_WINDOWS
HICON win_create_icon(HWND wnd, Uint8 *data, int w, int h, int xfocus, int yfocus, bool is_cursor);
#endif

extern Module m;

} // End namespace Nooskewl_Engine

#endif

#endif // INTERNAL_H

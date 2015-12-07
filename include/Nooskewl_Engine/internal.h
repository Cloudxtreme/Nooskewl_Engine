#ifndef INTERNAL_H
#define INTERNAL_H

#include "Nooskewl_Engine/brain.h"
#include "Nooskewl_Engine/basic_types.h"

namespace Nooskewl_Engine {

class Brain;
class Map_Logic;
struct SampleInstance;
class Vertex_Cache;

typedef bool (*DLL_Start)();
typedef void (*DLL_End)();
typedef void (*DLL_Game_Started)();
typedef void (*DLL_Pause)();
typedef Map_Logic *(*Map_Logic_Getter)(std::string map_name, int last_visited_time);
typedef Brain *(*Brain_Getter)(std::string type, std::string data);
typedef std::vector<std::string> (*Actions_Getter)(Map_Entity *entity);
typedef void (*Action_Taker)(std::string action, Map_Entity *entity);

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
	// Game DLL functions
	DLL_Start dll_start;
	DLL_End dll_end;
	DLL_Game_Started dll_game_started;
	DLL_Pause dll_pause;
	Map_Logic_Getter dll_get_map_logic;
	Brain_Getter dll_get_brain;
	Actions_Getter dll_get_actions;
	Action_Taker dll_take_action;

	// audio
	SDL_mutex *mixer_mutex;
	SDL_AudioSpec device_spec;
	std::vector<SampleInstance *> playing_samples;
	// graphics
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
int SDL_fputc(int c, SDL_RWops *file);
char *SDL_fgets(SDL_RWops *file, char * const buf, size_t max);
int SDL_fputs(const char *string, SDL_RWops *file);
NOOSKEWL_ENGINE_EXPORT void SDL_fprintf(SDL_RWops *file, const char *fmt, ...);

SDL_RWops *open_file(std::string filename);
NOOSKEWL_ENGINE_EXPORT std::string itos(int i);
int check_args(int argc, char **argv, std::string arg);
NOOSKEWL_ENGINE_EXPORT std::string string_printf(const char *fmt, ...);

NOOSKEWL_ENGINE_EXPORT std::string escape_string(std::string s, char c);
NOOSKEWL_ENGINE_EXPORT std::string unescape_string(std::string);

#if defined NOOSKEWL_ENGINE_WINDOWS
HICON win_create_icon(HWND wnd, Uint8 *data, Size<int> size, int xfocus, int yfocus, bool is_cursor);
#elif defined __linux__
X11::Cursor x_create_cursor(X11::Display *display, Uint8 *data, Size<int> size, int xfocus, int yfocus);
#endif

/* From: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring */
// trim from start
static inline std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

extern Module m;

} // End namespace Nooskewl_Engine

#endif // INTERNAL_H

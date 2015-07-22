#ifndef MAIN_H
#define MAIN_H

#ifdef NOOSKEWL_ENGINE_WINDOWS
// Disable warnings about dll-interface
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#ifdef NOOSKEWL_ENGINE_BUILD
#define NOOSKEWL_ENGINE_EXPORT __declspec(dllexport)
#else
#define NOOSKEWL_ENGINE_EXPORT __declspec(dllimport)
#endif
#else
#define NOOSKEWL_ENGINE_EXPORT
#endif

#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined NOOSKEWL_ENGINE_WINDOWS
#include <d3d9.h>
#include <d3dx9.h>
#else
#include <dlfcn.h>
#include <glob.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

#include <zlib.h>

#include <tgui3.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// To extract strings to translate
#define TRANSLATE(text) std::string(text)
#define END

namespace Nooskewl_Engine {

enum Direction {
	N = 1,
	S,
	E,
	W
};

} // End namespace Nooskewl_Engine

#endif // MAIN_H

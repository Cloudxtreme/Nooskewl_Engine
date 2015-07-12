#ifndef STARSQUATTERS_H
#define STARSQUATTERS_H

#ifdef _MSC_VER
// Disable warnings about dll-interface
#pragma warning(disable : 4251)

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

#if defined _MSC_VER
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

#ifdef NOOSKEWL_ENGINE_BUILD

#ifdef _MSC_VER
#define FVF (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE4(1))
#endif

#define PI ((float)M_PI)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif

namespace Nooskewl_Engine {

enum Direction {
	N = 1,
	S,
	E,
	W
};

} // End namespace Nooskewl_Engine

#endif // STARSQUATTERS_H

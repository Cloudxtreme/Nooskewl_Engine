#ifndef STARSQUATTERS_H
#define STARSQUATTERS_H

#ifdef _MSC_VER
// MSVC 2013 warns on throw (Error)
#pragma warning(disable : 4251)
#pragma warning(disable : 4290)

#ifdef NOOSKEWL_ENGINE_BUILD
#define NOOSKEWL_EXPORT __declspec(dllexport)
#else
#define NOOSKEWL_EXPORT __declspec(dllimport)
#endif
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
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef _MSC_VER
#include <d3d9.h>
#include <d3dx9.h>
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_rwops.h>
#include <SDL/SDL_timer.h>
#include <SDL/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_opengl.h>
#include <SDL_rwops.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#endif

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
#ifndef STARSQUATTERS_H
#define STARSQUATTERS_H

#ifdef _MSC_VER
// MSVC 2013 warns on throw (Error)
#pragma warning(disable : 4290)
#endif

#include <cctype>
#include <cmath>
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

#define PI ((float)M_PI)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

enum Direction {
	N = 1,
	S,
	E,
	W
};

#endif // STARSQUATTERS_H
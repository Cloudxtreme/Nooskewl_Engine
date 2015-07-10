#ifndef GLOBAL_H
#define GLOBAL_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/vertex_accel.h"

namespace Nooskewl_Engine {

struct Global {
	struct {
		bool mute;
	} audio;
	struct {
		int screen_w;
		int screen_h;
		bool opengl;

		SDL_Colour colours[256];
		SDL_Colour four_blacks[4];
		SDL_Colour four_whites[4];
		SDL_Colour black;
		SDL_Colour white;

		Font *font;
		Font *bold_font;

		Vertex_Accel *vertex_accel;
	} graphics;
	CPA *cpa;
	Map *map;
	Map_Entity *player;
};

NOOSKEWL_EXPORT extern Global g;

} // End namespace Nooskewl_Engine

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // GLOBAL_H
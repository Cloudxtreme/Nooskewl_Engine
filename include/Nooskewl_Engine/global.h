// Access to everything not in a class

#ifndef GLOBAL_H
#define GLOBAL_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/font.h"

namespace Nooskewl_Engine {

struct Global {
	// Audio
	bool mute;
	// Graphics
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
	// Other
	CPA *cpa;
	Map *map;
	Map_Entity *player;
};

NOOSKEWL_ENGINE_EXPORT extern Global g;

} // End namespace Nooskewl_Engine

#endif // GLOBAL_H
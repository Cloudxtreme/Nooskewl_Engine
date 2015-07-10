#ifndef VIDEO_H
#define VIDEO_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

	EXPORT void clear(SDL_Colour colour);
	EXPORT void clear_depth_buffer(float value);
	EXPORT void flip();
	EXPORT void set_default_projection();

}

#ifdef NOOSKEWL_ENGINE_BUILD

void init_video(int argc, char **argv);
void shutdown_video();

void set_map_transition_projection(float angle);

void release_graphics();
void reload_graphics();

#endif

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // VIDEO_H
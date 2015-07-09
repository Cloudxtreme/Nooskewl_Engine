#ifndef VIDEO_H
#define VIDEO_H

#include "Nooskewl_Engine/main.h"

EXPORT void clear(SDL_Colour colour);
EXPORT void flip();

void init_video(int argc, char **argv);
void shutdown_video();

EXPORT void set_default_projection();
void set_map_transition_projection(float angle);

void release_graphics();
void reload_graphics();

#endif // VIDEO_H

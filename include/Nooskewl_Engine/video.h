#ifndef VIDEO_H
#define VIDEO_H

#include "Nooskewl_Engine/main.h"

EXPORT void clear(SDL_Colour colour);
EXPORT void flip();

EXPORT void init_video(int argc, char **argv);
EXPORT void shutdown_video();

EXPORT void set_default_projection();
EXPORT void set_map_transition_projection(float angle);

EXPORT extern int screen_w;
EXPORT extern int screen_h;

EXPORT extern bool opengl;

#endif // VIDEO_H

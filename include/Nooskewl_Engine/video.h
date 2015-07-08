#ifndef VIDEO_H
#define VIDEO_H

#include "Nooskewl_Engine/main.h"

EXPORT void flip();

EXPORT void init_video();
EXPORT void shutdown_video();

EXPORT void set_default_projection();
EXPORT void set_map_transition_projection(float angle);

EXPORT extern int screen_w;
EXPORT extern int screen_h;

#endif // VIDEO_H

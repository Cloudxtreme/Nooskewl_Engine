#ifndef VIDEO_H
#define VIDEO_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

NOOSKEWL_ENGINE_EXPORT void clear(SDL_Colour colour);
NOOSKEWL_ENGINE_EXPORT void clear_depth_buffer(float value);
NOOSKEWL_ENGINE_EXPORT void flip();
NOOSKEWL_ENGINE_EXPORT void set_default_projection();

} // End namespace Nooskewl_Engine

#endif // VIDEO_H
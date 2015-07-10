#ifndef VIDEO_H
#define VIDEO_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

NOOSKEWL_EXPORT void clear(SDL_Colour colour);
NOOSKEWL_EXPORT void clear_depth_buffer(float value);
NOOSKEWL_EXPORT void flip();
NOOSKEWL_EXPORT void set_default_projection();

} // End namespace Nooskewl_Engine

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // VIDEO_H
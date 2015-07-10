#ifndef GRAPHICS_H
#define GRAPHICS_h

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/font.h"

namespace Nooskewl_Engine {

NOOSKEWL_EXPORT void draw_line(Point<int> a, Point<int> b, SDL_Colour colour);
NOOSKEWL_EXPORT void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4]);
NOOSKEWL_EXPORT void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour colour);
NOOSKEWL_EXPORT void draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle);

} // End namespace Nooskewl_Engine

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // GRAPHICS_H
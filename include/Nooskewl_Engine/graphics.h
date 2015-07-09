#ifndef GRAPHICS_H
#define GRAPHICS_h

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/font.h"

EXPORT void init_graphics() throw (Error);
EXPORT void shutdown_graphics();
EXPORT void update_graphics();

EXPORT void load_palette(std::string name) throw (Error);
EXPORT void draw_line(Point<int> a, Point<int> b, SDL_Colour colour);
EXPORT void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4]);
EXPORT void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour colour);
EXPORT void draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle);

EXPORT extern SDL_Colour colours[256];
EXPORT extern SDL_Colour four_blacks[4];
EXPORT extern SDL_Colour four_whites[4];
EXPORT extern SDL_Colour black;
EXPORT extern SDL_Colour white;

#endif // GRAPHICS_H
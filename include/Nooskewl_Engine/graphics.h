#ifndef GRAPHICS_H
#define GRAPHICS_h

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/font.h"

void init_graphics() throw (Error);
void shutdown_graphics();
void update_graphics();
void load_palette(std::string name) throw (Error);

EXPORT void draw_line(Point<int> a, Point<int> b, SDL_Colour colour);
EXPORT void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4]);
EXPORT void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour colour);
EXPORT void draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle);

#endif // GRAPHICS_H
#ifndef GRAPHICS_H
#define GRAPHICS_h

#include "starsquatters.h"
#include "error.h"
#include "font.h"

void init_graphics() throw (Error);
void shutdown_graphics();
void update_graphics();

void load_palette(std::string name) throw (Error);
void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4]);
void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour colour);
void draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle);

extern SDL_Colour colours[256];
extern SDL_Colour four_blacks[4];
extern SDL_Colour four_whites[4];
extern SDL_Colour black;
extern SDL_Colour white;

extern Font *font;
extern Font *bold_font;

#endif // GRAPHICS_H
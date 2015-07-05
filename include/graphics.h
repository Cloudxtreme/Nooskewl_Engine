#ifndef GRAPHICS_H
#define GRAPHICS_h

#include "starsquatters.h"
#include "error.h"
#include "font.h"

void init_graphics() throw (Error);
void shutdown_graphics();

void load_palette(std::string name) throw (Error);
void draw_quad(float x, float y, float w, float h, SDL_Colour vertex_colours[4]);
void draw_quad(float x, float y, float w, float h, SDL_Colour colour);
void draw_window(float x, float y, float w, float h);

extern SDL_Colour colours[256];
extern SDL_Colour four_whites[4];
extern SDL_Colour black;
extern SDL_Colour white;

extern Font *font;
extern Font *bold_font;

#endif // GRAPHICS_H
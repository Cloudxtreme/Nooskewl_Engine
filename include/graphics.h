#ifndef GRAPHICS_H
#define GRAPHICS_h

bool init_graphics();
void shutdown_graphics();

bool load_palette(std::string name);
void draw_quad(float x, float y, float w, float h, SDL_Colour vertex_colours[4]);
void draw_quad(float x, float y, float w, float h, SDL_Colour colour);
void draw_window(float x, float y, float w, float h);

extern SDL_Colour colours[256];
extern SDL_Colour four_whites[4];

#endif // GRAPHICS_H
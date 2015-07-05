#ifndef GRAPHICS_H
#define GRAPHICS_h

bool init_graphics();
void shutdown_graphics();

void draw_quad(float x, float y, float w, float h, SDL_Colour colour);
void draw_window(float x, float y, float w, float h);

#endif // GRAPHICS_H
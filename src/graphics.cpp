#include "starsquatters.h"
#include "graphics.h"
#include "image.h"
#include "vertex_accel.h"

static Image *window_image;

bool init_graphics()
{
	window_image = new Image();
	if (window_image->load_tga("misc_graphics/window.tga") == false) {
		delete window_image;
		return false;
	}

	return true;
}

void shutdown_graphics()
{
	delete window_image;
}

void draw_quad(float x, float y, float w, float h, SDL_Colour colour)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	vertex_accel->start();
	vertex_accel->buffer(0, 0, 0, 0, x, y, w, h, colour.r/255.0f, colour.g/255.0f, colour.b/255.0f, colour.a/255.0f, 0);
	vertex_accel->end();
}

void draw_window(float x, float y, float w, float h)
{
	// FIXME: use palette
	SDL_Colour colours[4] = {
		{ 162, 186, 255, 220 },
		{ 81, 30, 255, 220 },
		{ 65, 65, 255, 220 },
		{ 40, 0, 186, 220 }
	};

	float bg_w = w - 2;
	float bg_h = h - 2;
	float section_h = bg_h / 4;

	for (int i = 0; i < 4; i++) {
		draw_quad(x+1, y+1+i*section_h, bg_w, section_h, colours[i]);
	}

	window_image->start();
	window_image->draw_region(0, 0, 6, 6, x, y, 0); // top left
	window_image->draw_region(6, 0, 6, 6, x+w-6, y, 0); // top right
	window_image->draw_region(6, 6, 6, 6, x+w-6, y+h-6, 0); // bottom right
	window_image->draw_region(0, 6, 6, 6, x, y+h-6, 0); // bottom left
	window_image->stretch_region(5, 1, 2, 4, x+6, y+1, w-12, 4, 0); // top
	window_image->stretch_region(5, 7, 2, 4, x+6, y+h-5, w-12, 4, 0); // bottom
	window_image->stretch_region(1, 5, 4, 2, x+1, y+6, 4, h-12, 0); // left
	window_image->stretch_region(7, 5, 4, 2, x+w-5, y+6, 4, h-12, 0); // right
	window_image->end();
}
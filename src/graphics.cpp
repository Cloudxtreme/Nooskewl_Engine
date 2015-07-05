#include "starsquatters.h"
#include "graphics.h"
#include "image.h"
#include "log.h"
#include "util.h"
#include "vertex_accel.h"

static Image *window_image;

SDL_Colour colours[256];
SDL_Colour four_whites[4];

bool init_graphics()
{
	window_image = new Image();
	if (window_image->load_tga("misc_graphics/window.tga") == false) {
		errormsg("Error loading misc_graphics/window.tga!\n");
		delete window_image;
		return false;
	}

	if (load_palette("nes.gpl") == false) {
		errormsg("Couldn't load nes.gpl!\n");
		delete window_image;
		return false;
	}

	return true;
}

void shutdown_graphics()
{
	delete window_image;
}

bool load_palette(std::string name)
{
	SDL_RWops *file = open_file(name);
	if (file == NULL) {
		errormsg("Couldn't load palette: %s\n", name.c_str());
		return false;
	}

	char line[1000];

	SDL_fgets(file, line, 1000);
	if (strncmp(line, "GIMP Palette", 12)) {
		errormsg("Not a GIMP palette (%s)\n", name.c_str());
		SDL_RWclose(file);
		return false;
	}

	int line_count = 1;
	int colour_count = 0;

	while (SDL_fgets(file, line, 1000) != NULL) {
		line_count++;
		char *p = line;
		while (*p != 0 && isspace(*p)) p++;
		// Skip comments
		if (*p == '#') {
			continue;
		}
		int r, g, b;
		if (sscanf(line, "%d %d %d", &r, &g, &b) == 3) {
			colours[colour_count].r = r;
			colours[colour_count].g = g;
			colours[colour_count].b = b;
			colours[colour_count].a = 255;
			colour_count++;
		}
		else {
			infomsg("Syntax error on line %d of %s\n", line_count, name.c_str());
		}
	}

	for (int i = 0; i < 4; i++) {
		four_whites[i].r = 255;
		four_whites[i].g = 255;
		four_whites[i].b = 255;
		four_whites[i].a = 255;
	}

	SDL_RWclose(file);

	return true;
}

void draw_quad(float x, float y, float w, float h, SDL_Colour vertex_colours[4])
{
	glBindTexture(GL_TEXTURE_2D, 0);
	vertex_accel->start();
	vertex_accel->buffer(0, 0, 0, 0, x, y, w, h, vertex_colours, 0);
	vertex_accel->end();
}

void draw_quad(float x, float y, float w, float h, SDL_Colour colour)
{
	static SDL_Colour vertex_colours[4];
	for (int i = 0; i < 4; i++) {
		vertex_colours[i] = colour;
	}
	draw_quad(x, y, w, h, vertex_colours);
}

void draw_window(float x, float y, float w, float h)
{
	SDL_Colour vertex_colours[4];

	// Blue shades in NES palette
	vertex_colours[0] = vertex_colours[1] = colours[47];
	vertex_colours[2] = vertex_colours[3] = colours[44];

	for (int i = 0; i < 4; i++) {
		vertex_colours[i].a = 220;
	}

	draw_quad(x+1, y+1, w-2, h-2, vertex_colours);

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
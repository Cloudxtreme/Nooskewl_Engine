#include "graphics.h"
#include "image.h"
#include "log.h"
#include "sprite.h"
#include "util.h"
#include "vertex_accel.h"

static Image *window_image;
static Sprite *speech_arrow;

SDL_Colour colours[256];
SDL_Colour four_whites[4];
SDL_Colour black;
SDL_Colour white;

Font *font;
Font *bold_font;

void init_graphics()
{
	// FIXME: exceptions!
	try {
		window_image = new Image("window.tga");
		speech_arrow = new Sprite("speech_arrow");
		speech_arrow->start();
		font = new Font("fff_majestica.ttf", 8);
		bold_font = new Font("fff_majestica_bold.ttf", 8);
		load_palette("nes.gpl");
	}
	catch (Error e) {
		delete window_image;
		delete font;
		delete bold_font;
		window_image = NULL;
		font = NULL;
		bold_font = NULL;
		throw e;
	}
}

void shutdown_graphics()
{
	delete window_image;
}

void update_graphics()
{
	speech_arrow->update();
}

void load_palette(std::string name)
{
	name = "palettes/" + name;

	SDL_RWops *file = open_file(name);

	char line[1000];

	SDL_fgets(file, line, 1000);
	if (strncmp(line, "GIMP Palette", 12)) {
		SDL_RWclose(file);
		throw LoadError("not a GIMP palette: " + name);
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

	black.r = black.g = black.b = 0;
	black.a = 255;
	white.r = white.g = white.b = white.a = 255;

	for (int i = 0; i < 4; i++) {
		four_whites[i] = white;
	}

	SDL_RWclose(file);
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

void draw_window(float x, float y, float w, float h, bool arrow, bool circle)
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

	if (circle) {
		speech_arrow->set_animation("circle");
		speech_arrow->get_current_image()->draw_single(x+w-12, y+h-12, 0);
	}
	else if (arrow) {
		speech_arrow->set_animation("arrow");
		speech_arrow->get_current_image()->draw_single(x+w-12, y+h-12, 0);
	}
}
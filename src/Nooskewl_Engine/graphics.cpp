#include "Nooskewl_Engine/global.h"
#include "Nooskewl_Engine/graphics.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/log.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/util.h"
#include "Nooskewl_Engine/vertex_accel.h"
#include "Nooskewl_Engine/video.h"

using namespace Nooskewl_Engine;

static Image *window_image;
static Sprite *speech_arrow;

namespace Nooskewl_Engine {

void draw_line(Point<int> a, Point<int> b, SDL_Colour colour)
{
	SDL_Colour vertex_colours[4];
	for (int i = 0; i < 4; i++) {
		vertex_colours[i] = colour;
	}
	float x1 = (float)a.x;
	float y1 = (float)a.y;
	float x2 = (float)b.x;
	float y2 = (float)b.y;
	float dx = x2 - x1;
	float dy = y2 - y1;
	float angle = atan2(dy, dx);
	float a1 = angle + PI / 2.0f;
	float a2 = angle - PI / 2.0f;
	// FIXME: 4 hardcoded
	float scale = 0.5f;
	Point<float> da = a;
	Point<float> db = a;
	Point<float> dc = b;
	Point<float> dd = b;
	da.x += cos(a1) * scale;
	da.y += sin(a1) * scale;
	db.x += cos(a2) * scale;
	db.y += sin(a2) * scale;
	dc.x += cos(a1) * scale;
	dc.y += sin(a1) * scale;
	dd.x += cos(a2) * scale;
	dd.y += sin(a2) * scale;
	if (g.graphics.opengl) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	g.graphics.vertex_accel->start();
	g.graphics.vertex_accel->buffer(Point<int>(0, 0), Size<int>(0, 0), da, dc, dd, db, vertex_colours, 0);
	g.graphics.vertex_accel->end();
}

void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4])
{
	if (g.graphics.opengl) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	g.graphics.vertex_accel->start();
	g.graphics.vertex_accel->buffer(Point<int>(0, 0), Size<int>(0, 0), dest_position, dest_size, vertex_colours, 0);
	g.graphics.vertex_accel->end();
}

void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour colour)
{
	static SDL_Colour vertex_colours[4];
	for (int i = 0; i < 4; i++) {
		vertex_colours[i] = colour;
	}
	draw_quad(dest_position, dest_size, vertex_colours);
}

void draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle)
{
	SDL_Colour vertex_colours[4];

	// Blue shades in NES palette
	vertex_colours[0] = vertex_colours[1] = g.graphics.colours[47];
	vertex_colours[2] = vertex_colours[3] = g.graphics.colours[44];

	for (int i = 0; i < 4; i++) {
		vertex_colours[i].a = 220;
	}

	draw_quad(dest_position+1, dest_size-2, vertex_colours);

	window_image->start();
	window_image->draw_region(Point<int>(0, 0), Size<int>(6, 6), dest_position, 0); // top left
	window_image->draw_region(Point<int>(6, 0), Size<int>(6, 6), Point<int>(dest_position.x+dest_size.w-6, dest_position.y), 0); // top right
	window_image->draw_region(Point<int>(6, 6), Size<int>(6, 6), dest_position+dest_size-6, 0); // bottom right
	window_image->draw_region(Point<int>(0, 6), Size<int>(6, 6), Point<int>(dest_position.x, dest_position.y+dest_size.h-6), 0); // bottom left
	window_image->stretch_region(Point<int>(5, 1), Size<int>(2, 4), Point<int>(dest_position.x+6, dest_position.y+1), Size<int>(dest_size.w-12, 4), 0); // top
	window_image->stretch_region(Point<int>(5, 7), Size<int>(2, 4), Point<int>(dest_position.x+6, dest_position.y+dest_size.h-5), Size<int>(dest_size.w-12, 4), 0); // bottom
	window_image->stretch_region(Point<int>(1, 5), Size<int>(4, 2), Point<int>(dest_position.x+1, dest_position.y+6), Size<int>(4, dest_size.h-12), 0); // left
	window_image->stretch_region(Point<int>(7, 5), Size<int>(4, 2), Point<int>(dest_position.x+dest_size.w-5, dest_position.y+6), Size<int>(4, dest_size.h-12), 0); // right
	window_image->end();

	if (circle) {
		speech_arrow->set_animation("circle");
		speech_arrow->get_current_image()->draw_single(dest_position+dest_size-12, 0);
	}
	else if (arrow) {
		speech_arrow->set_animation("arrow");
		speech_arrow->get_current_image()->draw_single(dest_position+dest_size-12, 0);
	}
}

} // End namespace Nooskewl_Engine

void init_graphics()
{
	// FIXME: exceptions!
	try {
		window_image = new Image("window.tga");
		speech_arrow = new Sprite("speech_arrow");
		speech_arrow->start();
		load_palette("nes.gpl");
	}
	catch (Error e) {
		delete window_image;
		window_image = NULL;
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
		int red, green, blue;
		if (sscanf(line, "%d %d %d", &red, &green, &blue) == 3) {
			g.graphics.colours[colour_count].r = red;
			g.graphics.colours[colour_count].g = green;
			g.graphics.colours[colour_count].b = blue;
			g.graphics.colours[colour_count].a = 255;
			colour_count++;
		}
		else {
			infomsg("Syntax error on line %d of %s\n", line_count, name.c_str());
		}
	}

	g.graphics.black.r = g.graphics.black.g = g.graphics.black.b = 0;
	g.graphics.black.a = 255;
	g.graphics.white.r = g.graphics.white.g = g.graphics.white.b = g.graphics.white.a = 255;

	for (int i = 0; i < 4; i++) {
		g.graphics.four_blacks[i] = g.graphics.black;
		g.graphics.four_whites[i] = g.graphics.white;
	}

	SDL_RWclose(file);
}
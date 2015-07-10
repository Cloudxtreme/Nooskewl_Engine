#include "Nooskewl_Engine/Nooskewl_Engine.h"

using namespace Nooskewl_Engine;

namespace Nooskewl_Engine {

Engine noo;

Engine::Engine()
{
}

Engine::~Engine()
{
	delete noo.map;

	delete window_image;

	delete noo.font;
	delete noo.bold_font;
	TTF_Quit();

	shutdown_video();
	shutdown_audio();

	delete noo.cpa;

	if (joy && SDL_JoystickGetAttached(joy)) {
		SDL_JoystickClose(joy);
	}

	SDL_Quit();
}

void Engine::start(int argc, char **argv)
{
	noo.mute = check_args(argc, argv, "+mute");

	load_dll();

	int flags = SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_VIDEO;
	if (noo.mute == false) {
		flags |= SDL_INIT_AUDIO;
	}

	if (SDL_Init(flags) != 0) {
		throw Error("SDL_Init failed");
	}

	if (SDL_NumJoysticks() > 0) {
		joy = SDL_JoystickOpen(0);
	}
	else {
		joy = NULL;
	}

	noo.cpa = new CPA();

	init_audio(argc, argv);
	init_video(argc, argv);

	if (TTF_Init() == -1) {
		throw Error("TTF_Init failed");
	}
	noo.font = new Font("fff_majestica.ttf", 8);
	noo.bold_font = new Font("fff_majestica_bold.ttf", 8);

	window_image = new Image("window.tga");
	speech_arrow = new Sprite("speech_arrow");
	speech_arrow->start();
	load_palette("nes.gpl");

	noo.map = new Map("test.map");

	Player_Brain *player_brain = new Player_Brain();
	noo.player = new Map_Entity(player_brain);
	noo.player->load_sprite("player");
	noo.player->set_position(Point<int>(1, 3));
	noo.map->add_entity(noo.player);
}

void Engine::handle_event(TGUI_Event *event)
{
	noo.map->handle_event(event);
}

bool Engine::update()
{
	speech_arrow->update();

	if (noo.map->update() == false) {
		std::string map_name;
		Point<int> position;
		Direction direction;
		noo.map->get_new_map_details(map_name, position, direction);
		if (map_name != "") {
			Map *old_map = noo.map;
			noo.map = new Map(map_name);
			noo.map->add_entity(noo.player);

			// draw transition

			const Uint32 duration = 500;
			Uint32 start_time = SDL_GetTicks();
			Uint32 end_time = start_time + duration;
			bool moved_player = false;

			while (SDL_GetTicks() < end_time) {
				Uint32 elapsed = SDL_GetTicks() - start_time;
				if (moved_player == false && elapsed >= duration/2) {
					// The actual moving happens below in this same loop
					moved_player = true;
					noo.player->set_position(position);
					noo.player->set_direction(direction);
				}

				set_map_transition_projection((float)elapsed / duration * PI);

				clear(noo.black);

				m.vertex_cache->set_perspective_drawing(true);
				if (moved_player) {
					noo.map->update_camera();
					noo.map->draw();
				}
				else {
					old_map->update_camera();
					old_map->draw();
				}
				m.vertex_cache->set_perspective_drawing(false);

				flip();
			}

			set_default_projection();

			delete old_map;
		}
		else {
			return false;
		}
	}

	return true;
}


void Engine::draw()
{
	clear(noo.black);

	noo.map->draw();

	noo.font->draw(noo.white, "This is the most insane time to live!", Point<int>(0, 0));
	SDL_Colour green = { 0, 255, 0, 255 };
	noo.font->draw(green, "This is the most insane time to live!", Point<int>(0, 15));

	flip();
}

void Engine::draw_line(Point<int> a, Point<int> b, SDL_Colour colour)
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
	if (noo.opengl) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	m.vertex_cache->start();
	m.vertex_cache->buffer(Point<int>(0, 0), Size<int>(0, 0), da, dc, dd, db, vertex_colours, 0);
	m.vertex_cache->end();
}

void Engine::draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4])
{
	if (noo.opengl) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	m.vertex_cache->start();
	m.vertex_cache->buffer(Point<int>(0, 0), Size<int>(0, 0), dest_position, dest_size, vertex_colours, 0);
	m.vertex_cache->end();
}

void Engine::draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour colour)
{
	static SDL_Colour vertex_colours[4];
	for (int i = 0; i < 4; i++) {
		vertex_colours[i] = colour;
	}
	draw_quad(dest_position, dest_size, vertex_colours);
}

void Engine::draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle)
{
	SDL_Colour vertex_colours[4];

	// Blue shades in NES palette
	vertex_colours[0] = vertex_colours[1] = noo.colours[47];
	vertex_colours[2] = vertex_colours[3] = noo.colours[44];

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

void Engine::load_palette(std::string name)
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
			noo.colours[colour_count].r = red;
			noo.colours[colour_count].g = green;
			noo.colours[colour_count].b = blue;
			noo.colours[colour_count].a = 255;
			colour_count++;
		}
		else {
			infomsg("Syntax error on line %d of %s\n", line_count, name.c_str());
		}
	}

	noo.black.r = noo.black.g = noo.black.b = 0;
	noo.black.a = 255;
	noo.white.r = noo.white.g = noo.white.b = noo.white.a = 255;

	for (int i = 0; i < 4; i++) {
		noo.four_blacks[i] = noo.black;
		noo.four_whites[i] = noo.white;
	}

	SDL_RWclose(file);
}

} // End namespace Nooskewl_Engine
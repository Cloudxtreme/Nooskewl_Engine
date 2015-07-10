#include "Nooskewl_Engine/Nooskewl_Engine.h"

using namespace Nooskewl_Engine;

namespace Nooskewl_Engine {

Engine::Engine(int argc, char **argv)
{
	g.audio.mute = check_args(argc, argv, "+mute");

	load_dll();

	int flags = SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_VIDEO;
	if (g.audio.mute == false) {
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

	g.cpa = new CPA();

	init_audio(argc, argv);
	init_video(argc, argv);
	init_font();
	init_graphics();

	g.map = new Map("test.map");
	g.map->start();

	Player_Brain *player_brain = new Player_Brain();
	g.player = new Map_Entity(player_brain);
	g.player->load_sprite("player");
	g.player->set_position(Point<int>(1, 3));
	g.map->add_entity(g.player);
}

Engine::~Engine()
{
	g.map->end();
	delete g.map;

	shutdown_graphics();
	shutdown_font();
	shutdown_video();
	shutdown_audio();

	delete g.cpa;

	if (joy && SDL_JoystickGetAttached(joy)) {
		SDL_JoystickClose(joy);
	}

	SDL_Quit();
}

void Engine::handle_event(TGUI_Event *event)
{
	g.map->handle_event(event);
}

bool Engine::update()
{
	update_graphics();

	if (g.map->update() == false) {
		std::string map_name;
		Point<int> position;
		Direction direction;
		g.map->get_new_map_details(map_name, position, direction);
		if (map_name != "") {
			Map *old_map = g.map;
			g.map = new Map(map_name);
			g.map->start();
			g.map->add_entity(g.player);

			// draw transition

			const Uint32 duration = 500;
			Uint32 start_time = SDL_GetTicks();
			Uint32 end_time = start_time + duration;
			bool moved_player = false;

			while (SDL_GetTicks() < end_time) {
				Uint32 elapsed = SDL_GetTicks() - start_time;
				if (moved_player == false && elapsed >= duration/2) {
					moved_player = true;
					g.player->set_position(position);
					g.player->set_direction(direction);
				}

				set_map_transition_projection((float)elapsed / duration * PI);

				clear(g.graphics.black);

				g.graphics.vertex_accel->set_perspective_drawing(true);
				if (moved_player) {
					g.map->update_camera();
					g.map->draw();
				}
				else {
					old_map->update_camera();
					old_map->draw();
				}
				g.graphics.vertex_accel->set_perspective_drawing(false);

				flip();
			}

			set_default_projection();

			old_map->end();
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
	clear(g.graphics.black);

	g.map->draw();

	g.graphics.font->draw("HELLO!", Point<int>(11, 11), g.graphics.black);
	g.graphics.font->draw("HELLO!", Point<int>(10, 10), g.graphics.white);

	flip();
}

} // End namespace Nooskewl_Engine
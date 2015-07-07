#include "audio.h"
#include "cpa.h"
#include "font.h"
#include "graphics.h"
#include "log.h"
#include "map.h"
#include "map_entity.h"
#include "player_brain.h"
#include "vertex_accel.h"
#include "video.h"
#include "widgets.h"

Map *map;
Map_Entity *player;

SDL_Joystick *joy;

bool run_main();

int main(int argc, char **argv)
{
	try {
		run_main();
	}
	catch (Error e) {
		errormsg("Fatal error: %s\n", e.error_message.c_str());
		return 1;
	}

	return 0;
}

static Uint32 main_callback(Uint32 interval, void *data)
{
	SDL_Event event;
	SDL_UserEvent userevent;

	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);

    return interval;
}

static bool run_main()
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
		throw Error("SDL_Init failed");
	}

	if (SDL_NumJoysticks() > 0) {
		joy = SDL_JoystickOpen(0);
	}

	cpa = new CPA("ss.cpa");

	init_audio();
	init_video();
	init_font();
	init_graphics();

	map = new Map("test.map");

	Player_Brain *player_brain = new Player_Brain();
	player = new Map_Entity(player_brain);
	player->load_sprite("player");
	player->set_position(Point<int>(1, 3));
	map->add_entity(player);

	Audio music = load_audio("title.mml");
	play_audio(music, true);

	SDL_AddTimer(16, main_callback, NULL);

	bool quit = false;
	bool draw = false;

	SS_Div *main_div = new SS_Div(1.0f, 1.0f);
	SS_Div *child1 = new SS_Div(0.25f, 50);
	child1->set_parent(main_div);
	child1->set_padding(10);
	SS_Div *child2 = new SS_Div(-1.0f, 75);
	child2->set_parent(main_div);
	SS_Div *child3 = new SS_Div(1.0f, -1.0f);
	child3->set_parent(main_div);
	SS_Div *child4 = new SS_Div(50, -1.0f);
	child4->set_parent(child3);
	child4->set_padding(10);
	child4->set_float_right(true);
	SS_Div *child5 = new SS_Div(10, 10);
	child5->set_parent(child3);
	child5->set_padding(10);
	child5->set_float_right(true);
	TGUI *gui = new TGUI(main_div, screen_w, screen_h);
	gui->layout();

	gui->set_focus(child5);

	while (quit == false) {
		bool got_event = false;
		while (true) {
			SDL_Event sdl_event;
			if (SDL_PollEvent(&sdl_event)) {
				got_event = true;
			}
			else {
				break;
			}

			TGUI_Event event = tgui_sdl_convert_event(&sdl_event);
			// FIXME: process function
			if (event.type == TGUI_MOUSE_DOWN || event.type == TGUI_MOUSE_UP || event.type == TGUI_MOUSE_AXIS) {
				event.mouse.x /= 4;
				event.mouse.y /= 4;
			}
			gui->handle_event(&event);

			if (sdl_event.type == SDL_QUIT) {
				quit = true;
				break;
			}
			else if (sdl_event.type == SDL_USEREVENT) {
				update_graphics();
				if (map->update() == false) {
					std::string map_name;
					Point<int> position;
					Direction direction;
					map->get_new_map_details(map_name, position, direction);
					if (map_name != "") {
						Map *old_map = map;
						map = new Map(map_name);
						map->add_entity(player);

						// draw transition

						const Uint32 duration = 500;
						Uint32 start_time = SDL_GetTicks();
						Uint32 end_time = start_time + duration;
						bool moved_player = false;

						while (SDL_GetTicks() < end_time) {
							Uint32 elapsed = SDL_GetTicks() - start_time;
							if (moved_player == false && elapsed >= duration/2) {
								moved_player = true;
								player->set_position(position);
								player->set_direction(direction);
							}

							set_map_transition_projection((float)elapsed / duration * PI);

							glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
							glClear(GL_COLOR_BUFFER_BIT);

							vertex_accel->set_perspective_drawing(true);
							if (moved_player) {
								map->update();
								map->draw();
							}
							else {
								old_map->update();
								old_map->draw();
							}
							vertex_accel->set_perspective_drawing(false);

							flip();
						}

						set_default_projection();

						delete old_map;
					}
					else {
						quit = true;
					}
				}
				draw = true;
			}

			map->handle_event(&event);

			/*
			if (event.type == TGUI_MOUSE_AXIS) {
				gui->resize(event.mouse.x/4, event.mouse.y/4);
			}
			*/
		}

		if (quit) {
			break;
		}

		if (draw) {
			draw = false;

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			map->draw();

			gui->draw();

			flip();
		}
		else if (!got_event) {
			SDL_Delay(1);
		}
	}

	delete map;

	shutdown_graphics();
	shutdown_font();
	shutdown_video();
	shutdown_audio();

	if (SDL_JoystickGetAttached(joy)) {
		SDL_JoystickClose(joy);
	}
	
	return true;
}
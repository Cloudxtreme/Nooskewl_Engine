#include "audio.h"
#include "font.h"
#include "graphics.h"
#include "log.h"
#include "map.h"
#include "map_entity.h"
#include "player_brain.h"
#include "video.h"

Map *map;
Map_Entity *player;

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
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
		throw Error("SDL_Init failed");
	}

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

	SDL_AddTimer(16, main_callback, NULL);

	bool quit = false;
	bool draw = false;

	while (quit == false) {
		bool got_event = false;
		while (true) {
			SDL_Event event;
			if (SDL_PollEvent(&event)) {
				got_event = true;
			}
			else {
				break;
			}
			if (event.type == SDL_QUIT) {
				quit = true;
				break;
			}
			else if (event.type == SDL_USEREVENT) {
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
						player->set_position(position);
						player->set_direction(direction);
						delete old_map;
					}
					else {
						quit = true;
					}
				}
				draw = true;
			}
			map->handle_event(&event);
		}

		if (quit) {
			break;
		}

		if (draw) {
			draw = false;

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			map->draw();

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
	
	return true;
}
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
	player->load_animation_set("sprites/player");
	player->set_map(map);
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
				map->update();
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

			draw_window(5, screen_h/2, screen_w-10, screen_h/2-5);

			std::string text = "I've been wandering around this room for three days... there are no windows or doors, just bright blue walls. All I can do is dance. And I hate dancing. When I get out of here... IF I get out of here... I'm going to eat 4 big bags of chips!";
			int skip = font->draw_wrapped(text, 14, screen_h/2+9, screen_w-28, 14, 4, white);
			font->draw("\"" + text.substr(skip) + "\"", 0, 0, white);

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
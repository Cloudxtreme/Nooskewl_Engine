#include "audio.h"
#include "font.h"
#include "graphics.h"
#include "log.h"
#include "map.h"
#include "map_entity.h"
#include "player_brain.h"
#include "video.h"

bool run_main();

int main(int argc, char **argv)
{
	try {
		if (run_main() == false) {
			return 1;
		}
	}
	catch (Error e) {
		errormsg("Fatal error: %s\n", e.error_message.c_str());
		return 1;
	}

	return 0;
}

bool run_main()
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		return false;
	}

	init_audio();
	init_video();
	init_font();
	init_graphics();

	Map *map = new Map("test.map");

	Player_Brain *player_brain = new Player_Brain();

	Map_Entity *player = new Map_Entity(player_brain);
	player->load_animation_set("sprites/player");
	player->set_map(map);
	player->set_position(Point<int>(1, 3));

	map->add_entity(player);

	// FIXME!!!!!!!!!! TIMER

	while (1) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
			map->handle_event(&event);
		}

		map->update();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		map->draw();

		draw_window(5, screen_h/2, screen_w-10, screen_h/2-5);

		font->draw("I've been wandering around this room for three days... there", 14, screen_h/2+9, four_whites[0]);
		font->draw("are no windows or doors, just bright blue walls. All I can do", 14, screen_h/2+9+14, four_whites[0]);
		font->draw("is dance. And I hate dancing. When I get out of here... IF I", 14, screen_h/2+9+28, four_whites[0]);
		font->draw("get out of here...", 14, screen_h/2+9+42, four_whites[0]);

		flip();
	}

	delete map;

	shutdown_audio();
	shutdown_font();
	shutdown_graphics();
	shutdown_video();
	
	return true;
}
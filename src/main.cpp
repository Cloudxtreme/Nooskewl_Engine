#include "audio.h"
#include "font.h"
#include "graphics.h"
#include "log.h"
#include "map.h"
#include "map_entity.h"
#include "player_brain.h"
#include "video.h"

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		return 1;
	}

	if (!init_audio()) {
		errormsg("Error initializing audio\n");
		return 1;
	}

	if (!init_video()) {
		errormsg("Error initializing video\n");
		return 1;
	}

	if (!init_font()) {
		errormsg("Error initializing fonts\n");
		return 1;
	}

	if (!init_graphics()) {
		errormsg("Error initializing graphics\n");
		return 1;
	}

	Font *font = new Font();
	if (font->load_ttf("fonts/majestica.ttf", 8) == false) {
		errormsg("Error loading fonts/majestica.ttf\n");
		return 1;
	}

	Map *map = new Map();
	if (map->load("test.map") == false) {
		errormsg("Couldn't load test.map\n");
		return 1;
	}

	Player_Brain *player_brain = new Player_Brain();
	if (player_brain == NULL) {
		errormsg("Error allocating player brain\n");
		return 1;
	}

	Map_Entity *player = new Map_Entity(player_brain);
	if (player == NULL || player->load_animation_set("sprites/player") == false) {
		errormsg("Error loading player\n");
		return 1;
	}
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

		SDL_Colour white = { 255, 255, 255, 255 };
		font->draw("Hello, world!", 12, screen_h/2+7, white);

		flip();
	}

	delete map;

	shutdown_audio();
	shutdown_font();
	shutdown_graphics();
	shutdown_video();
	
	return 0;
}
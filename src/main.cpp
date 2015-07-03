#include "starsquatters.h"
#include "animation_set.h"
#include "audio.h"
#include "brain.h"
#include "font.h"
#include "image.h"
#include "log.h"
#include "map_entity.h"
#include "tilemap.h"
#include "video.h"

// FIXME
#include <Mmsystem.h>

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		return 1;
	}

	if (!init_audio()) {
		errormsg("Error initializing audio");
		return 1;
	}

	if (!init_video()) {
		errormsg("Error initializing video");
		return 1;
	}

	if (!init_font()) {
		errormsg("Error initializing fonts");
		return 1;
	}

	Tilemap *tilemap = new Tilemap(8);
	if (tilemap->load("test", "test.level") == false) {
		errormsg("Error loading tilemap");
		return 1;
	}

	Player_Brain *player_brain = new Player_Brain();
	if (player_brain == NULL) {
		errormsg("Error allocating player brain");
		return 1;
	}

	Map_Entity *player = new Map_Entity(player_brain);
	if (player == NULL || player->load_animation_set("player") == false) {
		errormsg("Error loading player");
		return 1;
	}

	while (1) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
			player_brain->update((void *)&event);
		}

		player->update();

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (int i = 0; i < tilemap->get_layer_count(); i++) {
			tilemap->draw_layer(i, 0, 0);
		}

		player->draw();

		flip();
	}

	delete tilemap;

	shutdown_audio();
	shutdown_font();
	shutdown_video();
	
	return 0;
}
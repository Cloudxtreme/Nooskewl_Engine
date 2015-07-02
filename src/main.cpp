#include "starsquatters.h"
#include "animation_set.h"
#include "audio.h"
#include "font.h"
#include "image.h"
#include "log.h"
#include "tilemap.h"
#include "vertex_accel.h"
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

	Image *img = new Image();
	if (img->load_tga("test.tga") == false) {
		errormsg("Error loading test.tga");
		return 1;
	}

	Sample *sample = new Sample();
	if (sample->load_wav("test.wav") == false) {
		errormsg("Error loading test.wav");
		return 1;
	}

	Font *font = new Font();
	if (font->load_ttf("C:\\Windows\\Fonts\\arial.ttf", 16) == false) {
		errormsg("Error loading font");
		return 1;
	}

	Audio music = load_audio("title.mml");
	play_audio(music, true);

	Tilemap *tilemap = new Tilemap(8);
	if (tilemap->load("test", "test.level") == false) {
		errormsg("Error loading tilemap");
		return 1;
	}

	vertex_accel = new Vertex_Accel();
	if (vertex_accel->init() == false) {
		errormsg("Couldn't create vertex accelerator");
		return 1;
	}

	Animation_Set *anim = new Animation_Set();
	if (anim->load("test.xml", "anim") == false) {
		errormsg("Error loading animation set");
		return 1;
	}
	anim->start();

	while (1) {
		int start = timeGetTime();
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
			else if (event.type == SDL_KEYDOWN) {
				sample->play(1.0f, false);
			}
		}

		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/*
		img->start();
		for (int i = 0; i < 1000; i++) {
			img->draw(rand()%1024, rand()%768, 0);
		}
		img->end();
		*/

		img->start();
		img->draw_region(0, 0, 16, 16, 10, 10, 0);
		img->draw_region(16, 0, 16, 16, 50, 50, 0);
		img->draw(100, 100, 0);
		img->end();

		Image *frame = anim->get_current_image();
		frame->start();
		frame->draw(150, 150, 0);
		frame->end();

		SDL_Colour colour = { 255, 255, 255, 255 };
		font->draw("HELLO, WORLD!", 200, 200, colour);

		for (int i = 0; i < tilemap->get_layer_count(); i++) {
			tilemap->draw_layer(i, 300, 300);
		}

		flip();
	}

	delete img;

	shutdown_audio();
	shutdown_video();
	shutdown_font();
	
	return 0;
}
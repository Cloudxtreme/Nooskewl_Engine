#include "starsquatters.h"

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		return 1;
	}

	if (!init_audio()) {
		logmsg("Error initializing audio");
		return 1;
	}

	if (!init_video()) {
		logmsg("Error initializing video");
		return 1;
	}

	if (!init_font()) {
		logmsg("Error initializing fonts");
		return 1;
	}

	Image *img = new Image();
	SDL_RWops *file = SDL_RWFromFile("test.tga", "rb");
	if (file == NULL || img->load_tga(file) == false) {
		logmsg("Error loading test.tga");
		return 1;
	}

	Sample *sample = new Sample();
	file = SDL_RWFromFile("test.wav", "rb");
	if (file == NULL || sample->load_wav(file) == false) {
		logmsg("Error loading test.wav");
		return 1;
	}

	Font *font = new Font();
	file = SDL_RWFromFile("arial.ttf", "rb");
	if (file == NULL || font->load_ttf(file, 16) == false) {
		logmsg("Error loading font");
		return 1;
	}

	while (1) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
			else if (event.type == SDL_KEYDOWN) {
				sample->play(1.0f, false);
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		img->bind();
		img->draw_region(0, 0, 16, 16, 10, 10, 0);
		img->draw_region(16, 0, 16, 16, 50, 50, 0);
		img->draw(100, 100, 0);

		SDL_Colour colour = { 255, 255, 255, 255 };
		font->draw("HELLO, WORLD!", 200, 200, colour);

		flip();
	}

	delete img;

	shutdown_audio();
	shutdown_video();
	shutdown_font();
	
	return 0;
}
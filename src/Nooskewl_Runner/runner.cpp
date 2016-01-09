#include "Nooskewl_Engine/Nooskewl_Engine.h"

using namespace Nooskewl_Engine;

static bool run_main(int argc, char **argv);

int main(int argc, char **argv)
{
	try {
		if (run_main(argc, argv) == false) {
			printf("run_main returned false\n");
		}
	}
	catch (Error e) {
		printf("Fatal error: %s\n", e.error_message.c_str());
		return 1;
	}

	return 0;
}

static bool run_main(int argc, char **argv)
{
#ifdef NOOSKEWL_ENGINE_WINDOWS
	SDL_RegisterApp("MORPG3", 0, 0);
#endif

	if (noo.start(argc, argv) == false) {
		return false;
	}

	bool quit = false;
	bool draw = false;

	// These keep the logic running at around 60 FPS even if drawing isn't
	Uint32 start = SDL_GetTicks();
	int frames = 0;
	bool skip = false;

	while (quit == false) {
		// LOGIC
		if (noo.update() == false) {
			break;
		}

		// EVENTS
		SDL_Event sdl_event;

		while (SDL_PollEvent(&sdl_event)) {
			if (noo.handle_event(&sdl_event) == false) {
				quit = true;
				break;
			}
		}

		if (quit) {
			break;
		}

		if (skip == false) {
			noo.draw();
		}

		frames++;

		int diff = SDL_GetTicks() - start;
		float average;
		if (diff > 0) {
			average = frames / (diff / 1000.0f);
		}
		else {
			average = 60.0f;
		}

		if (average < 59.0f) { // allow a little bit of fluctuation, i.e., not 60.0f here
			skip = true;
		}
		else {
			skip = false;
		}
	}

	noo.end();

	return true;
}

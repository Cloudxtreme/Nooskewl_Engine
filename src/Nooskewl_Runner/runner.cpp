#include "Nooskewl_Engine/Nooskewl_Engine.h"

using namespace Nooskewl_Engine;

static bool run_main(int argc, char **argv);

int main(int argc, char **argv)
{
	try {
		run_main(argc, argv);
	}
	catch (Error e) {
		printf("Fatal error: %s\n", e.error_message.c_str());
		return 1;
	}

	return 0;
}

static bool run_main(int argc, char **argv)
{
	noo.start(argc, argv);

	bool quit = false;
	bool draw = false;

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

		noo.draw();
	}

	noo.end();

	return true;
}
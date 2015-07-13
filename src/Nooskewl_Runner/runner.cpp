#include "Nooskewl_Engine/Nooskewl_Engine.h"

using namespace Nooskewl_Engine;

const Uint32 TICKS_PER_FRAME = (1000 / 60);

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
	int accumulated_delay = 0;

	Uint32 last_frame = SDL_GetTicks();

	while (quit == false) {
		// LOGIC
		if (noo.update() == false) {
			break;
		}

		// EVENTS
		SDL_Event sdl_event;

		while (SDL_PollEvent(&sdl_event)) {
			if (sdl_event.type == SDL_QUIT) {
				quit = true;
				break;
			}
			else if (sdl_event.type == SDL_WINDOWEVENT && sdl_event.window.event == SDL_WINDOWEVENT_RESIZED) {
				noo.set_screen_size(sdl_event.window.data1, sdl_event.window.data2);
				noo.set_default_projection();
			}

			// FIXME: move GUI into engine
			TGUI_Event event = tgui_sdl_convert_event(&sdl_event);

			if (event.type == TGUI_MOUSE_DOWN || event.type == TGUI_MOUSE_UP || event.type == TGUI_MOUSE_AXIS) {
				event.mouse.x /= noo.scale;
				event.mouse.y /= noo.scale;
			}

			noo.handle_event(&event);
		}

		noo.draw();

		// TIMING
		// This code is ugly for a reason
		Uint32 now = SDL_GetTicks();
		int elapsed = now - last_frame;
		if (elapsed < TICKS_PER_FRAME) {
			int wanted_delay = TICKS_PER_FRAME - elapsed;
			int final_delay = wanted_delay + accumulated_delay;
			if (final_delay > 0) {
				SDL_Delay(final_delay);
				elapsed = SDL_GetTicks() - now;
				accumulated_delay -= elapsed - wanted_delay;
			}
			else {
				accumulated_delay += elapsed;
			}
			if (accumulated_delay > 100 || accumulated_delay < -100) {
				accumulated_delay = 0;
			}
		}
		last_frame = SDL_GetTicks();
	}

	noo.end();

	return true;
}
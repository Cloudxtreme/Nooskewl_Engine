#include "Nooskewl_Engine/Nooskewl_Engine.h"

const Uint32 TICKS_PER_FRAME = (1000 / 60);

bool run_main(int argc, char **argv);

int main(int argc, char **argv)
{
	try {
		run_main(argc, argv);
	}
	catch (Error e) {
		errormsg("Fatal error: %s\n", e.error_message.c_str());
		return 1;
	}

	return 0;
}

static bool run_main(int argc, char **argv)
{
	init_nooskewl_engine(argc, argv);

	Audio music = load_audio("title.mml");
	play_audio(music, true);

	SS_Widget *main_widget = new SS_Widget(1.0f, 1.0f);
	SS_Widget *child1 = new SS_Widget(50, 50);
	child1->set_parent(main_widget);
	child1->set_padding(10);
	child1->set_accepts_focus(true);
	SS_Widget *child11 = new SS_Widget(20, 20);
	child11->set_parent(child1);
	SS_Widget *child2 = new SS_Widget(-1.0f, 75);
	child2->set_parent(main_widget);
	SS_Widget *child3 = new SS_Widget(1.0f, -1.0f);
	child3->set_parent(main_widget);
	SS_Widget *child4 = new SS_Widget(50, -1.0f);
	child4->set_parent(child3);
	child4->set_padding(10);
	child4->set_float_right(true);
	child4->set_accepts_focus(true);
	SS_Widget *child5 = new SS_Widget(10, 10);
	child5->set_parent(child3);
	child5->set_padding(10);
	child5->set_float_right(true);
	child5->set_accepts_focus(true);
	SS_Widget *child6 = new SS_Widget(0.25f, 1.0f);
	child6->set_parent(child3);
	child6->set_accepts_focus(true);
	TGUI *gui = new TGUI(main_widget, g.graphics.screen_w, g.graphics.screen_h);

	bool quit = false;
	bool draw = false;
	int accumulated_delay = 0;

	Uint32 last_frame = SDL_GetTicks();

	while (quit == false) {
		// LOGIC
		if (update_nooskewl_engine() == false) {
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
				g.graphics.screen_w = sdl_event.window.data1 / 4;
				g.graphics.screen_h = sdl_event.window.data2 / 4;
				set_default_projection();
				gui->resize(g.graphics.screen_w, g.graphics.screen_h);
			}

			// FIXME: move GUI into engine
			TGUI_Event event = tgui_sdl_convert_event(&sdl_event);

			if (event.type == TGUI_MOUSE_DOWN || event.type == TGUI_MOUSE_UP || event.type == TGUI_MOUSE_AXIS) {
				event.mouse.x /= 4;
				event.mouse.y /= 4;
			}

			nooskewl_engine_handle_event(&event);

			gui->handle_event(&event);
		}

		nooskewl_engine_draw();

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

	shutdown_nooskewl_engine();

	return true;
}
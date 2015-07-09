#include "Nooskewl_Engine/Nooskewl_Engine.h"

const int64_t TICKS_PER_FRAME = (1000 / 60);

Map *map;
Map_Entity *player;

bool run_main();

int main(int argc, char **argv)
{
	try {
		init_nooskewl_engine(argc, argv);

		run_main();
	}
	catch (Error e) {
		errormsg("Fatal error: %s\n", e.error_message.c_str());
		return 1;
	}

	return 0;
}

static Uint32 timer_callback(Uint32 interval, void *data)
{
	SDL_Event event;
	SDL_UserEvent userevent;

	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);

	int64_t *timer_last_call = (int64_t *)data;
	int64_t now = SDL_GetTicks();
	int64_t elapsed = now - *timer_last_call;
	*timer_last_call = now;
	int64_t over = elapsed - TICKS_PER_FRAME;

	if (over > 0 && over < TICKS_PER_FRAME) {
		return TICKS_PER_FRAME - over;
	}
	else {
		return TICKS_PER_FRAME;
	}
}

static bool run_main()
{
	int64_t start_frame = SDL_GetTicks();

	map = new Map("test.map");
	map->start();

	Player_Brain *player_brain = new Player_Brain();
	player = new Map_Entity(player_brain);
	player->load_sprite("player");
	player->set_position(Point<int>(1, 3));
	map->add_entity(player);

	Audio music = load_audio("title.mml");
	play_audio(music, true);

	int64_t timer_last_call = SDL_GetTicks();
	SDL_AddTimer(16, timer_callback, &timer_last_call);

	bool quit = false;
	bool draw = false;

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
	TGUI *gui = new TGUI(main_widget, screen_w, screen_h);

	while (quit == false) {
		SDL_Event sdl_event;
		while (SDL_PollEvent(&sdl_event)) {
			TGUI_Event event = tgui_sdl_convert_event(&sdl_event);

			// FIXME: process function
			if (event.type == TGUI_MOUSE_DOWN || event.type == TGUI_MOUSE_UP || event.type == TGUI_MOUSE_AXIS) {
				event.mouse.x /= 4;
				event.mouse.y /= 4;
			}

			gui->handle_event(&event);

			if (sdl_event.type == SDL_QUIT) {
				quit = true;
				break;
			}
			else if (sdl_event.type == SDL_USEREVENT) {
				update_graphics();

				if (map->update() == false) {
					std::string map_name;
					Point<int> position;
					Direction direction;
					map->get_new_map_details(map_name, position, direction);
					if (map_name != "") {
						Map *old_map = map;
						map = new Map(map_name);
						map->start();
						map->add_entity(player);

						// draw transition

						const Uint32 duration = 500;
						Uint32 start_time = SDL_GetTicks();
						Uint32 end_time = start_time + duration;
						bool moved_player = false;

						while (SDL_GetTicks() < end_time) {
							Uint32 elapsed = SDL_GetTicks() - start_time;
							if (moved_player == false && elapsed >= duration/2) {
								moved_player = true;
								player->set_position(position);
								player->set_direction(direction);
							}

							set_map_transition_projection((float)elapsed / duration * PI);

							glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
							glClear(GL_COLOR_BUFFER_BIT);

							vertex_accel->set_perspective_drawing(true);
							if (moved_player) {
								map->update_camera();
								map->draw();
							}
							else {
								old_map->update_camera();
								old_map->draw();
							}
							vertex_accel->set_perspective_drawing(false);

							flip();
						}

						set_default_projection();

						old_map->end();
						delete old_map;
					}
					else {
						quit = true;
					}
				}

				draw = true;
			}
			else if (sdl_event.type == SDL_WINDOWEVENT && sdl_event.window.event == SDL_WINDOWEVENT_RESIZED) {
				screen_w = sdl_event.window.data1 / 4;
				screen_h = sdl_event.window.data2 / 4;
				printf("screen_w=%d screen_h=%d\n", screen_w, screen_h);
				glViewport(0, 0, sdl_event.window.data1, sdl_event.window.data2);
				set_default_projection();
				gui->resize(screen_w, screen_h);
			}

			map->handle_event(&event);
		}

		if (draw) {
			draw = false;

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			map->draw();

//			gui->draw();

			flip();

			int64_t now = SDL_GetTicks();
			int64_t elapsed = now - start_frame;
			int64_t delay = TICKS_PER_FRAME - elapsed;
			if (delay > 0) {
				SDL_Delay(delay);
			}
			start_frame = SDL_GetTicks();
		}
	}

	map->end();
	delete map;

	shutdown_nooskewl_engine();

	return true;
}
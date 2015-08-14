#include "Nooskewl_Engine/a_star.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/player_brain.h"
#include "Nooskewl_Engine/tilemap.h"

using namespace Nooskewl_Engine;

Player_Brain::Player_Brain() :
	pressed(false),
	dragged(false)
{
	reset();
}

Player_Brain::~Player_Brain()
{
}

void Player_Brain::handle_event(TGUI_Event *event)
{
	if (event->type == TGUI_KEY_DOWN) {
		if (event->keyboard.code == TGUIK_LEFT) {
			l = true;
		}
		else if (event->keyboard.code == TGUIK_RIGHT) {
			r = true;
		}
		else if (event->keyboard.code == TGUIK_UP) {
			u = true;
		}
		else if (event->keyboard.code == TGUIK_DOWN) {
			d = true;
		}
		else if (event->keyboard.code == noo.key_b1 || event->keyboard.code == TGUIK_RETURN) {
			b1 = true;
		}
	}
	else if (event->type == TGUI_KEY_UP) {
		if (event->keyboard.code == TGUIK_LEFT) {
			l = false;
		}
		else if (event->keyboard.code == TGUIK_RIGHT) {
			r = false;
		}
		else if (event->keyboard.code == TGUIK_UP) {
			u = false;
		}
		else if (event->keyboard.code == TGUIK_DOWN) {
			d = false;
		}
		else if (event->keyboard.code == noo.key_b1 || event->keyboard.code == TGUIK_RETURN) {
			b1 = false;
		}
	}
	else if (event->type == TGUI_JOY_AXIS) {
		if (event->joystick.axis == 0) {
			if (event->joystick.value < -0.5f) {
				l = true;
			}
			else {
				l = false;
			}
			if (event->joystick.value > 0.5f) {
				r = true;
			}
			else {
				r = false;
			}
		}
		else {
			if (event->joystick.value < -0.5f) {
				u = true;
			}
			else {
				u = false;
			}
			if (event->joystick.value > 0.5f) {
				d = true;
			}
			else {
				d = false;
			}
		}
	}
	else if (event->type == TGUI_JOY_DOWN) {
		if (event->joystick.button == noo.joy_b1) {
			b1 = true;
		}
	}
	else if (event->type == TGUI_JOY_UP) {
		if (event->joystick.button == noo.joy_b1) {
			b1 = false;
		}
	}
	else if (event->type == TGUI_MOUSE_DOWN) {
		if (noo.player->is_input_enabled()) {
			pressed = true;
			pressed_pos = Point<float>(event->mouse.x, event->mouse.y);
		}
	}
	else if (event->type == TGUI_MOUSE_UP) {
		if (noo.player->is_input_enabled()) {
			if (pressed) {
				if (dragged == false) {
					if (noo.player->is_sitting()) {
						noo.player->set_sitting(false);
					}
					else {
						Point<float> mouse_pos(event->mouse.x, event->mouse.y);
						float moved_distance = (mouse_pos-pressed_pos).length();
						if (moved_distance < (float)TOLERANCE) {
							Point<float> map_offset = noo.map->get_offset();
							Point<float> map_pan = noo.map->get_pan();
							Point<int> click((int)event->mouse.x, (int)event->mouse.y);
							Size<int> tilemap_size = noo.map->get_tilemap()->get_size() * noo.tile_size;
							click -= map_offset;
							click += map_pan;
							click /= noo.tile_size;
							Point<int> player_pos = noo.player->get_position();
							if (click.x >= 0 && click.y >= 0 && click.x < tilemap_size.w && click.y < tilemap_size.h) {
								int dx = click.x - player_pos.x;
								int dy = click.y - player_pos.y;
								bool activated = false;
								if ((abs(dx) <= 2 && dy == 0) || (abs(dy) <= 2 && dx == 0)) {
									std::vector<Map_Entity *> colliding_entities = noo.map->get_colliding_entities(-1, click, Size<int>(1, 1));
									if (colliding_entities.size() > 0) {
										Direction direction;
										if (dx < 0) {
											direction = W;
										}
										else if (dx > 0) {
											direction = E;
										}
										else if (dy < 0) {
											direction = N;
										}
										else {
											direction = S;
										}
										noo.player->set_direction(direction);
										activated = noo.map->activate(noo.player);
									}
								}
								if (activated == false) {
									std::list<A_Star::Node *> path = noo.map->find_path(player_pos, click);
									if (path.size() > 0) {
										noo.player->set_path(path);
									}
								}
							}
						}
					}
				}
			}
			pressed = false;
			dragged = false;
			noo.map->set_panning(false);
		}
	}
	else if (event->type == TGUI_MOUSE_AXIS) {
		if (noo.player->is_input_enabled()) {
			Point<float> mouse_pos(event->mouse.x, event->mouse.y);
			if (pressed == true && dragged == false) {
				float distance = (mouse_pos-pressed_pos).length();
				if (distance >= TOLERANCE) {
					dragged = true;
				}
			}
			if (dragged) {
				Point<float> moved = pressed_pos - mouse_pos;
				if (abs(moved.x) > noo.screen_size.w / 2.0f) {
					if (moved.x < 0.0f) {
						moved.x = -noo.screen_size.w / 2.0f;
					}
					else {
						moved.x = noo.screen_size.w / 2.0f;
					}
				}
				if (abs(moved.y) > noo.screen_size.h / 2.0f) {
					if (moved.y < 0.0f) {
						moved.y = -noo.screen_size.h / 2.0f;
					}
					else {
						moved.y = noo.screen_size.h / 2.0f;
					}
				}
				Point<float> offset = noo.map->get_offset();
				Point<float> player_pos = noo.player->get_position() * noo.tile_size + noo.tile_size / 2 - offset;
				Size<int> level_size = noo.map->get_tilemap()->get_size() * noo.tile_size - Size<float>(offset.x, offset.y);
				float min_x = -MAX(0, player_pos.x - noo.screen_size.w / 2.0f);
				float max_x = MAX(0, level_size.w - (player_pos.x + noo.screen_size.w / 2.0f));
				float min_y = -MAX(0, player_pos.y - noo.screen_size.h / 2.0f);
				float max_y = MAX(0, level_size.h - (player_pos.y + noo.screen_size.h / 2.0f));
				if (moved.x < min_x) {
					moved.x = min_x;
				}
				if (moved.x > max_x) {
					moved.x = max_x;
				}
				if (moved.y < min_y) {
					moved.y = min_y;
				}
				if (moved.y > max_y) {
					moved.y = max_y;
				}
				noo.map->set_panning(true);
				noo.map->set_pan(moved);
			}
		}
	}
}

void Player_Brain::reset()
{
	Brain::reset();

	pressed = dragged = false;
}

bool Player_Brain::save(SDL_RWops *file)
{
	SDL_fprintf(file, "brain=player_brain\n");
	return true;
}

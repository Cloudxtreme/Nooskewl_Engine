#include "Nooskewl_Engine/a_star.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/player_brain.h"
#include "Nooskewl_Engine/tilemap.h"

static SDL_Colour white = { 255, 255, 255, 255 };

using namespace Nooskewl_Engine;

Player_Brain::Player_Brain() :
	Light_Brain(Vec3D<float>(0.0f, 0.0f, 1.0f), white, 3, 3)
{
	pressed = false;
	pressed2 = false;
	dragged = false;
	reset();
}

Player_Brain::~Player_Brain()
{
}

void Player_Brain::handle_event(TGUI_Event *event)
{
	bool cancel_astar = false;
	if (event->type == TGUI_KEY_DOWN) {
		if (event->keyboard.code == TGUIK_LEFT) {
			l = true;
			cancel_astar = true;
		}
		else if (event->keyboard.code == TGUIK_RIGHT) {
			r = true;
			cancel_astar = true;
		}
		else if (event->keyboard.code == TGUIK_UP) {
			u = true;
			cancel_astar = true;
		}
		else if (event->keyboard.code == TGUIK_DOWN) {
			d = true;
			cancel_astar = true;
		}
		else if (event->keyboard.code == noo.key_b1 || event->keyboard.code == TGUIK_RETURN) {
			b1 = true;
			cancel_astar = true;
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
			cancel_astar = true;
		}
	}
	else if (event->type == TGUI_JOY_UP) {
		if (event->joystick.button == noo.joy_b1) {
			b1 = false;
		}
	}
	else if (event->type == TGUI_MOUSE_DOWN) {
		if (event->mouse.button == SDL_BUTTON_LEFT) {
			if (noo.player->is_input_enabled()) {
				pressed = true;
				pressed_pos = Point<float>(event->mouse.x, event->mouse.y);
			}
		}
		else if (event->mouse.button == SDL_BUTTON_RIGHT) {
			if (noo.player->is_input_enabled()) {
				pressed2 = true;
				pressed_pos = Point<float>(event->mouse.x, event->mouse.y);
			}
		}
	}
	else if (event->type == TGUI_MOUSE_DOWN && event->mouse.button != SDL_BUTTON_LEFT) {
		cancel_astar = true;
	}
	else if (event->type == TGUI_MOUSE_UP) {
		if (noo.player->is_input_enabled()) {
			if (pressed || pressed2) {
				if (dragged == false) {
					if (pressed && noo.player->is_sitting()) {
						noo.player->set_sitting(false);
					}
					else if (pressed && noo.player->is_sleeping()) {
						noo.player->set_sleeping(false);
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
								bool activated = pressed2 ? true : false;

								if ((abs(dx) <= 2 && dy == 0) || (abs(dy) <= 2 && dx == 0)) {

									bool collides_with_chair_or_bed = false;

									if (pressed) {
										std::vector<Tilemap::Group *> groups = noo.map->get_tilemap()->get_groups(-1);

										for (size_t i = 0; i < groups.size(); i++) {
											Tilemap::Group *g = groups[i];

											int x1_1 = click.x;
											int y1_1 = click.y;
											int x2_1 = click.x + 1;
											int y2_1 = click.y + 1;

											int x1_2 = g->position.x;
											int y1_2 = g->position.y;
											int x2_2 = g->position.x + g->size.w;
											int y2_2 = g->position.y + g->size.h;

											if (x1_1 >= x2_2 || x2_1 <= x1_2 || y1_1 >= y2_2 || y2_1 <= y1_2) {
												continue;
											}

											if (
												(g->type & Tilemap::Group::GROUP_CHAIR_NORTH) ||
												(g->type & Tilemap::Group::GROUP_CHAIR_EAST) ||
												(g->type & Tilemap::Group::GROUP_CHAIR_SOUTH) ||
												(g->type & Tilemap::Group::GROUP_CHAIR_WEST) ||
												(g->type & Tilemap::Group::GROUP_BED_NORTH) ||
												(g->type & Tilemap::Group::GROUP_BED_EAST) ||
												(g->type & Tilemap::Group::GROUP_BED_SOUTH) ||
												(g->type & Tilemap::Group::GROUP_BED_WEST)
											) {
												collides_with_chair_or_bed = true;
												break;
											}
										}
									}

									std::vector<Map_Entity *> colliding_entities = noo.map->get_colliding_entities(-1, click, Size<int>(1, 1));
									if (collides_with_chair_or_bed || colliding_entities.size() > 0) {
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

										if (pressed) {
											activated = noo.map->activate(noo.player);
										}
										else if (pressed2) {
											noo.map->choose_action();
										}
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
			pressed2 = false;
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
				if (fabs(moved.x) > noo.screen_size.w / 2.0f) {
					if (moved.x < 0.0f) {
						moved.x = -noo.screen_size.w / 2.0f;
					}
					else {
						moved.x = noo.screen_size.w / 2.0f;
					}
				}
				if (fabs(moved.y) > noo.screen_size.h / 2.0f) {
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

	if (cancel_astar && map_entity->can_cancel_astar() && map_entity->is_following_path() && map_entity->is_input_enabled()) {
		map_entity->set_stop_next_tile(true);
	}
}

void Player_Brain::reset()
{
	Brain::reset();

	pressed = pressed2 = dragged = false;
}

bool Player_Brain::save(std::string &out)
{
	out += string_printf("brain=player_brain,0\n");
	return true;
}

void Player_Brain::update()
{
	if (noo.player == 0) {
		return;
	}

	// Set light position
	Point<int> pos = noo.player->get_position();
	position.x = pos.x;
	position.y = pos.y;
}

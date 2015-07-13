#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/player_brain.h"

using namespace Nooskewl_Engine;

Player_Brain::Player_Brain()
{
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
		else if (event->keyboard.code == noo.key_b1) {
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
		else if (event->keyboard.code == noo.key_b1) {
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
		Point<int> map_offset = noo.map->get_offset();
		Point<int> click(event->mouse.x, event->mouse.y);
		Size<int> tilemap_size = noo.map->get_tilemap()->get_size() * noo.tile_size;
		click -= map_offset;
		if (click.x >= 0 && click.y >= 0 && click.x < tilemap_size.w && click.y < tilemap_size.h) {
			std::list<A_Star::Node *> path = noo.map->find_path(noo.player->get_position(), click / noo.tile_size);
			if (path.size() > 0) {
				noo.player->set_path(path);
			}
		}
	}
}

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
		switch (event->keyboard.code) {
			case TGUIK_LEFT:
				l = true;
				break;
			case TGUIK_RIGHT:
				r = true;
				break;
			case TGUIK_UP:
				u = true;
				break;
			case TGUIK_DOWN:
				d = true;
				break;
			case TGUIK_RETURN:
			case TGUIK_SPACE:
				b1 = true;
				break;
		}
	}
	else if (event->type == TGUI_KEY_UP) {
		switch (event->keyboard.code) {
			case TGUIK_LEFT:
				l = false;
				break;
			case TGUIK_RIGHT:
				r = false;
				break;
			case TGUIK_UP:
				u = false;
				break;
			case TGUIK_DOWN:
				d = false;
				break;
			case TGUIK_RETURN:
			case TGUIK_SPACE:
				b1 = false;
				break;
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

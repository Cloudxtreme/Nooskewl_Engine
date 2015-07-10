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
				b1 = false;
				break;
		}
	}
}
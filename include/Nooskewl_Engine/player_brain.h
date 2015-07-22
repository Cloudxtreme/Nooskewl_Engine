#ifndef PLAYER_BRAIN_H
#define PLAYER_BRAIN_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/basic_types.h"
#include "Nooskewl_Engine/brain.h"

namespace Nooskewl_Engine {

class Player_Brain : public Brain {
public:
		Player_Brain();
		~Player_Brain();

		void handle_event(TGUI_Event *event);
		void reset();
		bool save(SDL_RWops *file);

private:
	static const int TOLERANCE = 5;

	bool pressed;
	bool dragged;
	Point<float> pressed_pos;
};

} // End namespace Nooskewl_Engine

#endif // PLAYER_BRAIN_H

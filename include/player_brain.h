#ifndef PLAYER_BRAIN_H
#define PLAYER_BRAIN_H

#include "brain.h"

class Player_Brain : public Brain {
public:
		Player_Brain();
		~Player_Brain();

		void handle_event(TGUI_Event *event);
};

#endif PLAYER_BRAIN_H
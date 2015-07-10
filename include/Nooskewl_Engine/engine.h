#ifndef ENGINE_H
#define ENGINE_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Engine {
public:
	Engine(int argc, char **argv);
	~Engine();

	void handle_event(TGUI_Event *event);
	bool update();
	void draw();

private:
	SDL_Joystick *joy;
};

} // End namespace Nooskewl_Engine

#endif // ENGINE_H
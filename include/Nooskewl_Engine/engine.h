#ifndef ENGINE_H
#define ENGINE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/types.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Engine {
public:
	Engine();
	~Engine();

	void start(int argc, char **argv);

	void handle_event(TGUI_Event *event);
	bool update();
	void draw();

	void draw_line(Point<int> a, Point<int> b, SDL_Colour colour);
	void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4]);
	void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour colour);
	void draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle);
	void load_palette(std::string name);

private:
	SDL_Joystick *joy;
	Image *window_image;
	Sprite *speech_arrow;
};

NOOSKEWL_ENGINE_EXPORT extern Engine engine;

} // End namespace Nooskewl_Engine

#endif // ENGINE_H
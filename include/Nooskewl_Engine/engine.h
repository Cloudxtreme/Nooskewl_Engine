#ifndef ENGINE_H
#define ENGINE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/types.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Engine {
public:
	/* Publicly accessible variables */
	// Audio
	bool mute;
	// Graphics
	int screen_w;
	int screen_h;
	bool opengl;
	SDL_Colour colours[256];
	SDL_Colour four_blacks[4];
	SDL_Colour four_whites[4];
	SDL_Colour black;
	SDL_Colour white;
	Font *font;
	Font *bold_font;
	// Other
	CPA *cpa;
	Map *map;
	Map_Entity *player;

	Engine();
	~Engine();

	void start(int argc, char **argv);

	void handle_event(TGUI_Event *event);
	bool update();
	void draw();

	void clear(SDL_Colour colour);
	void clear_depth_buffer(float value);
	void flip();
	void set_default_projection();
	void set_map_transition_projection(float angle);

	void draw_line(Point<int> a, Point<int> b, SDL_Colour colour);
	void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4]);
	void draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour colour);
	void draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle);
	void load_palette(std::string name);

private:
	void init_video();
	void shutdown_video();
	void init_audio();
	void shutdown_audio();
	void load_fonts();

	SDL_Window *window;

	bool vsync;

	GLuint vertexShader;
	GLuint fragmentShader;
	SDL_GLContext opengl_context;

#ifdef _MSC_VER
	HWND hwnd;
	D3DPRESENT_PARAMETERS d3d_pp;
	bool d3d_lost;
	IDirect3D9 *d3d;
#endif

	SDL_Joystick *joy;
	Image *window_image;
	Sprite *speech_arrow;

	SDL_AudioDeviceID audio_device;
};

NOOSKEWL_ENGINE_EXPORT extern Engine noo;

} // End namespace Nooskewl_Engine

#endif // ENGINE_H
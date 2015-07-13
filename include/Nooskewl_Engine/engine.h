#ifndef ENGINE_H
#define ENGINE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/types.h"
#include "Nooskewl_Engine/widgets.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Engine {
public:
	/* Publicly accessible variables */
	// Audio
	bool mute;
	// Graphics
	std::string window_title; // set this first thing to change it
	int scale;
	int screen_w;
	int screen_h;
	int tile_size;
	bool fullscreen;
	bool opengl;
	SDL_Colour colours[256];
	SDL_Colour four_blacks[4];
	SDL_Colour four_whites[4];
	SDL_Colour black;
	SDL_Colour white;
	SDL_Colour magenta;
	Font *font;
	Font *bold_font;
	// Input
	int joy_b1;
	int key_b1;
	// Other
	CPA *cpa;
	Map *map;
	Map_Entity *player;
	TGUI *gui;
	MML *music;

	Engine();
	~Engine();

	void start(int argc, char **argv);
	void stop();

	void handle_event(TGUI_Event *event);
	bool update();
	void draw();

	void clear(SDL_Colour colour);
	void flip();

	void clear_depth_buffer(float value);
	void enable_depth_buffer(bool enable);

	void set_screen_size(int w, int h);
	void set_default_projection();
	void set_map_transition_projection(float angle);

	void draw_line(SDL_Colour colour, Point<int> a, Point<int> b);
	void draw_quad(SDL_Colour vertex_colours[4], Point<int> dest_position, Size<int> dest_size);
	void draw_quad(SDL_Colour colour, Point<int> dest_position, Size<int> dest_size);
	void draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle);
	void load_palette(std::string name);

private:
	void init_video();
	void shutdown_video();
	void init_audio();
	void shutdown_audio();
	void load_fonts();
	void check_joysticks();

	SDL_Window *window;

	bool vsync;

	GLuint vertexShader;
	GLuint fragmentShader;
	SDL_GLContext opengl_context;

#ifdef NOOSKEWL_ENGINE_WINDOWS
	HWND hwnd;
	D3DPRESENT_PARAMETERS d3d_pp;
	bool d3d_lost;
	IDirect3D9 *d3d;
#endif

	SDL_Joystick *joy;
	int num_joysticks;

	Image *window_image;
	Sprite *speech_arrow;

	SDL_AudioDeviceID audio_device;

	MO3_Widget *main_widget;
	MO3_Text_Button *new_game;
};

NOOSKEWL_ENGINE_EXPORT extern Engine noo;

} // End namespace Nooskewl_Engine

#endif // ENGINE_H

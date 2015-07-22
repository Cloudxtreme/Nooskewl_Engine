#ifndef ENGINE_H
#define ENGINE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/types.h"
#include "Nooskewl_Engine/widgets.h"

namespace Nooskewl_Engine {

typedef void (*Callback)(void *data);

class Brain;
class GUI;
class Map;
class Map_Entity;

class NOOSKEWL_ENGINE_EXPORT Engine {
public:
	static const Uint32 TICKS_PER_FRAME = (1000 / 60);

	/* Publicly accessible variables */
	// Audio
	bool mute;
	MML *music;
	MML *button_mml;
	// Graphics
	std::string window_title; // set this first thing to change it
	float scale;
	Size<int> screen_size;
	Point<int> screen_offset;
	int tile_size;
	bool fullscreen;
	bool opengl;
	SDL_Colour colours[256];
	SDL_Colour shadow_colour;
	SDL_Colour four_blacks[4];
	SDL_Colour four_whites[4];
	SDL_Colour black;
	SDL_Colour white;
	SDL_Colour magenta;
	Font *font;
	Font *bold_font;
	Image *window_image;
	Image *name_box_image_top;
	Image *name_box_image_bottom;
	Image *name_box_image_top_right;
	Image *name_box_image_bottom_right;
	Shader *current_shader;
	Shader *default_shader;
	Shader *brighten_shader;
	Shader *shadow_shader;
	Shader *glitch_shader;
#ifdef NOOSKEWL_ENGINE_WINDOWS
	IDirect3DDevice9 *d3d_device;
#endif
	// Input
	int joy_b1;
	int key_b1;
	// Other
	std::string language;
	CPA *cpa;
	Map *map;
	std::string last_map_name;
	Map_Entity *player;
	std::vector<GUI *> guis;

	Engine();
	~Engine();

	void start(int argc, char **argv);
	void end();

	bool handle_event(SDL_Event *sdl_event);
	bool update();
	void draw();

	bool check_milestone(int number);
	void set_milestone(int number, bool completed);
	int milestone_name_to_number(std::string name);
	std::string milestone_number_to_name(int number);
	int get_num_milestones();
	void clear_milestones();

	void clear(SDL_Colour colour);
	void flip();

	void clear_depth_buffer(float value);
	void enable_depth_buffer(bool enable);

	void set_screen_size(int w, int h);
	void set_default_projection();
	void set_map_transition_projection(float angle);
	void update_projection();

	void draw_line(SDL_Colour colour, Point<float> a, Point<float> b, float thickness = 1.0f);
	void draw_rectangle(SDL_Colour colour, Point<float> pos, Size<float> size, float thickness = 1.0f);
	void draw_quad(SDL_Colour vertex_colours[4], Point<float> dest_position, Size<float> dest_size);
	void draw_quad(SDL_Colour colour, Point<float> dest_position, Size<float> dest_size);
	void draw_9patch_tinted(SDL_Colour tint, Image *image, Point<int> dest_position, Size<int> dest_size);
	void draw_9patch(Image *image, Point<int> dest_position, Size<int> dest_size);

	void load_palette(std::string name);
	std::string load_text(std::string filename);
	void play_music(std::string name);

private:
	void init_video();
	void shutdown_video();
	void init_audio();
	void shutdown_audio();
	void load_fonts();
	void check_joysticks();
	void set_mouse_cursor();
	void set_initial_d3d_state();
	void maybe_expand_milestones(int number);
	void load_milestones();

	SDL_Window *window;
	bool vsync;
	Uint32 last_frame;
	int accumulated_delay;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;

	SDL_GLContext opengl_context;

	SDL_Joystick *joy;
	int num_joysticks;

	SDL_AudioDeviceID audio_device;

#ifdef NOOSKEWL_ENGINE_WINDOWS
	HWND hwnd;
	D3DPRESENT_PARAMETERS d3d_pp;
	bool d3d_lost;
	IDirect3D9 *d3d;
	HICON mouse_cursor;
#endif

	std::string default_opengl_vertex_source;
	std::string default_opengl_fragment_source;
	std::string brighten_opengl_fragment_source;
	std::string default_d3d_vertex_source;
	std::string default_d3d_fragment_source;
	std::string brighten_d3d_fragment_source;
	std::string d3d_technique_source;

	bool *milestones;
	int num_milestones;
	std::map<int, std::string> ms_number_to_name;
	std::map<std::string, int> ms_name_to_number;
};

NOOSKEWL_ENGINE_EXPORT extern Engine noo;

} // End namespace Nooskewl_Engine

#endif // ENGINE_H

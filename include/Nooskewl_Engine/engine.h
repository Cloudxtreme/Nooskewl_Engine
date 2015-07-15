#ifndef ENGINE_H
#define ENGINE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/types.h"
#include "Nooskewl_Engine/widgets.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Engine {
public:
	struct Shader {
		GLuint opengl_vertex_shader;
		GLuint opengl_fragment_shader;
		GLuint opengl_shader;
#ifdef NOOSKEWL_ENGINE_WINDOWS
		LPD3DXEFFECT d3d_effect;
		D3DXHANDLE d3d_technique;
#endif
	};

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
	SDL_Colour shadow_colour;
	SDL_Colour four_blacks[4];
	SDL_Colour four_whites[4];
	SDL_Colour black;
	SDL_Colour white;
	SDL_Colour magenta;
	Font *font;
	Font *bold_font;
	Image *window_image;
	Image *window_image_with_name;
	Image *name_box_image;
	Shader current_shader;
	Shader default_shader;
	Shader brighten_shader;
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
	Map_Entity *player;
	TGUI *gui;
	MML *music;

	Engine();
	~Engine();

	void start(int argc, char **argv);
	void end();

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

	void draw_9patch_tinted(SDL_Colour tint, Image *image, Point<int> dest_position, Size<int> dest_size);
	void draw_9patch(Image *image, Point<int> dest_position, Size<int> dest_size);
	void load_palette(std::string name);
	Shader create_shader(std::string opengl_vertex_source, std::string opengl_fragment_source, std::string d3d_vertex_source, std::string d3d_fragment_source);
	void use_shader(Shader shader);
	void destroy_shader(Shader shader);
	void set_shader_float(Shader shader, std::string name, float value);

private:
	void init_video();
	void shutdown_video();
	void init_audio();
	void shutdown_audio();
	void load_fonts();
	void check_joysticks();
	void set_mouse_cursor();
	void update_projection();

	SDL_Window *window;

	bool vsync;

	std::string default_opengl_vertex_source;
	std::string default_opengl_fragment_source;
	std::string brighten_opengl_fragment_source;
	std::string default_d3d_vertex_source;
	std::string default_d3d_fragment_source;
	std::string brighten_d3d_fragment_source;
	std::string d3d_technique_source;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;

	GLuint default_opengl_vertex_shader;
	GLuint default_opengl_fragment_shader;
	SDL_GLContext opengl_context;

	SDL_Joystick *joy;
	int num_joysticks;

	SDL_AudioDeviceID audio_device;

	MO3_Widget *main_widget;
	MO3_Text_Button *new_game;

	Image *logo;

#ifdef NOOSKEWL_ENGINE_WINDOWS
	HWND hwnd;
	D3DPRESENT_PARAMETERS d3d_pp;
	bool d3d_lost;
	IDirect3D9 *d3d;
	HICON mouse_cursor;
#endif

public:
	/* Template functions */

	template<typename T> void draw_line(SDL_Colour colour, Point<T> a, Point<T> b, float thickness = 1.0f)
	{
		float half_thickness = thickness / 2.0f;
		SDL_Colour vertex_colours[4];
		for (int i = 0; i < 4; i++) {
			vertex_colours[i] = colour;
		}
		float dx = float(a.x - b.x);
		float dy = float(a.y - b.y);
		float angle = atan2f(dy, dx);
		/* Make 4 points for thickness */
		float a1 = angle + M_PI / 2.0f;
		float a2 = angle - M_PI / 2.0f;
		Point<float> da = a;
		Point<float> db = a;
		Point<float> dc = b;
		Point<float> dd = b;
		da.x += cos(a1) * half_thickness;
		da.y += sin(a1) * half_thickness;
		db.x += cos(a2) * half_thickness;
		db.y += sin(a2) * half_thickness;
		dc.x += cos(a1) * half_thickness;
		dc.y += sin(a1) * half_thickness;
		dd.x += cos(a2) * half_thickness;
		dd.y += sin(a2) * half_thickness;
		if (opengl) {
			glDisable(GL_TEXTURE_2D);
			printGLerror("glBindTexture");
		}
		m.vertex_cache->start();
		m.vertex_cache->buffer<float>(vertex_colours, Point<float>(0, 0), Size<float>(0, 0), da, dc, dd, db, 0);
		m.vertex_cache->end();
		if (opengl) {
			glEnable(GL_TEXTURE_2D);
			printGLerror("glBindTexture");
		}
	}

	template<typename T> void draw_rectangle(SDL_Colour colour, Point<T> pos, Size<T> size, float thickness = 1.0f)
	{
		float half_thickness = thickness / 2.0f;
		Point<float> fpos = pos;
		Size<float> fsize = size;
		draw_line<float>(colour, Point<float>(fpos.x, fpos.y+half_thickness), Point<float>(fpos.x+fsize.w, fpos.y+half_thickness), thickness); // top
		draw_line<float>(colour, Point<float>(fpos.x, fpos.y+size.h-half_thickness), Point<float>(fpos.x+fsize.w, fpos.y+size.h-half_thickness), thickness); // bottom
		// left and right are a pixel short so there's no overlap
		draw_line<float>(colour, Point<float>(fpos.x+half_thickness, fpos.y+thickness), Point<float>(fpos.x+half_thickness, fpos.y+fsize.h-thickness), thickness); // left
		draw_line<float>(colour, Point<float>(fpos.x+size.w-half_thickness, fpos.y+thickness), Point<float>(fpos.x+size.w-half_thickness, fpos.y+fsize.h-thickness), thickness); // right
	}

	template<typename T> void draw_quad(SDL_Colour vertex_colours[4], Point<T> dest_position, Size<T> dest_size)
	{
		if (opengl) {
			glDisable(GL_TEXTURE_2D);
			printGLerror("glBindTexture");
		}
		m.vertex_cache->start();
		m.vertex_cache->buffer<T>(vertex_colours, Point<T>(0, 0), Size<T>(0, 0), dest_position, dest_size, 0);
		m.vertex_cache->end();
		if (opengl) {
			glEnable(GL_TEXTURE_2D);
			printGLerror("glBindTexture");
		}
	}

	template<typename T> void draw_quad(SDL_Colour colour, Point<T> dest_position, Size<T> dest_size)
	{
		static SDL_Colour vertex_colours[4];
		for (int i = 0; i < 4; i++) {
			vertex_colours[i] = colour;
		}
		draw_quad<T>(vertex_colours, dest_position, dest_size);
	}
};

NOOSKEWL_ENGINE_EXPORT extern Engine noo;

} // End namespace Nooskewl_Engine

#endif // ENGINE_H

#include "Nooskewl_Engine/brain.h"
#include "Nooskewl_Engine/cpa.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/gui.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/sample.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/speech.h"
#include "Nooskewl_Engine/tokenizer.h"
#include "Nooskewl_Engine/translation.h"
#include "Nooskewl_Engine/vertex_cache.h"
#include "Nooskewl_Engine/widgets.h"

#include "Nooskewl_Engine/engine_translation_English.h"

#ifdef NOOSKEWL_ENGINE_WINDOWS
#define NOOSKEWL_ENGINE_FVF (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE4(1))
#endif

#define perfect_w 176
#define perfect_h 99

using namespace Nooskewl_Engine;

static void audio_callback(void *userdata, Uint8 *stream, int stream_length)
{
	memset(stream, m.device_spec.silence, stream_length);

	SDL_LockMutex(m.mixer_mutex);

	std::vector<SampleInstance *>::iterator it;
	for (it = m.playing_samples.begin(); it != m.playing_samples.end();) {
		SampleInstance *s = *it;
		int length;
		if (s->loop) {
			length = stream_length;
		}
		else {
			length = s->length - s->offset;
			if (length > stream_length) {
				length = stream_length;
			}
			s->offset += length;
		}
		SDL_MixAudioFormat(stream, s->data+s->offset, m.device_spec.format, length, (int)(s->volume * 128.0f));
		if (s->loop == false && s->offset >= s->length) {
			it = m.playing_samples.erase(it);
		}
		else {
			it++;
		}
	}
	MML::mix(stream, stream_length);

	SDL_UnlockMutex(m.mixer_mutex);
}

namespace Nooskewl_Engine {

Engine noo;

Engine::Engine() :
	music(0),
	window_title("Nooskewl Engine"),
	joy_b1(10),
	key_b1(TGUIK_SPACE),
	map(0),
	last_map_name(""),
	tile_size(8),
	joy(0),
	num_joysticks(0),
	language("English"),
	milestones(0),
	num_milestones(0),
	depth_buffer_enabled(false),
	doing_map_transition(false)
{
}

Engine::~Engine()
{
}

bool Engine::start(int argc, char **argv)
{
	srand((unsigned int)time(0));

	mute = check_args(argc, argv, "+mute") > 0;
	fullscreen = check_args(argc, argv, "+fullscreen") > 0;
	vsync = !(check_args(argc, argv, "-vsync") > 0);
#ifdef NOOSKEWL_ENGINE_WINDOWS
	opengl = ((check_args(argc, argv, "-d3d") > 0) || (check_args(argc, argv, "+opengl")) > 0);
#else
	opengl = true;
#endif

	load_dll();

	int flags = SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_VIDEO;
	if (mute == false) {
		flags |= SDL_INIT_AUDIO;
	}

	if (SDL_Init(flags) != 0) {
		throw Error("SDL_Init failed");
	}

	init_audio();

	int play_mml;
	if ((play_mml = check_args(argc, argv, "+play-mml")) > 0) {
		MML *mml = new MML(argv[play_mml+1], true);
		mml->play(false);
		while (mml->is_done()) {
			SDL_Delay(1);
		}
		exit(0);
	}

	cpa = new CPA();

	set_mouse_cursor();

	init_video();

	if (TTF_Init() == -1) {
		throw Error("TTF_Init failed");
	}

	load_fonts();

	load_palette("palette.gpl");

	int dump_colours = check_args(argc, argv, "+dump-colours");
	int repalette_images = check_args(argc, argv, "+repalette-images");

	if (dump_colours > 0 || repalette_images > 0) {
		if (repalette_images > 0) {
			Image::keep_data = true;
		}
		if (dump_colours > 0) {
			Image::dumping_colours = true;
		}
		std::vector<std::string> v = cpa->get_all_filenames();
		for (size_t i = 0; i < v.size(); i++) {
			std::string &s = v[i];
			if (s.substr(s.length()-4) == ".tga") {
				Image *image = new Image(s, true);
				if (dump_colours > 0) {
					delete image;
				}
				else {
					std::string output_path = argv[repalette_images + 1];
					_mkdir(output_path.c_str());
					std::string filename = image->filename;
					std::vector<std::string> path_components;
					Tokenizer t(filename, '/');
					std::string tok;
					while ((tok = t.next()) != "") {
						path_components.push_back(tok);
					}
					std::string dir_name = output_path;
					for (size_t j = 0; j < path_components.size()-1; j++) {
						dir_name += "/" + path_components[j];
						_mkdir(dir_name.c_str());
					}
					image->save(dir_name + "/" + path_components[path_components.size()-1]);
					delete image;
				}
			}
		}
		exit(0);
	}

	window_image = new Image("window.tga");
	name_box_image_top = new Image("name_box_top.tga");
	name_box_image_bottom = new Image("name_box_bottom.tga");
	name_box_image_top_right = new Image("name_box_top_right.tga");
	name_box_image_bottom_right = new Image("name_box_bottom_right.tga");

	std::string engine_translation;
	std::string game_translation;

	if (language == "English") {
		engine_translation = engine_translation_English;
		game_translation = load_text("text/English.utf8");
	}

	t = new Translation(engine_translation);
	game_t = new Translation(game_translation);

	load_milestones();

	Widget::static_start();
	Speech::static_start();

	TGUI::set_focus_sloppiness(0);
	guis.push_back(new Title_GUI());

	button_mml = new MML("button.mml");

	if (m.dll_start() == false) {
		return false;
	}

	Uint32 last_frame = SDL_GetTicks();
	accumulated_delay = 0;

	return true;
}

void Engine::end()
{
	SDL_LockMutex(m.mixer_mutex);
	delete music;
	SDL_UnlockMutex(m.mixer_mutex);

	delete button_mml;

	if (map) {
		map->end();
	}
	delete map;
	delete player;

	Widget::static_end();
	Speech::static_end();

	for (size_t i = 0; i < guis.size(); i++) {
		delete guis[i];
	}
	guis.clear();

	delete window_image;
	delete name_box_image_top;
	delete name_box_image_bottom;
	delete name_box_image_top_right;
	delete name_box_image_bottom_right;

	delete font;
	delete bold_font;
	TTF_Quit();

	shutdown_video();
	shutdown_audio();

#ifdef NOOSKEWL_ENGINE_WINDOWS
	DestroyIcon(mouse_cursor);
#endif

	delete cpa;

	infomsg("%d unfreed images\n", Image::get_unfreed_count());

	delete t;
	delete game_t;

	m.dll_end();
	close_dll();

	if (joy && SDL_JoystickGetAttached(joy)) {
		SDL_JoystickClose(joy);
	}

	SDL_Quit();
}

void Engine::init_video()
{
	if (opengl) {
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	}

	int flags = SDL_WINDOW_RESIZABLE;
	if (opengl) {
		flags |= SDL_WINDOW_OPENGL;
	}

	int max_w = 1280;
	int max_h = 720;

	SDL_DisplayMode mode;
	int win_w = max_w;
	int win_h = max_h;

	if (fullscreen) {
		if (SDL_GetCurrentDisplayMode(0, &mode) == 0) {
			win_w = mode.w;
			win_h = mode.h;
		}
		else {
			fullscreen = false;
		}
	}

	if (fullscreen == false) {
		// Find close 16:9 window size
		for (int i = 0; i < 1/*SDL_GetNumVideoDisplays()*/; i++) {
			if (SDL_GetCurrentDisplayMode(i, &mode) == 0) {
				// Give room for toolbars and decorations
				mode.w -= 256;
				mode.h -= 256;
				float desired_w = 16.0f;
				float desired_h = 9.0f;
				float w = (float)mode.w * desired_h;
				float h = (float)mode.h * desired_w;
				if (w > h) {
					win_w = mode.w;
					win_h = mode.w * 9 / (int)desired_w;
				}
				else {
					win_w = mode.h * 16 / (int)desired_h;
					win_h = mode.h;
				}
			}
		}
		// FIXME: if the user picks a big screen, allow it
		if (win_w > max_w && win_h > max_h) {
			// Huge windows are annoying
			win_w = max_w;
			win_h = max_h;
		}
	}

	if (win_w < perfect_w || win_h < perfect_h) {
		win_w = perfect_w;
		win_h = perfect_h;
	}

	window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_w, win_h, flags);
	if (window == 0) {
		throw Error("SDL_CreateWindow failed");
	}

	SDL_SetWindowMinimumSize(window, perfect_w, perfect_h);

	if (fullscreen) {
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	if (opengl) {
		opengl_context = SDL_GL_CreateContext(window);
		SDL_GL_SetSwapInterval(vsync ? 1 : 0); // vsync, 1 = on

		glewExperimental = 1;
		GLenum error = glewInit();
		if (error != GL_NO_ERROR) {
			throw Error("glewInit failed");
		}

		glEnable(GL_BLEND);
		printGLerror("glEnable");
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		printGLerror("glBlendFunc");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		hwnd = GetActiveWindow();

		if ((d3d = Direct3DCreate9(D3D_SDK_VERSION)) == 0) {
			throw Error("Direct3D9Create failed");
		}

		ZeroMemory(&d3d_pp, sizeof(d3d_pp));

		d3d_pp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3d_pp.BackBufferWidth = w;
		d3d_pp.BackBufferHeight = h;
		d3d_pp.BackBufferCount = 1;
		d3d_pp.Windowed = 1;
		if (vsync) {
			d3d_pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		else {
			d3d_pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}
		d3d_pp.EnableAutoDepthStencil = true;
		d3d_pp.AutoDepthStencilFormat = D3DFMT_D16;
		//d3d_pp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		d3d_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3d_pp.hDeviceWindow = hwnd;

		HRESULT hr;
		if ((hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED, &d3d_pp, (LPDIRECT3DDEVICE9 *)&d3d_device)) != D3D_OK) {
			if ((hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED, &d3d_pp, (LPDIRECT3DDEVICE9 *)&d3d_device)) != D3D_OK) {
				throw Error("Unable to create D3D device");
			}
		}

		set_initial_d3d_state();
	}
#endif

	std::string default_vertex_source;
	std::string default_fragment_source;
	std::string brighten_fragment_source;
	std::string shadow_fragment_source;
	std::string glitch_fragment_source;

	std::string tag = opengl ? "glsl" : "hlsl";

	default_vertex_source = load_text("shaders/" + tag + "/default_vertex.txt");
	default_fragment_source = load_text("shaders/" + tag + "/default_fragment.txt");
	brighten_fragment_source = load_text("shaders/" + tag + "/brighten_fragment.txt");
	shadow_fragment_source = load_text("shaders/" + tag + "/shadow_fragment.txt");
	glitch_fragment_source = load_text("shaders/" + tag + "/glitch_fragment.txt");

	default_shader = new Shader(opengl, default_vertex_source, default_fragment_source);
	brighten_shader = new Shader(opengl, default_vertex_source, brighten_fragment_source);
	shadow_shader = new Shader(opengl, default_vertex_source, shadow_fragment_source);
	glitch_shader = new Shader(opengl, default_vertex_source, glitch_fragment_source);

	current_shader = default_shader;
	current_shader->use();

	set_screen_size(w, h);
	set_default_projection();

	m.vertex_cache = new Vertex_Cache();
	m.vertex_cache->init();
}

void Engine::shutdown_video()
{
	delete m.vertex_cache;

	delete default_shader;
	delete brighten_shader;
	delete shadow_shader;
	delete glitch_shader;

	if (opengl) {
		SDL_GL_DeleteContext(opengl_context);
	}
	else {
		d3d_device->Release();
	}

	SDL_DestroyWindow(window);
}

void Engine::init_audio()
{
	if (mute) {
		return;
	}

	m.mixer_mutex = SDL_CreateMutex();

	SDL_AudioSpec desired;
	desired.freq = 44100;
	desired.format = AUDIO_S16;
	desired.channels = 1;
	desired.samples = 4096;
	desired.callback = audio_callback;
	desired.userdata = 0;

	audio_device = SDL_OpenAudioDevice(0, false, &desired, &m.device_spec, 0);

	if (audio_device == 0) {
		throw Error("init_audio failed");
	}

	SDL_PauseAudioDevice(audio_device, false);
}

void Engine::shutdown_audio()
{
	if (audio_device != 0) {
		SDL_CloseAudioDevice(audio_device);
	}

	SDL_DestroyMutex(m.mixer_mutex);
}

bool Engine::handle_event(SDL_Event *sdl_event)
{
	if (sdl_event->type == SDL_QUIT) {
		return false;

	}
	else if (sdl_event->type == SDL_WINDOWEVENT && sdl_event->window.event == SDL_WINDOWEVENT_RESIZED) {
#ifdef NOOSKEWL_ENGINE_WINDOWS
		if (opengl == false) {
			font->clear_cache();
			bold_font->clear_cache();
			Image::release_all();
			Shader::release_all();
			d3d_pp.BackBufferFormat = D3DFMT_X8R8G8B8;
			d3d_pp.BackBufferWidth = sdl_event->window.data1;
			d3d_pp.BackBufferHeight = sdl_event->window.data2;
			d3d_pp.BackBufferCount = 1;
			d3d_device->Reset(&d3d_pp);
			set_initial_d3d_state();
			load_fonts();
			Image::reload_all();
			Shader::reload_all();
			set_screen_size(sdl_event->window.data1, sdl_event->window.data2);
			set_default_projection(); // FIXME: change this to current projection (update the matrices then call update_projection())
		}
#endif
		noo.set_screen_size(sdl_event->window.data1, sdl_event->window.data2);
		set_default_projection();
		return true;
	}

	TGUI_Event event = tgui_sdl_convert_event(sdl_event);

	if (event.type == TGUI_MOUSE_DOWN || event.type == TGUI_MOUSE_UP || event.type == TGUI_MOUSE_AXIS) {
		event.mouse.x = (event.mouse.x - screen_offset.x) / scale;
		event.mouse.y = (event.mouse.y - screen_offset.y) / scale;
		// Due to scaling and offset, mouse events can come in outside of the playable area, skip those
		if (event.mouse.x < 0 || event.mouse.x >= screen_size.w || event.mouse.y < 0 || event.mouse.y >= screen_size.h) {
			return true;
		}
	}

#ifdef NOOSKEWL_ENGINE_WINDOWS
	if (event.type == TGUI_MOUSE_AXIS || event.type == TGUI_MOUSE_DOWN || event.type == TGUI_MOUSE_UP) {
		SetCursor(mouse_cursor);
	}
#endif

	if (guis.size() > 0) {
		GUI *noo_gui = guis[guis.size()-1];
		if (noo_gui->gui) {
			noo_gui->gui->handle_event(&event);
		}
	}
	else if (doing_map_transition == false && map) {
		map->handle_event(&event);

		if (event.type == TGUI_KEY_DOWN && event.keyboard.code == TGUIK_ESCAPE) {
				guis.push_back(new Pause_GUI());
		}
	}

	return true;
}

bool Engine::update()
{
	check_joysticks();

	if (guis.size() > 0) {
		std::vector<GUI *> other_guis;
		other_guis.insert(other_guis.begin(), guis.begin(), guis.end()-1);
		GUI *noo_gui = guis[guis.size()-1];
		if (noo_gui->gui && noo_gui->gui->get_focus() == 0) {
			noo_gui->gui->set_focus(noo_gui->focus);
		}
		if (noo_gui->update() == false) {
			// update may have push other GUIs on the stack, so we can't just erase the last one
			for (size_t i = 0; i < guis.size(); i++) {
				if (guis[i] == noo_gui) {
					guis.erase(guis.begin() + i);
					break;
				}
			}
			delete noo_gui;
		}
		for (size_t i = 0; i < other_guis.size(); i++) {
			noo_gui = other_guis[i];
			if (noo_gui->gui && noo_gui->gui->get_focus() != 0) {
				noo_gui->focus = noo_gui->gui->get_focus();
				noo_gui->gui->set_focus(0);
			}
			if (noo_gui->update_background() == false) {
				// update may have push other GUIs on the stack, so we can't just erase the last one
				for (size_t j = 0; j < guis.size(); j++) {
		 			if (guis[j] == noo_gui) {
						guis.erase(guis.begin() + j);
						break;
					}
				}
				delete noo_gui;
			}
		}
	}

	if (doing_map_transition == false) {
		if (map && map->update() == false) {
			map->get_new_map_details(new_map_name, new_map_position, new_map_direction);
			if (new_map_name != "") {
				old_map = map;
				last_map_name = old_map->get_map_name();
				map = new Map(new_map_name);
				player->get_brain()->reset();
				map->add_entity(player);
				map->start();
				map->update_camera();

				doing_map_transition = true;
				map_transition_start = SDL_GetTicks();
				moved_player_during_map_transition = false;
			}
		}
	}

	if (doing_map_transition) {
		Uint32 end_time = map_transition_start + map_transition_duration;

		if (SDL_GetTicks() >= end_time) {
			doing_map_transition = false;

			old_map->end();
			delete old_map;
		}
		else {
			Uint32 elapsed = SDL_GetTicks() - map_transition_start;
			if (moved_player_during_map_transition == false && elapsed >= map_transition_duration/2) {
				// The actual moving happens below in this same loop
				moved_player_during_map_transition = true;
				player->set_position(new_map_position);
				player->set_direction(new_map_direction);
			}

		}
	}

	return true;
}

void Engine::draw()
{
	clear_buffers();

	if (doing_map_transition) {
		Uint32 elapsed = MIN(map_transition_duration-1, SDL_GetTicks() - map_transition_start);

		set_map_transition_projection((float)elapsed / map_transition_duration * (float)M_PI);

		m.vertex_cache->enable_perspective_drawing(screen_size);

		if (moved_player_during_map_transition) {
			map->update_camera();
			map->draw();
		}
		else {
			old_map->update_camera();
			old_map->draw();
		}

		m.vertex_cache->disable_perspective_drawing();

		set_default_projection();
	}
	else {
		if (map) {
			map->draw();
		}

		for (size_t i = 0; i < guis.size(); i++) {
			guis[i]->draw_back();
			if (guis[i]->gui) {
				guis[i]->gui->draw();
			}
			guis[i]->draw_fore();
		}
	}

	flip();

	// TIMING
	// This code is ugly for a reason
	Uint32 now = SDL_GetTicks();
	int elapsed = now - last_frame;
	if (elapsed < TICKS_PER_FRAME) {
		int wanted_delay = TICKS_PER_FRAME - elapsed;
		int final_delay = wanted_delay + accumulated_delay;
		if (final_delay > 0) {
			SDL_Delay(final_delay);
			elapsed = SDL_GetTicks() - now;
			accumulated_delay -= elapsed - wanted_delay;
		}
		else {
			accumulated_delay += elapsed;
		}
		if (accumulated_delay > 100 || accumulated_delay < -100) {
			accumulated_delay = 0;
		}
	}
	last_frame = SDL_GetTicks();
}

bool Engine::check_milestone(int number)
{
	maybe_expand_milestones(number);

	return milestones[number];
}

bool Engine::check_milestone(std::string name)
{
	return check_milestone(milestone_name_to_number(name));
}

void Engine::set_milestone(int number, bool completed)
{
	maybe_expand_milestones(number);

	milestones[number] = completed;
}

int Engine::milestone_name_to_number(std::string name)
{
	std::map<std::string, int>::iterator it;
	if ((it = ms_name_to_number.find(name)) == ms_name_to_number.end()) {
		return -1;
	}
	return (*it).second;
}

std::string Engine::milestone_number_to_name(int number)
{
	std::map<int, std::string>::iterator it;
	if ((it = ms_number_to_name.find(number)) == ms_number_to_name.end()) {
		return "";
	}
	return (*it).second;
}

int Engine::get_num_milestones()
{
	return num_milestones;
}

void Engine::clear_milestones()
{
	free(milestones);
	milestones = 0;
	num_milestones = 0;
}

void Engine::clear(SDL_Colour colour)
{
	if (opengl) {
		glClearColor(colour.r/255.0f, colour.g/255.0f, colour.b/255.0f, colour.a/255.0f);
		printGLerror("glClearColor");
		glClear(GL_COLOR_BUFFER_BIT);
		printGLerror("glClear");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		d3d_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_RGBA(colour.r, colour.g, colour.b, colour.a), 0.0f, 0);
	}
#endif
}

void Engine::clear_depth_buffer(float value)
{
	if (opengl) {
		glClearDepth(value);
		printGLerror("glClearDepth");
		glClear(GL_DEPTH_BUFFER_BIT);
		printGLerror("glClear");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		d3d_device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, value, 0);
	}
#endif
}

void Engine::enable_depth_buffer(bool enable)
{
	if (depth_buffer_enabled == enable) {
		return;
	}

	depth_buffer_enabled = enable;

	if (opengl) {
		if (enable) {
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.1f);
		}
		else {
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_ALPHA_TEST);
		}
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		if (enable) {
			d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			d3d_device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
			d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			d3d_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			d3d_device->SetRenderState(D3DRS_ALPHAREF, 1);
		}
		else {
			d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		}
	}
#endif
}

bool Engine::is_depth_buffer_enabled()
{
	return depth_buffer_enabled;
}

void Engine::flip()
{
	if (opengl) {
		SDL_GL_SwapWindow(window);
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		d3d_device->EndScene();

		if (d3d_lost) {
			HRESULT hr = d3d_device->TestCooperativeLevel();
			if (hr == D3DERR_DEVICENOTRESET) {
				hr = d3d_device->Reset(&d3d_pp);
				if (hr != D3D_OK) {
					infomsg("Device couldn't be reset!\n");
				}
				else {
					d3d_lost = false;

					set_initial_d3d_state();

					// Everything's gone!
					font->clear_cache();
					bold_font->clear_cache();
					Image::release_all();
					Shader::release_all();

					// So reload it!
					Shader::reload_all();
					load_fonts();
					Image::reload_all();

					int w, h;
					SDL_GetWindowSize(window, &w, &h);
					set_screen_size(w, h);
					set_default_projection(); // FIXME: change this to current projection (update the matrices then call update_projection())
				}
			}
		}
		else {
			HRESULT hr = d3d_device->Present(0, 0, hwnd, 0);

			if (hr == D3DERR_DEVICELOST) {
				infomsg("D3D device lost\n");
				d3d_lost = true;
			}
		}

		d3d_device->BeginScene();
	}
#endif
}

void Engine::set_screen_size(int w, int h)
{
	float aspect = (float)w / h;
	float perfect_aspect = (float)perfect_w / perfect_h;
	if (w*perfect_h >= h*perfect_w) {
		scale = (float)h / perfect_h;
	}
	else {
		scale = (float)w / perfect_w;
	}

	// Don't scale too much away from max dimension (no cheating!)
	if (fabs(aspect-perfect_aspect) > 0.5f) {
		if (w*perfect_h >= h*perfect_w) {
			screen_size.h = int(h / scale);
			screen_size.w = screen_size.h * perfect_w / perfect_h;
		}
		else {
			screen_size.w = int(w / scale);
			screen_size.h = screen_size.w * perfect_h / perfect_w;
		}
	}
	else {
		screen_size.w = int(w / scale);
		screen_size.h = int(h / scale);
	}

	screen_offset = Point<int>(int(w-(screen_size.w*scale))/2, int(h-(screen_size.h*scale))/2);
	if (screen_offset.x < scale) {
		screen_offset.x = 0;
	}
	if (screen_offset.y < scale) {
		screen_offset.y = 0;
	}

	if (opengl) {
		glViewport(0, 0, w, h);
		printGLerror("glViewport");
		glEnable(GL_SCISSOR_TEST);
		printGLerror("glEnable(GL_SCISSOR_TEST)");
		glScissor(screen_offset.x, screen_offset.y, MIN(w, int(screen_size.w*scale)+1), MIN(h, int(screen_size.h*scale)+1));
		printGLerror("glScissor");
	}
	else {
		D3DVIEWPORT9 viewport = { 0, 0, w, h, 0.0f, 1.0f };
		d3d_device->SetViewport(&viewport);
		d3d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		RECT scissor = { screen_offset.x, screen_offset.y, screen_offset.x+MIN(w, int(screen_size.w*scale)+1), screen_offset.y+MIN(h, int(screen_size.h*scale)+1) };
		d3d_device->SetScissorRect(&scissor);
	}

	for (size_t i = 0; i < guis.size(); i++) {
		if (guis[i]->gui) {
			guis[i]->gui->resize(screen_size.w, screen_size.h);
		}
	}
}

void Engine::set_default_projection()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	model = glm::mat4();
	view = glm::translate(glm::mat4(), glm::vec3(screen_offset.x, screen_offset.y, 0));
	proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f);

	update_projection();
}

void Engine::set_map_transition_projection(float angle)
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	model = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3((angle >= M_PI/2.0f ? -1.0f : 1.0f) * ((screen_size.w*scale)/w), 1.0f * ((screen_size.h*scale)/h), (angle >= M_PI/2.0f) ? -1.0f : 1.0f));
	view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
	proj = glm::frustum(-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1000.0f);

	update_projection();
}

void Engine::draw_line(SDL_Colour colour, Point<float> a, Point<float> b, float thickness)
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
	float a1 = angle + (float)M_PI / 2.0f;
	float a2 = angle - (float)M_PI / 2.0f;
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
	m.vertex_cache->cache(vertex_colours, Point<float>(0, 0), Size<float>(0, 0), da, dc, dd, db, 0);
	m.vertex_cache->end();
	if (opengl) {
		glEnable(GL_TEXTURE_2D);
		printGLerror("glBindTexture");
	}
}

void Engine::draw_rectangle(SDL_Colour colour, Point<float> pos, Size<float> size, float thickness)
{
	float half_thickness = thickness / 2.0f;
	Point<float> fpos = pos;
	Size<float> fsize = size;
	draw_line(colour, Point<float>(fpos.x, fpos.y+half_thickness), Point<float>(fpos.x+fsize.w, fpos.y+half_thickness), thickness); // top
	draw_line(colour, Point<float>(fpos.x, fpos.y+size.h-half_thickness), Point<float>(fpos.x+fsize.w, fpos.y+size.h-half_thickness), thickness); // bottom
	// left and right are a pixel short so there's no overlap
	draw_line(colour, Point<float>(fpos.x+half_thickness, fpos.y+thickness), Point<float>(fpos.x+half_thickness, fpos.y+fsize.h-thickness), thickness); // left
	draw_line(colour, Point<float>(fpos.x+size.w-half_thickness, fpos.y+thickness), Point<float>(fpos.x+size.w-half_thickness, fpos.y+fsize.h-thickness), thickness); // right
}

void Engine::draw_quad(SDL_Colour vertex_colours[4], Point<float> dest_position, Size<float> dest_size)
{
	if (opengl) {
		glDisable(GL_TEXTURE_2D);
		printGLerror("glBindTexture");
	}
	m.vertex_cache->start();
	m.vertex_cache->cache(vertex_colours, Point<float>(0, 0), Size<float>(0, 0), dest_position, dest_size, 0);
	m.vertex_cache->end();
	if (opengl) {
		glEnable(GL_TEXTURE_2D);
		printGLerror("glBindTexture");
	}
}

void Engine::draw_quad(SDL_Colour colour, Point<float> dest_position, Size<float> dest_size)
{
	static SDL_Colour vertex_colours[4];
	for (int i = 0; i < 4; i++) {
		vertex_colours[i] = colour;
	}
	draw_quad(vertex_colours, dest_position, dest_size);
}

void Engine::draw_9patch_tinted(SDL_Colour tint, Image *image, Point<int> dest_position, Size<int> dest_size)
{
	int w = image->size.w;
	int size = image->size.w / 3;
	Size<int> dim(size, size);

	image->start();

	// Corners
	image->draw_region_tinted(tint, Point<int>(0, 0), dim, dest_position); // top left
	image->draw_region_tinted(tint, Point<int>(w-size, 0), dim, Point<int>(dest_position.x+dest_size.w-size, dest_position.y)); // top right
	image->draw_region_tinted(tint, Point<int>(w-size, w-size), dim, dest_position+dest_size-dim); // bottom right
	image->draw_region_tinted(tint, Point<int>(0, w-size), dim, Point<int>(dest_position.x, dest_position.y+dest_size.h-size)); // bottom left

	// Sides
	image->stretch_region_tinted_repeat(tint, Point<int>(size, 0), dim, Point<int>(dest_position.x+size, dest_position.y), Size<int>(dest_size.w-size*2, size)); // top
	image->stretch_region_tinted_repeat(tint, Point<int>(0, size), dim, Point<int>(dest_position.x, dest_position.y+size), Size<int>(size, dest_size.h-size*2)); // left
	image->stretch_region_tinted_repeat(tint, Point<int>(w-size, size), dim, Point<int>(dest_position.x+dest_size.w-size, dest_position.y+size), Size<int>(size, dest_size.h-size*2)); // right
	image->stretch_region_tinted_repeat(tint, Point<int>(size, w-size), dim, Point<int>(dest_position.x+size, dest_position.y+dest_size.h-size), Size<int>(dest_size.w-size*2, size)); // bottom

	// Middle
	image->stretch_region_tinted_repeat(tint, Point<int>(size, size), dim, dest_position+dim, dest_size-dim*2);

	image->end();
}

void Engine::draw_9patch(Image *image, Point<int> dest_position, Size<int> dest_size)
{
	draw_9patch_tinted(white, image, dest_position, dest_size);
}

void Engine::load_palette(std::string name)
{
	name = "palettes/" + name;

	SDL_RWops *file = open_file(name);

	char line[1000];

	SDL_fgets(file, line, 1000);
	if (strncmp(line, "GIMP Palette", 12)) {
		SDL_RWclose(file);
		throw LoadError("not a GIMP palette: " + name);
	}

	int line_count = 1;
	int colour_count = 0;

	while (SDL_fgets(file, line, 1000) != 0) {
		line_count++;
		char *p = line;
		while (*p != 0 && isspace(*p)) p++;
		// Skip comments
		if (*p == '#') {
			continue;
		}
		int red, green, blue;
		if (sscanf(line, "%d %d %d", &red, &green, &blue) == 3) {
			colours[colour_count].r = red;
			colours[colour_count].g = green;
			colours[colour_count].b = blue;
			colours[colour_count].a = 255;
			colour_count++;
		}
		else {
			infomsg("Syntax error on line %d of %s\n", line_count, name.c_str());
		}
	}

	black.r = black.g = black.b = 0;
	black.a = 255;

	shadow_colour = black;
	shadow_colour.a /= 2;

	white.r = white.g = white.b = white.a = 255;

	for (int i = 0; i < 4; i++) {
		four_blacks[i] = black;
		four_whites[i] = white;
	}

	magenta.r = 255;
	magenta.g = 0;
	magenta.b = 255;
	magenta.a = 255;

	SDL_RWclose(file);
}

std::string Engine::load_text(std::string filename)
{
	SDL_RWops *file = open_file(filename);

	Sint64 size = SDL_RWsize(file);

	char *buf = new char[(size_t)size+1];

	if (SDL_RWread(file, buf, 1, (size_t)size) != (size_t)size) {
		throw LoadError(filename);
	}

	buf[size] = 0;

	std::string s = buf;

	delete[] buf;

	return s;
}

void Engine::play_music(std::string name)
{
	if (music && music->get_name() == name) {
		return;
	}

	SDL_LockMutex(m.mixer_mutex);

	delete music;
	music = new MML(name);
	music->play(true);

	SDL_UnlockMutex(m.mixer_mutex);
}

void Engine::load_fonts()
{
	int actual_size;
	if (language == "English") {
		actual_size = 5;
	}
	else {
		actual_size = 7;
	}
	font = new Font("fff_minute.ttf", 8, actual_size);
	bold_font = new Font("fff_minute_bold.ttf", 8, actual_size);
}

void Engine::check_joysticks()
{
	int nj = SDL_NumJoysticks();
	if (nj != num_joysticks) {
		num_joysticks = nj;
		if (joy && SDL_JoystickGetAttached(joy)) {
			SDL_JoystickClose(joy);
		}
		joy = 0;
		if (num_joysticks > 0) {
			joy = SDL_JoystickOpen(0);
		}
	}
}

void Engine::set_mouse_cursor()
{
#ifdef NOOSKEWL_ENGINE_WINDOWS
	// Note: this needs to be a specific size on Windows, 32x32 works for me
	Size<int> size;
	unsigned char *pixels = Image::read_tga("images/mouse_cursor.tga", size);
	mouse_cursor = win_create_icon(GetActiveWindow(), (Uint8 *)pixels, size, 0, 0, true);
	delete[] pixels;
#endif
}

void Engine::update_projection()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	current_shader->set_matrix("model", glm::value_ptr(model));
	current_shader->set_matrix("view", glm::value_ptr(view));

	glm::mat4 d3d_fix = glm::translate(glm::mat4(), glm::vec3(-1.0f / (float)w, 1.0f / (float)h, 0.0f));
	current_shader->set_matrix("proj", glm::value_ptr(opengl ? proj : d3d_fix * proj));
}

void Engine::set_initial_d3d_state()
{
		d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
		d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		if (d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP) != D3D_OK) {
			infomsg("SetSamplerState failed\n");
		}
		if (d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP) != D3D_OK) {
			infomsg("SetSamplerState failed\n");
		}
		if (d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT) != D3D_OK) {
			infomsg("SetSamplerState failed\n");
		}
		if (d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT) != D3D_OK) {
			infomsg("SetSamplerState failed\n");
		}

		d3d_device->SetFVF(NOOSKEWL_ENGINE_FVF);

		d3d_device->BeginScene();
}

void Engine::maybe_expand_milestones(int number)
{
	if (milestones == 0) {
		milestones = (bool *)calloc(1, sizeof(bool) * (number+1));
		num_milestones = number+1;
	}
	else {
		if (number >= num_milestones) {
			milestones = (bool *)realloc(milestones, sizeof(bool) * (number+1));
			memset(milestones+num_milestones, 0, (number+1)-num_milestones);
			num_milestones = number+1;
		}
	}
}

void Engine::load_milestones()
{
	SDL_RWops *file = open_file("text/milestones.txt");
	char line[1000];

	while (SDL_fgets(file, line, 1000)) {
		if (line[strlen(line)-1] == '\n' || line[strlen(line)-1] == '\r') line[strlen(line)-1] = 0;
		if (line[strlen(line)-1] == '\n' || line[strlen(line)-1] == '\r') line[strlen(line)-1] = 0;
		Tokenizer t(line, '=');
		std::string num = t.next();
		std::string name = t.next();
		if (num != "" && name != "") {
			ms_name_to_number[name] = atoi(num.c_str());
			ms_number_to_name[atoi(num.c_str())] = name;
		}
	}
}

void Engine::clear_buffers()
{
	clear(black);
	clear_depth_buffer(1.0f);
}

} // End namespace Nooskewl_Engine

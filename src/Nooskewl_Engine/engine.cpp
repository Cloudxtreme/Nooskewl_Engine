#include "Nooskewl_Engine/Nooskewl_Engine.h"

#ifdef NOOSKEWL_ENGINE_WINDOWS
#define NOOSKEWL_ENGINE_FVF (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE4(1))
#endif

using namespace Nooskewl_Engine;

static void audio_callback(void *userdata, Uint8 *stream, int stream_length)
{
	memset(stream, m.device_spec.silence, stream_length);

	SDL_LockMutex(m.sample_mutex);
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
	SDL_UnlockMutex(m.sample_mutex);

	MML::mix(stream, stream_length);
}

namespace Nooskewl_Engine {

Engine noo;

Engine::Engine() :
	music(0),
	window_title("Nooskewl Engine"),
	joy_b1(10),
	key_b1(TGUIK_SPACE),
	map(0),
	tile_size(8),
	joy(0),
	num_joysticks(0),
	language("English"),
	did_intro(false)
{
}

Engine::~Engine()
{
}

void Engine::start(int argc, char **argv)
{
	mute = check_args(argc, argv, "+mute");
	fullscreen = check_args(argc, argv, "+fullscreen");
	vsync = !check_args(argc, argv, "-vsync");
#ifdef NOOSKEWL_ENGINE_WINDOWS
	opengl = !check_args(argc, argv, "+d3d");
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

	cpa = new CPA();

	set_mouse_cursor();

	init_audio();
	init_video();

	if (TTF_Init() == -1) {
		throw Error("TTF_Init failed");
	}

	load_fonts();

	logo = new Image("logo.tga");
	window_image = new Image("window.tga");
	window_image_with_name = new Image("window_with_name.tga");
	name_box_image = new Image("name_box.tga");

	load_palette("palette.gpl");

	MO3_Widget::static_start();
	Speech::static_start();

	setup_title_screen();

	button_mml = new MML("button.mml");

	intro_start = SDL_GetTicks();
}

void Engine::end()
{
	delete music;
	delete button_mml;

	if (map) {
		map->end();
	}
	delete map;
	delete player;

	MO3_Widget::static_end();
	Speech::static_end();

	delete gui;

	delete logo;
	delete window_image;
	delete window_image_with_name;
	delete name_box_image;

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

	SDL_DisplayMode mode;
	int win_w = 1280;
	int win_h = 720;

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
				float w = (float)mode.w / 16.0f;;
				float h = (float)mode.h / 9.0f;
				if (w > h) {
					win_w = mode.w;
					win_h = mode.w * 9 / 16;
				}
				else {
					win_w = mode.h * 16 / 9;
					win_h = mode.h;
				}
			}
		}
	}

	if (win_w > 1280 && win_h > 720) {
		// Huge windows are annoying
		win_w = 1280;
		win_h = 720;
	}

	window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_w, win_h, flags);
	if (window == 0) {
		throw Error("SDL_CreateWindow failed");
	}

	if (fullscreen) {
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	set_screen_size(w, h);

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
		//d3d_pp.BackBufferCount = 1;
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

		d3d_device->BeginScene();

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
	}
#endif

	std::string default_vertex_source;
	std::string default_fragment_source;
	std::string brighten_fragment_source;

	std::string tag = opengl ? "glsl" : "hlsl";

	default_vertex_source = load_text("shaders/" + tag + "/default_vertex.txt");
	default_fragment_source = load_text("shaders/" + tag + "/default_fragment.txt");
	brighten_fragment_source = load_text("shaders/" + tag + "/brighten_fragment.txt");

	default_shader = new Shader(opengl, default_vertex_source, default_fragment_source);
	brighten_shader = new Shader(opengl, default_vertex_source, brighten_fragment_source);

	current_shader = default_shader;
	current_shader->use();

	set_default_projection();

	m.vertex_cache = new Vertex_Cache();
	m.vertex_cache->init();
}

void Engine::shutdown_video()
{
	delete m.vertex_cache;

	delete default_shader;
	delete brighten_shader;

	if (opengl) {
		SDL_GL_DeleteContext(opengl_context);
	}
	else {
		// FIXME!!!
	}

	SDL_DestroyWindow(window);
}

void Engine::init_audio()
{
	if (mute) {
		return;
	}

	m.sample_mutex = SDL_CreateMutex();

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

	SDL_DestroyMutex(m.sample_mutex);
}

void Engine::handle_event(TGUI_Event *event)
{
#ifdef NOOSKEWL_ENGINE_WINDOWS
	if (event->type == TGUI_MOUSE_AXIS || event->type == TGUI_MOUSE_DOWN || event->type == TGUI_MOUSE_UP) {
		SetCursor(mouse_cursor);
	}
#endif

	if (gui) {
		gui->handle_event(event);
		if (new_game && new_game->pressed()) {
			delete gui;
			gui = 0;
			new_game = 0;

			Map::new_game_started();

			map = new Map("start.map");
			map->start();

			Player_Brain *player_brain = new Player_Brain();
			player = new Map_Entity(player_brain);
			player->load_sprite("player");
			player->set_position(Point<int>(1, 3));
			map->add_entity(player);
		}
	}
	else if (map) {
		map->handle_event(event);

		if (event->type == TGUI_KEY_DOWN && event->keyboard.code == TGUIK_ESCAPE) {
			delete map;
			map = 0;
			delete player;
			player = 0;
			setup_title_screen();
		}
	}
}

bool Engine::update()
{
	check_joysticks();

	if (map && map->update() == false) {
		std::string map_name;
		Point<int> position;
		Direction direction;
		map->get_new_map_details(map_name, position, direction);
		if (map_name != "") {
			Map *old_map = map;
			map = new Map(map_name);
			map->start();
			map->add_entity(player);

			// draw transition

			const Uint32 duration = 500;
			Uint32 start_time = SDL_GetTicks();
			Uint32 end_time = start_time + duration;
			bool moved_player = false;

			while (SDL_GetTicks() < end_time) {
				Uint32 elapsed = SDL_GetTicks() - start_time;
				if (moved_player == false && elapsed >= duration/2) {
					// The actual moving happens below in this same loop
					moved_player = true;
					player->set_position(position);
					player->set_direction(direction);
				}

				set_map_transition_projection((float)elapsed / duration * (float)M_PI);

				clear(black);

				m.vertex_cache->enable_perspective_drawing(screen_size);
				if (moved_player) {
					map->update_camera();
					map->draw(true);
				}
				else {
					old_map->update_camera();
					old_map->draw(true);
				}
				m.vertex_cache->disable_perspective_drawing();

				flip();
			}

			set_default_projection();

			old_map->end();
			delete old_map;
		}
		else {
			return false;
		}
	}

	return true;
}

void Engine::draw()
{
	clear(black);

	if (map) {
		map->draw();
	}
	if (new_game != NULL) {
		int scale1 = 4;
		int x1 = -logo->size.w / 2 * scale1;
		int y1 = -logo->size.h / 2 * scale1;
		int scale2 = 8;
		int x2 = -logo->size.w / 4 * scale2;
		int y2 = -logo->size.h / 4 * scale2;
		int dx1 = x1 + logo->size.w / 2 * scale1;
		int dy2 = y2 + logo->size.h / 4 * scale2;
		float p1 = (SDL_GetTicks() % 20000) / 20000.0f * 2.0f;
		if (p1 >= 1.0f) {
			p1 = 1.0f - (p1 - 1.0f);
		}
		float p2 = (SDL_GetTicks() % 30000) / 30000.0f * 2.0f;
		if (p2 >= 1.0f) {
			p2 = 1.0f - (p2 - 1.0f);
		}
		SDL_Colour tint1 = noo.white;
		tint1.a = 32;
		logo->stretch_region_tinted_single(tint1, Point<float>(0, 0), logo->size, Point<float>(x1 + (dx1 - x1) * p1, (float)y1), logo->size * scale1);
		SDL_Colour tint2 = noo.white;
		tint2.a = 64;
		logo->stretch_region_tinted_single(tint2, Point<float>(0, 0), logo->size, Point<float>((float)x2, y2 + (dy2 - y2) * p2), logo->size * scale2);
	}
	if (gui) {
		gui->draw();
	}
	if (new_game != NULL) {
		play_music("title.mml");

		if (did_intro == false) {
			int max_w = logo->size.w * 16 - logo->size.w;
			int max_h = logo->size.h * 16 - logo->size.h;
			float p = (SDL_GetTicks() - intro_start) / 2000.0f;
			if (p > 1.0f) {
				p = 1.0f;
				did_intro = true;
			}
			int w = int((1.0f - p) * max_w + logo->size.w);
			int h = int((1.0f - p) * max_h + logo->size.h);
			Point<int> pos;
			pos.x = screen_size.w / 2 - w / 2;
			pos.y = screen_size.h / 3 - h / 2;
			logo->stretch_region_single(Point<int>(0, 0), logo->size, pos, Size<int>(w, h));
		}
		else {
			Point<int> pos;
			pos.x = screen_size.w / 2 - logo->size.w / 2;
			pos.y = screen_size.h / 3 - logo->size.h / 2;
			logo->draw_single(pos);
		}
	}

	flip();
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

					load_fonts();

					Image::reload_all();
				}
			}
		}
		else {
			HRESULT hr = d3d_device->Present(0, 0, hwnd, 0);

			if (hr == D3DERR_DEVICELOST) {
				infomsg("D3D device lost\n");
				d3d_lost = true;

				font->clear_cache();
				bold_font->clear_cache();

				Image::release_all();
			}
		}

		d3d_device->BeginScene();
	}
#endif
}

void Engine::set_screen_size(int w, int h)
{
	if ((float)w/285.0f >= (float)h/160.0f) {
		scale = w / 285 + 1;
	}
	else {
		scale = h / 160 + 1;
	}
	screen_size.w = w / scale;
	screen_size.h = h / scale;

	if (gui) {
		gui->resize(screen_size.w, screen_size.h);
	}
}

void Engine::set_default_projection()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	model = glm::mat4();
	view = glm::scale(glm::mat4(), glm::vec3((float)scale, (float)scale, 1.0f));
	proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f);

	update_projection();
}

void Engine::set_map_transition_projection(float angle)
{
	model = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(angle >= M_PI/2.0f ? -1.0f : 1.0f, 1.0f, 1.0f));
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
	delete noo.music;
	noo.music = new MML(name);
	noo.music->play(true);
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

	if (opengl) {
		glViewport(0, 0, w, h);
		printGLerror("glViewport");
	}

	current_shader->set_matrix("model", glm::value_ptr(model));
	current_shader->set_matrix("view", glm::value_ptr(view));

	glm::mat4 d3d_fix = glm::translate(glm::mat4(), glm::vec3(-1.0f / (float)w, 1.0f / (float)h, 0.0f));
	current_shader->set_matrix("proj", glm::value_ptr(opengl ? proj : d3d_fix * proj));
}

void Engine::setup_title_screen()
{
	main_widget = new MO3_Widget(1.0f, 1.0f);
	new_game = new MO3_Text_Button("New Game");
	new_game->set_padding(0, 0, screen_size.h - screen_size.h / 6 - new_game->get_height(), 0);
	new_game->set_centered_x(true);
	new_game->set_parent(main_widget);
	gui = new TGUI(main_widget, screen_size.w, screen_size.h);
	gui->set_focus(new_game);
}

} // End namespace Nooskewl_Engine

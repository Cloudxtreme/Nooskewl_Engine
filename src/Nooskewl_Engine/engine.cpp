#include "Nooskewl_Engine/Nooskewl_Engine.h"

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
	window_title("Nooskewl Engine"),
	joy_b1(10),
	key_b1(TGUIK_SPACE),
	map(0),
	tile_size(8),
	joy(0),
	num_joysticks(0)
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

	init_audio();
	init_video();

	if (TTF_Init() == -1) {
		throw Error("TTF_Init failed");
	}

	load_fonts();

	window_image = new Image("window.tga");
	speech_arrow = new Sprite("speech_arrow");
	speech_arrow->start();
	logo = new Image("logo.tga");

	load_palette("palette.gpl");

	main_widget = new MO3_Widget(1.0f, 1.0f);
	new_game = new MO3_Text_Button("New Game");
	new_game->set_padding(0, 0, 100, 0);
	new_game->set_centered_x(true);
	new_game->set_parent(main_widget);
	gui = new TGUI(main_widget, noo.screen_w, noo.screen_h);
	gui->set_focus(new_game);
	// FIXME: make sure delete gui deletes widget

	music = new MML("title.mml");
	music->play(true);
}

void Engine::end()
{
	if (map) {
		map->end();
	}
	delete map;
	delete player;

	delete gui;

	delete window_image;
	delete speech_arrow;
	delete logo;

	delete font;
	delete bold_font;
	TTF_Quit();

	shutdown_video();
	shutdown_audio();

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

		const char *vertexSource =
			"#version 110\n"
			"uniform mat4 model;"
			"uniform mat4 view;"
			"uniform mat4 proj;"
			"attribute vec3 in_position;"
			"attribute vec4 in_colour;"
			"attribute vec2 in_texcoord;"
			"varying vec4 colour;"
			"varying vec2 texcoord;"
			"void main() {"
			"	colour = in_colour;"
			"	texcoord = in_texcoord;"
			"	gl_Position = proj * view * model * vec4(in_position, 1.0);"
			"}";
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		printGLerror("glCreateShader");
		glShaderSource(vertexShader, 1, &vertexSource, 0);
		printGLerror("glShaderSource");
		glCompileShader(vertexShader);
		printGLerror("glCompileShader");
		GLint status;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
		printGLerror("glGetShaderiv");
		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(vertexShader, 512, 0, buffer);
			errormsg("Vertex shader error: %s\n", buffer);
		}

		const char *fragmentSource =
			"#version 110\n"
			"uniform sampler2D tex;"
			"uniform bool use_tex;"
			"varying vec4 colour;"
			"varying vec2 texcoord;"
			"void main()"
			"{"
			"	if (use_tex) {"
			"		gl_FragColor = texture2D(tex, texcoord) * colour;"
			"	}"
			"	else {"
			"		gl_FragColor = colour;"
			"	}"
			"}";
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		printGLerror("glCreateShader");
		glShaderSource(fragmentShader, 1, &fragmentSource, 0);
		printGLerror("glShaderSource");
		glCompileShader(fragmentShader);
		printGLerror("glCompileShader");
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
		printGLerror("glGetShaderiv");
		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(fragmentShader, 512, 0, buffer);
			errormsg("Fragment shader error: %s\n", buffer);
		}

		m.current_shader = glCreateProgram();
		glAttachShader(m.current_shader, vertexShader);
		printGLerror("glAttachShader");
		glAttachShader(m.current_shader, fragmentShader);
		printGLerror("glAttachShader");
		glLinkProgram(m.current_shader);
		printGLerror("glLinkProgram");
		glUseProgram(m.current_shader);
		printGLerror("glUseProgram");

		glEnable(GL_BLEND);
		printGLerror("glEnable");
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		printGLerror("glBlendFunc");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		hwnd = GetActiveWindow();

		if ((d3d = Direct3DCreate9(D3D_SDK_VERSION)) == 0) {
			throw Error("Direct3D9CreateEx failed");
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
		if ((hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED, &d3d_pp, (LPDIRECT3DDEVICE9 *)&m.d3d_device)) != D3D_OK) {
			if ((hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED, &d3d_pp, (LPDIRECT3DDEVICE9 *)&m.d3d_device)) != D3D_OK) {
				throw Error("Unable to create D3D device");
			}
		}

		m.d3d_device->BeginScene();

		m.d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
		m.d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m.d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m.d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m.d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m.d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		if (m.d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP) != D3D_OK) {
			infomsg("SetSamplerState failed\n");
		}
		if (m.d3d_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP) != D3D_OK) {
			infomsg("SetSamplerState failed\n");
		}

		static const char *shader_source =
			"struct VS_INPUT\n"
			"{\n"
			"   float4 Position  : POSITION0;\n"
			"   float2 TexCoord  : TEXCOORD0;\n"
			"   float4 Color	 : TEXCOORD1;\n"
			"};\n"
			"struct VS_OUTPUT\n"
			"{\n"
			"   float4 Position  : POSITION0;\n"
			"   float4 Color	 : COLOR0;\n"
			"   float2 TexCoord  : TEXCOORD0;\n"
			"};\n"
			"\n"
			"float4x4 proj;\n"
			"float4x4 view;\n"
			"float4x4 model;\n"
			"\n"
			"VS_OUTPUT vs_main(VS_INPUT Input)\n"
			"{\n"
			"   VS_OUTPUT Output;\n"
			"   Output.Color = Input.Color;\n"
			"   Output.TexCoord = Input.TexCoord;\n"
			"   Output.Position = mul(Input.Position, mul(model, mul(view, proj)));\n"
			"   return Output;\n"
			"}\n"
			"bool use_tex;\n"
			"texture tex;\n"
			"sampler2D s = sampler_state {\n"
			"   texture = <tex>;\n"
			"};\n"
			"\n"
			"float4 ps_main(VS_OUTPUT Input) : COLOR0\n"
			"{\n"
			"   if (use_tex) {\n"
			"	  return Input.Color * tex2D(s, Input.TexCoord);\n"
			"   }\n"
			"   else {\n"
			"	  return Input.Color;\n"
			"   }\n"
			"}\n"
			"technique TECH\n"
			"{\n"
			"		pass p1\n"
			"		{\n"
			"				VertexShader = compile vs_2_0 vs_main();\n"
			"				PixelShader = compile ps_2_0 ps_main();\n"
			"		}\n"
			"}\n";

		LPD3DXBUFFER errors;

		DWORD ok = D3DXCreateEffect(
			m.d3d_device,
			shader_source,
			strlen(shader_source),
			0,
			0,
			D3DXSHADER_PACKMATRIX_ROWMAJOR,
			0,
			&m.effect,
			&errors
			);

		if (ok != D3D_OK) {
			char *msg = (char *)errors->GetBufferPointer();
			throw Error("Shader error: " + std::string(msg));
		}

		D3DXHANDLE hTech;
		hTech = m.effect->GetTechniqueByName("TECH");
		m.effect->ValidateTechnique(hTech);
		m.effect->SetTechnique(hTech);
	}
#endif

	set_default_projection();

	m.vertex_cache = new Vertex_Cache();
	m.vertex_cache->init();
}

void Engine::shutdown_video()
{
	delete m.vertex_cache;

	if (opengl) {
		glDeleteProgram(m.current_shader);
		printGLerror("glDeleteProgram");
		glDeleteShader(fragmentShader);
		printGLerror("glDeleteShader");
		glDeleteShader(vertexShader);
		printGLerror("glDeleteShader");

		SDL_GL_DeleteContext(opengl_context);
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
	if (gui) {
		gui->handle_event(event);
		if (new_game && new_game->pressed()) {
			delete gui;
			gui = 0;
			new_game = 0;

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
	}
}

bool Engine::update()
{
	check_joysticks();

	speech_arrow->update();

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

				set_map_transition_projection((float)elapsed / duration * PI);

				clear(black);

				m.vertex_cache->set_perspective_drawing(true);
				if (moved_player) {
					map->update_camera();
					map->draw();
				}
				else {
					old_map->update_camera();
					old_map->draw();
				}
				m.vertex_cache->set_perspective_drawing(false);

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
	if (gui) {
		gui->draw();
	}
	if (new_game != NULL) {
		int x = noo.screen_w / 2 - logo->w / 2;
		int y = noo.screen_h / 3 - logo->h / 2;
		logo->draw_single(Point<int>(x, y), 0);
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
		m.d3d_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_RGBA(colour.r, colour.g, colour.b, colour.a), 0.0f, 0);
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
		m.d3d_device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, value, 0);
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
			m.d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			m.d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			m.d3d_device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
			m.d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			m.d3d_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			m.d3d_device->SetRenderState(D3DRS_ALPHAREF, 1);
		}
		else {
			m.d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			m.d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			m.d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
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
		m.d3d_device->EndScene();

		if (d3d_lost) {
			HRESULT hr = m.d3d_device->TestCooperativeLevel();
			if (hr == D3DERR_DEVICENOTRESET) {
				hr = m.d3d_device->Reset(&d3d_pp);
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
			HRESULT hr = m.d3d_device->Present(0, 0, hwnd, 0);

			if (hr == D3DERR_DEVICELOST) {
				infomsg("D3D device lost\n");
				d3d_lost = true;

				font->clear_cache();
				bold_font->clear_cache();

				Image::release_all();
			}
		}

		m.d3d_device->BeginScene();
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
	screen_w = w / scale;
	screen_h = h / scale;

	if (gui) {
		gui->resize(screen_w, screen_h);
	}
}

void Engine::set_default_projection()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	glm::mat4 proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
	glm::mat4 view = glm::scale(glm::mat4(), glm::vec3((float)scale, (float)scale, 1.0f));
	glm::mat4 model = glm::mat4();

	if (opengl) {
		glViewport(0, 0, w, h);
		printGLerror("glViewport");

		GLint uni;

		uni = glGetUniformLocation(m.current_shader, "proj");
		printGLerror("glGetUniformLocation");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(proj));
		printGLerror("glUniformMatrix4fv");

		uni = glGetUniformLocation(m.current_shader, "view");
		printGLerror("glGetUniformLocation");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(view));
		printGLerror("glUniformMatrix4fv");

		uni = glGetUniformLocation(m.current_shader, "model");
		printGLerror("glGetUniformLocation");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(model));
		printGLerror("glUniformMatrix4fv");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		/* D3D pixels are slightly different than OpenGL */
		glm::mat4 d3d_fix = glm::translate(glm::mat4(), glm::vec3(-1.0f / (float)w, 1.0f / (float)h, 0.0f));

		m.effect->SetMatrix("proj", (LPD3DXMATRIX)glm::value_ptr(d3d_fix * proj));
		m.effect->SetMatrix("view", (LPD3DXMATRIX)glm::value_ptr(view));
		m.effect->SetMatrix("model", (LPD3DXMATRIX)glm::value_ptr(model));
	}
#endif
}

void Engine::set_map_transition_projection(float angle)
{
	glm::mat4 proj = glm::frustum(-1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1000.0f);
	glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 model = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(angle >= PI/2.0f ? -1.0f : 1.0f, 1.0f, 1.0f));

	if (opengl) {
		GLint uni;

		uni = glGetUniformLocation(m.current_shader, "proj");
		printGLerror("glGetUniformLocation");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(proj));
		printGLerror("glUniformMatrix4fv");

		uni = glGetUniformLocation(m.current_shader, "view");
		printGLerror("glGetUniformLocation");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(view));
		printGLerror("glUniformMatrix4fv");

		uni = glGetUniformLocation(m.current_shader, "model");
		printGLerror("glGetUniformLocation");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(model));
		printGLerror("glUniformMatrix4fv");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		/* D3D pixels are slightly different than OpenGL */
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		glm::mat4 d3d_fix = glm::translate(glm::mat4(), glm::vec3(-1.0f / (float)w, 1.0f / (float)h, 0.0f));

		m.effect->SetMatrix("proj", (LPD3DXMATRIX)glm::value_ptr(d3d_fix * proj));
		m.effect->SetMatrix("view", (LPD3DXMATRIX)glm::value_ptr(view));
		m.effect->SetMatrix("model", (LPD3DXMATRIX)glm::value_ptr(model));
	}
#endif
}

void Engine::draw_line(SDL_Colour colour, Point<int> a, Point<int> b)
{
	SDL_Colour vertex_colours[4];
	for (int i = 0; i < 4; i++) {
		vertex_colours[i] = colour;
	}
	float x1 = (float)a.x;
	float y1 = (float)a.y;
	float x2 = (float)b.x;
	float y2 = (float)b.y;
	float dx = x2 - x1;
	float dy = y2 - y1;
	float angle = atan2(dy, dx);
	float a1 = angle + PI / 2.0f;
	float a2 = angle - PI / 2.0f;
	float scale = 0.5f * noo.scale;
	Point<float> da = a;
	Point<float> db = a;
	Point<float> dc = b;
	Point<float> dd = b;
	da.x += cos(a1) * 0.5f;
	da.y += sin(a1) * 0.5f;
	db.x += cos(a2) * 0.5f;
	db.y += sin(a2) * 0.5f;
	dc.x += cos(a1) * 0.5f;
	dc.y += sin(a1) * 0.5f;
	dd.x += cos(a2) * 0.5f;
	dd.y += sin(a2) * 0.5f;
	if (opengl) {
		glDisable(GL_TEXTURE_2D);
		printGLerror("glBindTexture");
	}
	m.vertex_cache->start();
	m.vertex_cache->buffer(Point<int>(0, 0), Size<int>(0, 0), da, dc, dd, db, vertex_colours, 0);
	m.vertex_cache->end();
	if (opengl) {
		glEnable(GL_TEXTURE_2D);
		printGLerror("glBindTexture");
	}
}

void Engine::draw_quad(SDL_Colour vertex_colours[4], Point<int> dest_position, Size<int> dest_size)
{
	if (opengl) {
		glDisable(GL_TEXTURE_2D);
		printGLerror("glBindTexture");
	}
	m.vertex_cache->start();
	m.vertex_cache->buffer(Point<int>(0, 0), Size<int>(0, 0), dest_position, dest_size, vertex_colours, 0);
	m.vertex_cache->end();
	if (opengl) {
		glEnable(GL_TEXTURE_2D);
		printGLerror("glBindTexture");
	}
}

void Engine::draw_quad(SDL_Colour colour, Point<int> dest_position, Size<int> dest_size)
{
	static SDL_Colour vertex_colours[4];
	for (int i = 0; i < 4; i++) {
		vertex_colours[i] = colour;
	}
	draw_quad(vertex_colours, dest_position, dest_size);
}

void Engine::draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle)
{
	SDL_Colour colour = colours[44]; // blue
	colour.a = 240;

	draw_quad(colour, dest_position+1, dest_size-2);

	int iw = window_image->w;
	int sz = iw / 3;

	window_image->start();
	window_image->draw_region(Point<int>(0, 0), Size<int>(sz, sz), dest_position, 0); // top left
	window_image->draw_region(Point<int>(iw-sz, 0), Size<int>(sz, sz), Point<int>(dest_position.x+dest_size.w-sz, dest_position.y), 0); // top right
	window_image->draw_region(Point<int>(iw-sz, iw-sz), Size<int>(sz, sz), dest_position+dest_size-sz, 0); // bottom right
	window_image->draw_region(Point<int>(0, iw-sz), Size<int>(sz, sz), Point<int>(dest_position.x, dest_position.y+dest_size.h-sz), 0); // bottom left
	window_image->stretch_region(Point<int>(sz, 0), Size<int>(sz, sz), Point<int>(dest_position.x+sz, dest_position.y), Size<int>(dest_size.w-sz*2, sz), 0); // top
	window_image->stretch_region(Point<int>(sz, iw-sz), Size<int>(sz, sz), Point<int>(dest_position.x+sz, dest_position.y+dest_size.h-sz), Size<int>(dest_size.w-sz*2, sz), 0); // bottom
	window_image->stretch_region(Point<int>(0, sz), Size<int>(sz, sz), Point<int>(dest_position.x, dest_position.y+sz), Size<int>(sz, dest_size.h-sz*2), 0); // left
	window_image->stretch_region(Point<int>(iw-sz, sz), Size<int>(sz, sz), Point<int>(dest_position.x+dest_size.w-sz, dest_position.y+sz), Size<int>(sz, dest_size.h-sz*2), 0); // right
	window_image->end();

	if (circle) {
		speech_arrow->set_animation("circle");
		Image *i = speech_arrow->get_current_image();
		speech_arrow->get_current_image()->draw_single(dest_position+dest_size-Point<int>(i->w+sz, i->h+sz), 0);
	}
	else if (arrow) {
		speech_arrow->set_animation("arrow");
		Image *i = speech_arrow->get_current_image();
		speech_arrow->get_current_image()->draw_single(dest_position+dest_size-Point<int>(i->w+sz, i->h+sz), 0);
	}
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

void Engine::load_fonts()
{
	font = new Font("fff_majestica.ttf", 9);
	bold_font = new Font("fff_majestica_bold.ttf", 9);
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

} // End namespace Nooskewl_Engine
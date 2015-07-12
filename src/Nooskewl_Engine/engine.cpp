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

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::start(int argc, char **argv)
{
	mute = check_args(argc, argv, "+mute");
	vsync = !check_args(argc, argv, "-vsync");
#ifdef _MSC_VER
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

	if (SDL_NumJoysticks() > 0) {
		joy = SDL_JoystickOpen(0);
	}
	else {
		joy = NULL;
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
	load_palette("nes.gpl");

	map = new Map("test.map");

	Player_Brain *player_brain = new Player_Brain();
	player = new Map_Entity(player_brain);
	player->load_sprite("player");
	player->set_position(Point<int>(1, 3));
	map->add_entity(player);
}

void Engine::stop()
{
	delete map;

	delete window_image;

	delete font;
	delete bold_font;
	TTF_Quit();

	shutdown_video();
	shutdown_audio();

	delete cpa;

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

	// Find close 16:9 window size
	for (int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
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

	infomsg("Trying window size %dx%d\n", win_w, win_h);

	window = SDL_CreateWindow("SS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_w, win_h, flags);
	if (window == NULL) {
		throw Error("SDL_CreateWindow failed");
	}

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	set_screen_size(w, h);

	if (opengl) {
		opengl_context = SDL_GL_CreateContext(window);
		SDL_GL_SetSwapInterval(vsync ? 1 : 0); // vsync, 1 = on

		glewExperimental = 0;
		glewInit();

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
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);
		GLint status;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
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
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			char buffer[512];
			glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
			errormsg("Fragment shader error: %s\n", buffer);
		}

		m.current_shader = glCreateProgram();
		glAttachShader(m.current_shader, vertexShader);
		glAttachShader(m.current_shader, fragmentShader);
		glLinkProgram(m.current_shader);
		glUseProgram(m.current_shader);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
#ifdef _MSC_VER
	else {
		hwnd = GetActiveWindow();

		if ((d3d = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
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
		m.d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);

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
			NULL,
			NULL,
			D3DXSHADER_PACKMATRIX_ROWMAJOR,
			NULL,
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
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);

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
	desired.userdata = NULL;

	audio_device = SDL_OpenAudioDevice(NULL, false, &desired, &m.device_spec, 0);

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
	map->handle_event(event);
}

bool Engine::update()
{
	speech_arrow->update();

	if (map->update() == false) {
		std::string map_name;
		Point<int> position;
		Direction direction;
		map->get_new_map_details(map_name, position, direction);
		if (map_name != "") {
			Map *old_map = map;
			map = new Map(map_name);
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

	map->draw();

	flip();
}

void Engine::clear(SDL_Colour colour)
{
	if (opengl) {
		glClearColor(colour.r/255.0f, colour.g/255.0f, colour.b/255.0f, colour.a/255.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
#ifdef _MSC_VER
	else {
		m.d3d_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(colour.r, colour.g, colour.b, colour.a), 0.0f, 0);
	}
#endif
}

void Engine::clear_depth_buffer(float value)
{
	if (opengl) {
		glClearDepth(value);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
#ifdef _MSC_VER
	else {
		m.d3d_device->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, value, 0);
	}
#endif
}

void Engine::flip()
{
	if (opengl) {
		SDL_GL_SwapWindow(window);
	}
#ifdef _MSC_VER
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
			HRESULT hr = m.d3d_device->Present(NULL, NULL, hwnd, NULL);

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

		GLint uni;

		uni = glGetUniformLocation(m.current_shader, "proj");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(proj));

		uni = glGetUniformLocation(m.current_shader, "view");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(view));

		uni = glGetUniformLocation(m.current_shader, "model");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(model));
	}
#ifdef _MSC_VER
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
	glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f));
	glm::mat4 model = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(angle >= PI/2.0f ? -1.0f : 1.0f, 1.0f, 1.0f));

	if (opengl) {
		GLint uni;

		uni = glGetUniformLocation(m.current_shader, "proj");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(proj));

		uni = glGetUniformLocation(m.current_shader, "view");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(view));

		uni = glGetUniformLocation(m.current_shader, "model");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(model));
	}
#ifdef _MSC_VER
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

void Engine::draw_line(Point<int> a, Point<int> b, SDL_Colour colour)
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
	// FIXME: 4 hardcoded
	float scale = 0.5f;
	Point<float> da = a;
	Point<float> db = a;
	Point<float> dc = b;
	Point<float> dd = b;
	da.x += cos(a1) * scale;
	da.y += sin(a1) * scale;
	db.x += cos(a2) * scale;
	db.y += sin(a2) * scale;
	dc.x += cos(a1) * scale;
	dc.y += sin(a1) * scale;
	dd.x += cos(a2) * scale;
	dd.y += sin(a2) * scale;
	if (opengl) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	m.vertex_cache->start();
	m.vertex_cache->buffer(Point<int>(0, 0), Size<int>(0, 0), da, dc, dd, db, vertex_colours, 0);
	m.vertex_cache->end();
}

void Engine::draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4])
{
	if (opengl) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	m.vertex_cache->start();
	m.vertex_cache->buffer(Point<int>(0, 0), Size<int>(0, 0), dest_position, dest_size, vertex_colours, 0);
	m.vertex_cache->end();
}

void Engine::draw_quad(Point<int> dest_position, Size<int> dest_size, SDL_Colour colour)
{
	static SDL_Colour vertex_colours[4];
	for (int i = 0; i < 4; i++) {
		vertex_colours[i] = colour;
	}
	draw_quad(dest_position, dest_size, vertex_colours);
}

void Engine::draw_window(Point<int> dest_position, Size<int> dest_size, bool arrow, bool circle)
{
	SDL_Colour vertex_colours[4];

	// Blue shades in NES palette
	vertex_colours[0] = vertex_colours[1] = colours[47];
	vertex_colours[2] = vertex_colours[3] = colours[44];

	for (int i = 0; i < 4; i++) {
		vertex_colours[i].a = 220;
	}

	draw_quad(dest_position+1, dest_size-2, vertex_colours);

	window_image->start();
	window_image->draw_region(Point<int>(0, 0), Size<int>(6, 6), dest_position, 0); // top left
	window_image->draw_region(Point<int>(6, 0), Size<int>(6, 6), Point<int>(dest_position.x+dest_size.w-6, dest_position.y), 0); // top right
	window_image->draw_region(Point<int>(6, 6), Size<int>(6, 6), dest_position+dest_size-6, 0); // bottom right
	window_image->draw_region(Point<int>(0, 6), Size<int>(6, 6), Point<int>(dest_position.x, dest_position.y+dest_size.h-6), 0); // bottom left
	window_image->stretch_region(Point<int>(5, 1), Size<int>(2, 4), Point<int>(dest_position.x+6, dest_position.y+1), Size<int>(dest_size.w-12, 4), 0); // top
	window_image->stretch_region(Point<int>(5, 7), Size<int>(2, 4), Point<int>(dest_position.x+6, dest_position.y+dest_size.h-5), Size<int>(dest_size.w-12, 4), 0); // bottom
	window_image->stretch_region(Point<int>(1, 5), Size<int>(4, 2), Point<int>(dest_position.x+1, dest_position.y+6), Size<int>(4, dest_size.h-12), 0); // left
	window_image->stretch_region(Point<int>(7, 5), Size<int>(4, 2), Point<int>(dest_position.x+dest_size.w-5, dest_position.y+6), Size<int>(4, dest_size.h-12), 0); // right
	window_image->end();

	if (circle) {
		speech_arrow->set_animation("circle");
		speech_arrow->get_current_image()->draw_single(dest_position+dest_size-12, 0);
	}
	else if (arrow) {
		speech_arrow->set_animation("arrow");
		speech_arrow->get_current_image()->draw_single(dest_position+dest_size-12, 0);
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

	while (SDL_fgets(file, line, 1000) != NULL) {
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
	white.r = white.g = white.b = white.a = 255;

	for (int i = 0; i < 4; i++) {
		four_blacks[i] = black;
		four_whites[i] = white;
	}

	SDL_RWclose(file);
}

void Engine::load_fonts()
{
	font = new Font("fff_majestica.ttf", 8);
	bold_font = new Font("fff_majestica_bold.ttf", 8);
}

} // End namespace Nooskewl_Engine
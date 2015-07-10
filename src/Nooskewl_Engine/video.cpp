#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/global.h"
#include "Nooskewl_Engine/log.h"
#include "Nooskewl_Engine/module.h"
#include "Nooskewl_Engine/resource_manager.h"
#include "Nooskewl_Engine/util.h"
#include "Nooskewl_Engine/vertex_accel.h"
#include "Nooskewl_Engine/video.h"

// FIXME: put these something neat
static SDL_Window *window;

GLuint vertexShader;
GLuint fragmentShader;
GLuint current_shader;
static SDL_GLContext opengl_context;

#ifdef _MSC_VER
static HWND hwnd;
static D3DPRESENT_PARAMETERS d3d_pp;
static bool d3d_lost;
static IDirect3D9 *d3d;
#endif

void clear(SDL_Colour colour)
{
	if (g.graphics.opengl) {
		glClearColor(colour.r/255.0f, colour.g/255.0f, colour.b/255.0f, colour.a/255.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
#ifdef _MSC_VER
	else {
		m.d3d_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(colour.r, colour.g, colour.b, colour.a), 0, 0);
	}
#endif
}

void flip()
{
	if (g.graphics.opengl) {
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
					reload_graphics();
				}
			}
		}
		else {
			HRESULT hr = m.d3d_device->Present(NULL, NULL, hwnd, NULL);

			if (hr == D3DERR_DEVICELOST) {
				infomsg("D3D device lost\n");
				d3d_lost = true;
				release_graphics();
			}
		}

		m.d3d_device->BeginScene();
	}
#endif
}

void init_video(int argc, char **argv)
{
	bool vsync = !check_args(argc, argv, "-vsync");
#ifdef _MSC_VER
	g.graphics.opengl = !check_args(argc, argv, "+d3d");
#else
	g.graphics.opengl = true;
#endif

	if (g.graphics.opengl) {
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	}

	g.graphics.screen_w = 285;
	g.graphics.screen_h = 160;

	int flags = SDL_WINDOW_RESIZABLE;
	if (g.graphics.opengl) {
		flags |= SDL_WINDOW_OPENGL;
	}

	window = SDL_CreateWindow("SS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g.graphics.screen_w * 4, g.graphics.screen_h * 4, flags);
	if (window == NULL) {
		throw Error("SDL_CreateWindow failed");
	}

	if (g.graphics.opengl) {
		opengl_context = SDL_GL_CreateContext(window);
		SDL_GL_SetSwapInterval(vsync ? 1 : 0); // vsync, 1 = on

		glewExperimental = 0;
		glewInit();

		const char *vertexSource =
			"#version 110\n"
			"uniform mat4 model;"
			"uniform mat4 view;"
			"uniform mat4 proj;"
			"in vec3 in_position;"
			"in vec4 in_colour;"
			"in vec2 in_texcoord;"
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

		current_shader = glCreateProgram();
		glAttachShader(current_shader, vertexShader);
		glAttachShader(current_shader, fragmentShader);
		glLinkProgram(current_shader);
		glUseProgram(current_shader);

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
		d3d_pp.BackBufferWidth = g.graphics.screen_w*4;
		d3d_pp.BackBufferHeight = g.graphics.screen_h*4;
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

	g.graphics.vertex_accel = new Vertex_Accel();
	g.graphics.vertex_accel->init();
}

void shutdown_video()
{
	delete g.graphics.vertex_accel;

	if (g.graphics.opengl) {
		glDeleteProgram(current_shader);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);

		SDL_GL_DeleteContext(opengl_context);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
}

void set_default_projection()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	glm::mat4 proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
	glm::mat4 view = glm::scale(glm::mat4(), glm::vec3(4.0f, 4.0f, 4.0f));
	glm::mat4 model = glm::mat4();

	if (g.graphics.opengl) {
		glViewport(0, 0, w, h);

		GLint uni;

		uni = glGetUniformLocation(current_shader, "proj");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(proj));

		uni = glGetUniformLocation(current_shader, "view");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(view));

		uni = glGetUniformLocation(current_shader, "model");
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

void set_map_transition_projection(float angle)
{
	glm::mat4 proj = glm::frustum(1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f));
	glm::mat4 model = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(angle >= PI/2 ? -4.0f : 4.0f, 4.0f, 4.0f));

	if (g.graphics.opengl) {
		GLint uni;

		uni = glGetUniformLocation(current_shader, "proj");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(proj));

		uni = glGetUniformLocation(current_shader, "view");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(view));

		uni = glGetUniformLocation(current_shader, "model");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(model));
	}
#ifdef _MSC_VER
	else {
		/* D3D pixels are slightly different than OpenGL */
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		glm::mat4 d3d_fix = glm::translate(glm::mat4(), glm::vec3(-1.0f / (float)w, 1.0f / (float)h, 0.0f));

		m.effect->SetMatrix("proj", (LPD3DXMATRIX)glm::value_ptr(proj));
		m.effect->SetMatrix("view", (LPD3DXMATRIX)glm::value_ptr(view));
		m.effect->SetMatrix("model", (LPD3DXMATRIX)glm::value_ptr(model));
	}
#endif
}

void release_graphics()
{
	release_fonts();
	release_images();
}

void reload_graphics()
{
	load_fonts();
	reload_images();
}
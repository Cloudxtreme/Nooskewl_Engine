#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/log.h"
#include "Nooskewl_Engine/util.h"
#include "Nooskewl_Engine/vertex_accel.h"
#include "Nooskewl_Engine/video.h"

// FIXME: private
GLuint vertexShader;
GLuint fragmentShader;
GLuint current_shader;

SDL_Renderer *renderer;
IDirect3D9 *d3d;
IDirect3DDevice9 *d3d_device;
LPD3DXEFFECT effect;
HWND hwnd;

int screen_w;
int screen_h;

bool opengl;

static SDL_Window *window;
static SDL_GLContext opengl_context;

void clear(SDL_Colour colour)
{
	if (opengl) {
		glClearColor(colour.r/255.0f, colour.g/255.0f, colour.b/255.0f, colour.a/255.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	else {
		d3d_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(colour.r, colour.g, colour.b, colour.a), 0, 0);
	}
}

void flip()
{
	if (opengl) {
		SDL_GL_SwapWindow(window);
	}
	else {
		d3d_device->EndScene();

		HRESULT hr = d3d_device->Present(NULL, NULL, hwnd, NULL);

		if (hr == D3DERR_DEVICELOST) {
			infomsg("Device lost");
		}

		d3d_device->BeginScene();
	}
}

void init_video(int argc, char **argv)
{
	bool vsync = !check_args(argc, argv, "-vsync");
	opengl = !check_args(argc, argv, "+d3d");

	if (opengl) {
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	}

	screen_w = 285;
	screen_h = 160;

	int flags = SDL_WINDOW_RESIZABLE;
	if (opengl) {
		flags |= SDL_WINDOW_OPENGL;
	}

	window = SDL_CreateWindow("SS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w * 4, screen_h * 4, flags);
	if (window == NULL) {
		throw Error("SDL_CreateWindow failed");
	}

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
			"in vec3 in_position;"
			"in vec4 in_color;"
			"in vec2 in_texcoord;"
			"varying vec4 colour;"
			"varying vec2 texcoord;"
			"void main() {"
			"	colour = in_color;"
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
	else {
		hwnd = GetActiveWindow();

		if ((d3d = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
			throw Error("Direct3D9CreateEx failed");
		}

		D3DPRESENT_PARAMETERS d3d_pp;

		ZeroMemory(&d3d_pp, sizeof(d3d_pp));

		d3d_pp.BackBufferFormat = D3DFMT_X8R8G8B8;

		d3d_pp.BackBufferWidth = screen_w*4;
		d3d_pp.BackBufferHeight = screen_h*4;
		//d3d_pp.BackBufferCount = 1;
		d3d_pp.Windowed = 1;
		if (vsync) {
			d3d_pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
		}
		else {
			d3d_pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		//d3d_pp.EnableAutoDepthStencil = true;
		//d3d_pp.AutoDepthStencilFormat = D3DFMT_D24S8;

		//d3d_pp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		d3d_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3d_pp.hDeviceWindow = hwnd;

		HRESULT hr;
		if ((hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING/* | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED*/, &d3d_pp, (LPDIRECT3DDEVICE9 *)&d3d_device)) != D3D_OK) {
			throw Error("Unable to create D3D device");
		}

		d3d_device->BeginScene();

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
			d3d_device,
			shader_source,
			strlen(shader_source),
			NULL,
			NULL,
			D3DXSHADER_PACKMATRIX_ROWMAJOR,
			NULL,
			&effect,
			&errors
			);

		if (ok != D3D_OK) {
			char *msg = (char *)errors->GetBufferPointer();
			throw Error("Shader error: " + std::string(msg));
		}

		D3DXHANDLE hTech;
		hTech = effect->GetTechniqueByName("TECH");
		effect->ValidateTechnique(hTech);
		effect->SetTechnique(hTech);
	}

	set_default_projection();

	vertex_accel = new Vertex_Accel();
	vertex_accel->init();
}

void shutdown_video()
{
	delete vertex_accel;

	if (opengl) {
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

	if (opengl) {
		glViewport(0, 0, w, h);

		GLint uni;

		uni = glGetUniformLocation(current_shader, "proj");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(proj));

		uni = glGetUniformLocation(current_shader, "view");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(view));

		uni = glGetUniformLocation(current_shader, "model");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(model));
	}
	else {
		effect->SetMatrix("proj", (LPD3DXMATRIX)glm::value_ptr(proj));
		effect->SetMatrix("view", (LPD3DXMATRIX)glm::value_ptr(view));
		effect->SetMatrix("model", (LPD3DXMATRIX)glm::value_ptr(model));

		/*
		D3DXMATRIX proj;
		D3DXMatrixIdentity(&proj);
		D3DXMatrixOrthoRH(&proj, w, h, -1.0f, 1.0f);
		effect->SetMatrix("proj", &proj);

		D3DXMATRIX view;
		D3DXMatrixIdentity(&view);
		D3DXMatrixScaling(&view, 4.0f, 4.0f, 4.0f);
		effect->SetMatrix("view", &view);

		D3DXMATRIX model;
		D3DXMatrixIdentity(&model);
		effect->SetMatrix("model", &model);
		*/
	}
}

void set_map_transition_projection(float angle)
{
	glm::mat4 proj = glm::frustum(1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -2.0f));
	glm::mat4 model = glm::rotate(glm::mat4(), angle, glm::vec3(0.0f, 1.0f, 0.0f));

	if (opengl) {
		GLint uni;

		uni = glGetUniformLocation(current_shader, "model");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(proj));

		uni = glGetUniformLocation(current_shader, "view");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(view));

		model = glm::scale(model, glm::vec3(angle >= PI/2 ? -4.0f : 4.0f, 4.0f, 4.0f));
		uni = glGetUniformLocation(current_shader, "proj");
		glUniformMatrix4fv(uni, 1, GL_FALSE, glm::value_ptr(model));
	}
	else {
		effect->SetMatrix("proj", (LPD3DXMATRIX)glm::value_ptr(proj));
		effect->SetMatrix("view", (LPD3DXMATRIX)glm::value_ptr(view));
		effect->SetMatrix("model", (LPD3DXMATRIX)glm::value_ptr(model));

		/*
		D3DXMATRIX proj;
		D3DXMatrixIdentity(&proj);
		D3DXMatrixPerspectiveOffCenterRH(&proj, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		effect->SetMatrix("proj", &proj);

		D3DXMATRIX view;
		D3DXMatrixIdentity(&view);
		D3DXMatrixTranslation(&view, 0.0f, 0.0f, -2.0f);
		effect->SetMatrix("view", &view);

		D3DXMATRIX model;
		D3DXMatrixIdentity(&model);
		D3DXMatrixRotationY(&model, angle);
		D3DXMatrixScaling(&model, angle >= PI/2 ? -4.0f : 4.0f, 4.0f, 4.0f);
		effect->SetMatrix("model", &model);
		*/
	}
}
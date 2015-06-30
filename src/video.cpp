#include "starsquatters.h"
#include "log.h"
#include "video.h"

// FIXME: private
SDL_Window *window;
SDL_GLContext context;
GLuint vertexShader;
GLuint fragmentShader;
GLuint current_shader;

void flip()
{
	SDL_GL_SwapWindow(window);
}

bool init_video()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		return false;
	}

	context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(1);

	glewExperimental = 1;
	glewInit();

	const char *vertexSource =
		"#version 150 core\n"
		"in vec3 position;"
		"in vec4 color;"
		"in vec2 texcoord;"
		"out vec4 Color;"
		"out vec2 Texcoord;"
		"uniform mat4 proj;"
		"void main() {"
		"	Color = color;"
		"	Texcoord = texcoord;"
		"	gl_Position = proj * vec4(position, 1.0);"
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
		"#version 150 core\n"
		"in vec4 Color;"
		"in vec2 Texcoord;"
		"out vec4 outColor;"
		"uniform sampler2D tex;"
		"void main()"
		"{"
		"	outColor = texture(tex, Texcoord) * Color;"
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
	glBindFragDataLocation(current_shader, 0, "outColor");
	glLinkProgram(current_shader);
	glUseProgram(current_shader);

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	glm::mat4 ortho = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
	GLint uniTrans = glGetUniformLocation(current_shader, "proj");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(ortho));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void shutdown_video()
{
	glDeleteProgram(current_shader);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
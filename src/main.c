#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "tga.h"

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_Window *window = SDL_CreateWindow("Hello World!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		return 1;
	}

	SDL_GLContext *context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(1);

	glewExperimental = 1;
	glewInit();

	GLuint texture = load_tga("test.tga");
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float vertices[] = {
        -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // Top-left
         0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // Top-right
         0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
    };
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an element array
    GLuint ebo;
    glGenBuffers(1, &ebo);

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	const char *vertexSource =
		"#version 150 core\n"
		"in vec2 position;"
		"in vec3 color;"
		"in vec2 texcoord;"
		"out vec3 Color;"
		"out vec2 Texcoord;"
		"void main() {"
		"   Color = color;"
		"	Texcoord = texcoord;"
		"   gl_Position = vec4(position, 0.0, 1.0);"
		"}";
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		printf("%s\n", buffer);
	}

	const char *fragmentSource =
		"#version 150 core\n"
		"in vec3 Color;"
		"in vec2 Texcoord;"
		"out vec4 outColor;"
		"uniform sampler2D tex;"
		"void main()"
		"{"
		"	outColor = texture(tex, Texcoord) * vec4(Color, 1.0);"
		"}";
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		printf("%s\n", buffer);
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
	                       7*sizeof(float), (void*)(2*sizeof(float)));

	GLint texcoordAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texcoordAttrib);
	glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE,
	                       7*sizeof(float), (void*)(5*sizeof(float)));

	while (1) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);
	}

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	//glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}
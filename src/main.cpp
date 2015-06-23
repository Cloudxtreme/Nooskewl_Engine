#include "starsquatters.h"

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

	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(1);

	glewExperimental = 1;
	glewInit();

	Image *img = new Image();
	SDL_RWops *file = SDL_RWFromFile("test.tga", "rb");
	img->load_tga(file);
	SDL_RWclose(file);

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
		"in vec4 Color;"
		"in vec2 Texcoord;"
		"out vec4 outColor;"
		"uniform sampler2D tex;"
		"void main()"
		"{"
		"	outColor = texture(tex, Texcoord) * Color;"
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
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(3*sizeof(float)));

	GLint texcoordAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texcoordAttrib);
	glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 9*sizeof(float), (void*)(7*sizeof(float)));

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	glm::mat4 ortho = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
	GLint uniTrans = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(ortho));

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (1) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		img->bind();
		img->draw_region(0, 0, 16, 16, 10, 10);
		img->draw_region(16, 0, 16, 16, 50, 50);
		img->draw(100, 100);

		SDL_GL_SwapWindow(window);
	}

	delete img;

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}
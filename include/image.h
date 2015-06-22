#ifndef IMAGE_H
#define IMAGE_H

#include <string>

#include <windows.h>
#include <GL/gl.h>

#include <SDL_rwops.h>

class Image {
public:
	GLuint texture;
	int width;
	int height;

	Image();
	~Image();

	// Supports: TGA 16/24/32 bit
	bool load_tga(SDL_RWops *file);

	void draw(float x, float y, float z = 0.0f);

private:
	GLuint vao;
	GLuint vbo;
	float vertices[9*6]; // 6 * x, y, z, r, g, b, a, u, v
};

#endif IMAGE_H
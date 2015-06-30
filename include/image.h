#ifndef IMAGE_H
#define IMAGE_H

#include <string>

#include "starsquatters.h"

class Image {
public:
	enum Flags {
		FLIP_H = 1,
		FLIP_V = 2
	};

	GLuint texture;
	int w;
	int h;

	Image();
	~Image();

	// Supports: TGA 16/24/32 bit
	bool load_tga(SDL_RWops *file);
	bool from_surface(SDL_Surface *surface);

	void bind();
	void draw_region(float sx, float sy, float sw, float sh, float dx, float dy, int flags);
	void draw(float dx, float dy, int flags);

private:
	bool upload(unsigned char *data);

	GLuint vao;
	GLuint vbo;
	float vertices[9*6]; // 6 * x, y, z, r, g, b, a, u, v
};

#endif IMAGE_H
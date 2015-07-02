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

	std::string filename;
	GLuint texture;
	int w;
	int h;

	Image();
	~Image();

	bool load_tga(std::string filename);
	bool from_surface(SDL_Surface *surface);

	void start();
	void draw_region(float sx, float sy, float sw, float sh, float dx, float dy, int flags);
	void draw(float dx, float dy, int flags);
	void end(); // call after every group of draws

private:
	bool upload(unsigned char *data);

	GLuint vao;
	GLuint vbo;
};

#endif IMAGE_H
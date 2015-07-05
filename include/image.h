#ifndef IMAGE_H
#define IMAGE_H

#include <string>

#include "starsquatters.h"
#include "graphics.h"
#include "types.h"

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
	void stretch_region(float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags = 0);
	void draw_region(float sx, float sy, float sw, float sh, float dx, float dy, int flags = 0);
	void draw(float dx, float dy, int flags = 0);
	void draw(Point<int> position, int flags = 0);
	void end(); // call after every group of draws

	// These ones call start/end automatically
	void stretch_region_single(float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, int flags = 0);
	void draw_region_single(float sx, float sy, float sw, float sh, float dx, float dy, int flags = 0);
	void draw_single(float dx, float dy, int flags = 0);
	void draw_single(Point<int> position, int flags = 0);

private:
	bool upload(unsigned char *data);

	GLuint vao;
	GLuint vbo;
};

#endif IMAGE_H
#ifndef IMAGE_H
#define IMAGE_H

#include <string>

#include "starsquatters.h"
#include "error.h"
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

	Image(std::string filename, bool is_absolute_path) throw (Error);
	Image(std::string filename) throw (Error);
	Image(SDL_Surface *surface) throw (Error);
	~Image();

	void start();
	void stretch_region(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, int flags = 0);
	void draw_region(Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags = 0);
	void draw(Point<int> dest_position, int flags = 0);
	void end(); // call after every group of draws

	// These ones call start/end automatically
	void stretch_region_single(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, int flags = 0);
	void draw_region_single(Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags = 0);
	void draw_single(Point<int> dest_position, int flags = 0);

private:
	void upload(unsigned char *data) throw (Error);

	GLuint vao;
	GLuint vbo;
};

#endif IMAGE_H
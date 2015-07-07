#ifndef VERTEX_ACCEL_H
#define VERTEX_ACCEL_H

#include "starsquatters.h"
#include "types.h"

class Image;

class Vertex_Accel {
public:
	Vertex_Accel();
	~Vertex_Accel();

	void init();

	void start(); // no texture
	void start(Image *image);
	void buffer(Point<int> source_position, Size<int> source_size, Point<float> da, Point<float> db, Point<float> dc, Point<float> dd, SDL_Colour vertex_colours[4], int flags);
	void buffer(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4] /* tl, tr, br, bl */, int flags);
	void end();
	void maybe_resize_buffer(int increase);

	void set_perspective_drawing(bool perspective_drawing);

private:
	float *vertices;
	int count;
	int total;
	Image *image;
	bool perspective_drawing;
};

extern Vertex_Accel *vertex_accel;

#endif // VERTEX_ACCEL_H
#ifndef VERTEX_ACCEL_H
#define VERTEX_ACCEL_H

#include "starsquatters.h"

class Image;

class Vertex_Accel {
public:
	Vertex_Accel();
	~Vertex_Accel();

	void init();

	void start(); // no texture
	void start(Image *image);
	void buffer(float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, SDL_Colour vertex_colours[4] /* tl, tr, br, bl */, int flags);
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
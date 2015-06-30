#ifndef VERTEX_ACCEL_H
#define VERTEX_ACCEL_H

#include "starsquatters.h"

class Image;

class Vertex_Accel {
public:
	Vertex_Accel();
	~Vertex_Accel();

	bool init();

	void start(Image *image);
	bool buffer(float sx, float sy, float sw, float sh, float dx, float dy, int flags);
	void end();
	bool maybe_resize_buffer(int increase);

private:
	float *vertices;
	int count;
	int total;
	Image *image;
};

extern Vertex_Accel *vertex_accel;

#endif // VERTEX_ACCEL_H
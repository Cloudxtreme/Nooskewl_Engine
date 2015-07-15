#ifndef VERTEX_ACCEL_H
#define VERTEX_ACCEL_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/types.h"

namespace Nooskewl_Engine {

class Image;

class NOOSKEWL_ENGINE_EXPORT Vertex_Cache {
public:
	Vertex_Cache();
	~Vertex_Cache();

	void init();

	void start(bool repeat = false); // no texture
	void start(Image *image, bool repeat = false);
	void end();

	void enable_perspective_drawing(Size<int> screen_size);
	void disable_perspective_drawing();

	void cache(SDL_Colour vertex_colours[4], Point<float> source_position, Size<float> source_size, Point<float> da, Point<float> db, Point<float> dc, Point<float> dd, int flags);
	void cache_z(SDL_Colour vertex_colours[4], Point<float> source_position, Size<float> source_size, Point<float> dest_position, float z, Size<float> dest_size, int flags);
	void cache(SDL_Colour vertex_colours[4], Point<float> source_position, Size<float> source_size, Point<float> dest_position, Size<float> dest_size, int flags);

private:
	void maybe_resize_cache(int increase);

	float *vertices;
	int count;
	int total;
	Image *image;
	bool perspective_drawing;
	bool repeat;

#ifdef NOOSKEWL_ENGINE_WINDOWS
	unsigned int required_passes;
#endif

	Size<int> screen_size;
};

} // End namespace Nooskewl_Engine

#endif // VERTEX_ACCEL_H

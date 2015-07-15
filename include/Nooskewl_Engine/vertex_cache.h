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
	void maybe_resize_buffer(int increase);

	void enable_perspective_drawing(int screen_w, int screen_h);
	void disable_perspective_drawing();

private:
	float *vertices;
	int count;
	int total;
	Image *image;
	bool perspective_drawing;
	bool repeat;

#ifdef NOOSKEWL_ENGINE_WINDOWS
	unsigned int required_passes;
#endif

	int screen_w, screen_h;

public:
	/* Template functions */

	template<typename T> void buffer(SDL_Colour vertex_colours[4], Point<T> source_position, Size<T> source_size, Point<T> da, Point<T> db, Point<T> dc, Point<T> dd, int flags)
	{
		maybe_resize_buffer(256);

		// Set vertex x, y
		vertices[9*(count+0)+0] = (float)da.x;
		vertices[9*(count+0)+1] = (float)da.y;
		vertices[9*(count+1)+0] = (float)db.x;
		vertices[9*(count+1)+1] = (float)db.y;
		vertices[9*(count+2)+0] = (float)dc.x;
		vertices[9*(count+2)+1] = (float)dc.y;
		vertices[9*(count+3)+0] = (float)da.x;
		vertices[9*(count+3)+1] = (float)da.y;
		vertices[9*(count+4)+0] = (float)dc.x;
		vertices[9*(count+4)+1] = (float)dc.y;
		vertices[9*(count+5)+0] = (float)dd.x;
		vertices[9*(count+5)+1] = (float)dd.y;

		for (int i = 0; i < 6; i++) {
			vertices[9*(count+i)+2] = 0.0f; // set vertex z
		}

		if (image) {
			float sx = (float)source_position.x;
			float sy = (float)source_position.y;
			float tu = sx / (float)image->w;
			float tv = sy / (float)image->h;
			float tu2 = tu + (float)source_size.w / (float)image->w;
			float tv2 = tv + (float)source_size.h / (float)image->h;

			tv = 1.0f - tv;
			tv2 = 1.0f - tv2;

			if (flags & Image::FLIP_H) {
				float tmp = tu;
				tu = tu2;
				tu2 = tmp;
			}
			if (flags & Image::FLIP_V) {
				float tmp = tv;
				tv = tv2;
				tv2 = tmp;
			}

			// texture coordinates
			vertices[9*(count+0)+3] = tu;
			vertices[9*(count+0)+4] = tv;
			vertices[9*(count+1)+3] = tu2;
			vertices[9*(count+1)+4] = tv;
			vertices[9*(count+2)+3] = tu2;
			vertices[9*(count+2)+4] = tv2;
			vertices[9*(count+3)+3] = tu;
			vertices[9*(count+3)+4] = tv;
			vertices[9*(count+4)+3] = tu2;
			vertices[9*(count+4)+4] = tv2;
			vertices[9*(count+5)+3] = tu;
			vertices[9*(count+5)+4] = tv2;
		}

		vertices[9*(count+0)+5+0] = (float)vertex_colours[0].r / 255.0f;
		vertices[9*(count+0)+5+1] = (float)vertex_colours[0].g / 255.0f;
		vertices[9*(count+0)+5+2] = (float)vertex_colours[0].b / 255.0f;
		vertices[9*(count+0)+5+3] = (float)vertex_colours[0].a / 255.0f;

		vertices[9*(count+1)+5+0] = (float)vertex_colours[1].r / 255.0f;
		vertices[9*(count+1)+5+1] = (float)vertex_colours[1].g / 255.0f;
		vertices[9*(count+1)+5+2] = (float)vertex_colours[1].b / 255.0f;
		vertices[9*(count+1)+5+3] = (float)vertex_colours[1].a / 255.0f;

		vertices[9*(count+2)+5+0] = (float)vertex_colours[2].r / 255.0f;
		vertices[9*(count+2)+5+1] = (float)vertex_colours[2].g / 255.0f;
		vertices[9*(count+2)+5+2] = (float)vertex_colours[2].b / 255.0f;
		vertices[9*(count+2)+5+3] = (float)vertex_colours[2].a / 255.0f;

		vertices[9*(count+3)+5+0] = (float)vertex_colours[0].r / 255.0f;
		vertices[9*(count+3)+5+1] = (float)vertex_colours[0].g / 255.0f;
		vertices[9*(count+3)+5+2] = (float)vertex_colours[0].b / 255.0f;
		vertices[9*(count+3)+5+3] = (float)vertex_colours[0].a / 255.0f;

		vertices[9*(count+4)+5+0] = (float)vertex_colours[2].r / 255.0f;
		vertices[9*(count+4)+5+1] = (float)vertex_colours[2].g / 255.0f;
		vertices[9*(count+4)+5+2] = (float)vertex_colours[2].b / 255.0f;
		vertices[9*(count+4)+5+3] = (float)vertex_colours[2].a / 255.0f;

		vertices[9*(count+5)+5+0] = (float)vertex_colours[3].r / 255.0f;
		vertices[9*(count+5)+5+1] = (float)vertex_colours[3].g / 255.0f;
		vertices[9*(count+5)+5+2] = (float)vertex_colours[3].b / 255.0f;
		vertices[9*(count+5)+5+3] = (float)vertex_colours[3].a / 255.0f;

		count += 6;
	}

	template<typename T> void buffer_z(SDL_Colour vertex_colours[4], Point<T> source_position, Size<T> source_size, Point<T> dest_position, float z, Size<T> dest_size, int flags)
	{
		maybe_resize_buffer(256);

		float dx = (float)dest_position.x;
		float dy = (float)dest_position.y;
		float dx2 = dx + (float)dest_size.w;
		float dy2 = dy + (float)dest_size.h;

		if (perspective_drawing) {
			dx /= (float)screen_w;
			dy /= (float)screen_h;
			dx2 /= (float)screen_w;
			dy2 /= (float)screen_h;
			dx -= 0.5f;
			dy -= 0.5f;
			dx2 -= 0.5f;
			dy2 -= 0.5f;
			dx *= 6.0f;
			dy *= 6.0f;
			dx2 *= 6.0f;
			dy2 *= 6.0f;
		}

		// Set vertex x, y
		vertices[9*(count+0)+0] = dx;
		vertices[9*(count+0)+1] = dy;
		vertices[9*(count+1)+0] = dx2;
		vertices[9*(count+1)+1] = dy;
		vertices[9*(count+2)+0] = dx2;
		vertices[9*(count+2)+1] = dy2;
		vertices[9*(count+3)+0] = dx;
		vertices[9*(count+3)+1] = dy;
		vertices[9*(count+4)+0] = dx2;
		vertices[9*(count+4)+1] = dy2;
		vertices[9*(count+5)+0] = dx;
		vertices[9*(count+5)+1] = dy2;

		for (int i = 0; i < 6; i++) {
			vertices[9*(count+i)+2] = z; // set vertex z
		}

		if (image) {
			float tu, tv, tu2, tv2;

			if (repeat) {
				tu = (float)source_position.x / image->w;
				tv = (float)source_position.y / image->h;
				tu2 = tu + (float)dest_size.w / source_size.w;
				tv2 = tv + (float)dest_size.h / source_size.w;
			}
			else {
				float sx = (float)source_position.x;
				float sy = (float)source_position.y;
				tu = sx / (float)image->w;
				tv = sy / (float)image->h;
				tu2 = tu + (float)source_size.w / (float)image->w;
				tv2 = tv + (float)source_size.h / (float)image->h;

				tv = 1.0f - tv;
				tv2 = 1.0f - tv2;

				if (flags & Image::FLIP_H) {
					float tmp = tu;
					tu = tu2;
					tu2 = tmp;
				}
				if (flags & Image::FLIP_V) {
					float tmp = tv;
					tv = tv2;
					tv2 = tmp;
				}
			}

			// texture coordinates
			vertices[9*(count+0)+3] = tu;
			vertices[9*(count+0)+4] = tv;
			vertices[9*(count+1)+3] = tu2;
			vertices[9*(count+1)+4] = tv;
			vertices[9*(count+2)+3] = tu2;
			vertices[9*(count+2)+4] = tv2;
			vertices[9*(count+3)+3] = tu;
			vertices[9*(count+3)+4] = tv;
			vertices[9*(count+4)+3] = tu2;
			vertices[9*(count+4)+4] = tv2;
			vertices[9*(count+5)+3] = tu;
			vertices[9*(count+5)+4] = tv2;
		}

		vertices[9*(count+0)+5+0] = (float)vertex_colours[0].r / 255.0f;
		vertices[9*(count+0)+5+1] = (float)vertex_colours[0].g / 255.0f;
		vertices[9*(count+0)+5+2] = (float)vertex_colours[0].b / 255.0f;
		vertices[9*(count+0)+5+3] = (float)vertex_colours[0].a / 255.0f;

		vertices[9*(count+1)+5+0] = (float)vertex_colours[1].r / 255.0f;
		vertices[9*(count+1)+5+1] = (float)vertex_colours[1].g / 255.0f;
		vertices[9*(count+1)+5+2] = (float)vertex_colours[1].b / 255.0f;
		vertices[9*(count+1)+5+3] = (float)vertex_colours[1].a / 255.0f;

		vertices[9*(count+2)+5+0] = (float)vertex_colours[2].r / 255.0f;
		vertices[9*(count+2)+5+1] = (float)vertex_colours[2].g / 255.0f;
		vertices[9*(count+2)+5+2] = (float)vertex_colours[2].b / 255.0f;
		vertices[9*(count+2)+5+3] = (float)vertex_colours[2].a / 255.0f;

		vertices[9*(count+3)+5+0] = (float)vertex_colours[0].r / 255.0f;
		vertices[9*(count+3)+5+1] = (float)vertex_colours[0].g / 255.0f;
		vertices[9*(count+3)+5+2] = (float)vertex_colours[0].b / 255.0f;
		vertices[9*(count+3)+5+3] = (float)vertex_colours[0].a / 255.0f;

		vertices[9*(count+4)+5+0] = (float)vertex_colours[2].r / 255.0f;
		vertices[9*(count+4)+5+1] = (float)vertex_colours[2].g / 255.0f;
		vertices[9*(count+4)+5+2] = (float)vertex_colours[2].b / 255.0f;
		vertices[9*(count+4)+5+3] = (float)vertex_colours[2].a / 255.0f;

		vertices[9*(count+5)+5+0] = (float)vertex_colours[3].r / 255.0f;
		vertices[9*(count+5)+5+1] = (float)vertex_colours[3].g / 255.0f;
		vertices[9*(count+5)+5+2] = (float)vertex_colours[3].b / 255.0f;
		vertices[9*(count+5)+5+3] = (float)vertex_colours[3].a / 255.0f;

		count += 6;
	}

	template<typename T> void buffer(SDL_Colour vertex_colours[4], Point<T> source_position, Size<T> source_size, Point<T> dest_position, Size<T> dest_size, int flags)
	{
		buffer_z<T>(vertex_colours, source_position, source_size, dest_position, 0.0f, dest_size, flags);
	}
};

} // End namespace Nooskewl_Engine

#endif // VERTEX_ACCEL_H

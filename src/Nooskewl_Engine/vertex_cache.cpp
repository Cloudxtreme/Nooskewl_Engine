#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/vertex_cache.h"

/* So textures don't bleed into each other when tiling. This is about 1/100th of a pixel on a 1024x1024 texture */
#define SMALL_TEXTURE_OFFSET 0.00001f
#define CLAMP(v) if (v < 0.0f) v = 0.0f; else if (v > 1.0f) v = 1.0f;

using namespace Nooskewl_Engine;

Vertex_Cache::Vertex_Cache() :
	vertices(0),
	count(0),
	total(0),
	perspective_drawing(false),
	repeat(false),
	font_scaling(false)
{
}

Vertex_Cache::~Vertex_Cache()
{
	free(vertices);
}

void Vertex_Cache::init()
{
	maybe_resize_cache(256);
}

void Vertex_Cache::start(bool repeat)
{
	image = 0;
	this->repeat = repeat;
	noo.current_shader->set_bool("use_tex", false);
	noo.current_shader->set_texture("tex", 0);
}

void Vertex_Cache::start(Image *image, bool repeat)
{
	this->image = image;
	this->repeat = repeat;
	noo.current_shader->set_bool("use_tex", true);
	noo.current_shader->set_texture("tex", image);
}

void Vertex_Cache::end()
{
	if (noo.opengl) {
		GLuint opengl_shader = noo.current_shader->get_opengl_shader();

		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9*count, vertices, GL_DYNAMIC_DRAW);
		printGLerror("glBufferData");
	
		GLint posAttrib = glGetAttribLocation(opengl_shader, "in_position");
		printGLerror("glGetAttribLocation (in_position)");
		if (posAttrib != -1) {
			glEnableVertexAttribArray(posAttrib);
			printGLerror("glEnableVertexAttribArray (in_position)");
			glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
			printGLerror("glVertexAttribPointer (in_position)");
		}

		GLint texcoordAttrib = glGetAttribLocation(opengl_shader, "in_texcoord");
		printGLerror("glGetAttribLocation (in_texcoord)");
		if (texcoordAttrib != -1) {
			glEnableVertexAttribArray(texcoordAttrib);
			printGLerror("glEnableVertexAttribArray (in_texcoord)");
			glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
			printGLerror("glVertexAttribPointer (in_texcoord)");
		}

		GLint colAttrib = glGetAttribLocation(opengl_shader, "in_colour");
		printGLerror("glGetAttribLocation (in_colour)");
		if (colAttrib != -1) {
			glEnableVertexAttribArray(colAttrib);
			printGLerror("glEnableVertexAttribArray (in_colour)");
			glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
			printGLerror("glVertexAttribPointer (in_colour)");
		}

		glDrawArrays(GL_TRIANGLES, 0, count);
		printGLerror("glDrawArrays");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		if (noo.d3d_lost) {
			return;
		}
		LPD3DXEFFECT d3d_effect = noo.current_shader->get_d3d_effect();
		unsigned int required_passes;
		d3d_effect->Begin(&required_passes, 0);
		for (unsigned int i = 0; i < required_passes; i++) {
			d3d_effect->BeginPass(i);
			if (noo.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, (void *)vertices, 9*sizeof(float)) != D3D_OK) {
				infomsg("DrawPrimitiveUP failed\n");
				return;
			}
			d3d_effect->EndPass();
		}
		d3d_effect->End();
	}
#endif

	count = 0;
}

void Vertex_Cache::enable_perspective_drawing(Size<int> screen_size)
{
	perspective_drawing = true;
	this->screen_size = screen_size;
}

void Vertex_Cache::disable_perspective_drawing()
{
	perspective_drawing = false;
}

void Vertex_Cache::enable_font_scaling(bool enable)
{
	font_scaling = enable;
}

void Vertex_Cache::cache(SDL_Colour vertex_colours[3], Point<float> da, Point<float> db, Point<float> dc)
{
	maybe_resize_cache(256);

	float scale;

	if (font_scaling) {
		scale = noo.font_scale;
	}
	else {
		scale = noo.scale;
	}

	// Set vertex x, y
	vertices[9*(count+0)+0] = (float)da.x * scale;
	vertices[9*(count+0)+1] = (float)da.y * scale;
	vertices[9*(count+1)+0] = (float)db.x * scale;
	vertices[9*(count+1)+1] = (float)db.y * scale;
	vertices[9*(count+2)+0] = (float)dc.x * scale;
	vertices[9*(count+2)+1] = (float)dc.y * scale;

	for (int i = 0; i < 3; i++) {
		vertices[9*(count+i)+2] = 0.0f; // set vertex z
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

	count += 3;
}

void Vertex_Cache::cache(SDL_Colour vertex_colours[4], Point<float> source_position, Size<float> source_size, Point<float> da, Point<float> db, Point<float> dc, Point<float> dd, int flags)
{
	maybe_resize_cache(256);

	float scale;

	if (font_scaling) {
		scale = noo.font_scale;
	}
	else {
		scale = noo.scale;
	}

	// Set vertex x, y
	vertices[9*(count+0)+0] = (float)da.x * scale;
	vertices[9*(count+0)+1] = (float)da.y * scale;
	vertices[9*(count+1)+0] = (float)db.x * scale;
	vertices[9*(count+1)+1] = (float)db.y * scale;
	vertices[9*(count+2)+0] = (float)dc.x * scale;
	vertices[9*(count+2)+1] = (float)dc.y * scale;
	vertices[9*(count+3)+0] = (float)da.x * scale;
	vertices[9*(count+3)+1] = (float)da.y * scale;
	vertices[9*(count+4)+0] = (float)dc.x * scale;
	vertices[9*(count+4)+1] = (float)dc.y * scale;
	vertices[9*(count+5)+0] = (float)dd.x * scale;
	vertices[9*(count+5)+1] = (float)dd.y * scale;

	for (int i = 0; i < 6; i++) {
		vertices[9*(count+i)+2] = 0.0f; // set vertex z
	}

	if (image) {
		float sx = (float)source_position.x;
		float sy = (float)source_position.y;
		float tu = sx / (float)image->size.w + SMALL_TEXTURE_OFFSET;
		float tv = sy / (float)image->size.h + SMALL_TEXTURE_OFFSET;
		float tu2 = float(source_position.x + source_size.w) / image->size.w - SMALL_TEXTURE_OFFSET;
		float tv2 = float(source_position.y + source_size.h) / image->size.h - SMALL_TEXTURE_OFFSET;

		CLAMP(tu)
		CLAMP(tv)
		CLAMP(tu2)
		CLAMP(tv2)

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

void Vertex_Cache::cache_z(SDL_Colour vertex_colours[4], Point<float> source_position, Size<float> source_size, Point<float> dest_position, float z, Size<float> dest_size, int flags)
{
	maybe_resize_cache(256);

	float dx = (float)dest_position.x;
	float dy = (float)dest_position.y;
	float dx2 = dx + (float)dest_size.w;
	float dy2 = dy + (float)dest_size.h;

	if (perspective_drawing) {
		dx /= (float)screen_size.w;
		dy /= (float)screen_size.h;
		dx2 /= (float)screen_size.w;
		dy2 /= (float)screen_size.h;
		dx -= 0.5f;
		dy -= 0.5f;
		dx2 -= 0.5f;
		dy2 -= 0.5f;
		dx *= 6.0f;
		dy *= 6.0f;
		dx2 *= 6.0f;
		dy2 *= 6.0f;
	}
	else {
		float scale;

		if (font_scaling) {
			scale = noo.font_scale;
		}
		else {
			scale = noo.scale;
		}

		dx *= scale;
		dy *= scale;
		dx2 *= scale;
		dy2 *= scale;
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
			tu = (float)source_position.x / image->size.w + SMALL_TEXTURE_OFFSET;
			tv = (float)source_position.y / image->size.h + SMALL_TEXTURE_OFFSET;
			tu2 = (float)dest_size.w / source_size.w - SMALL_TEXTURE_OFFSET;
			tv2 = (float)dest_size.h / source_size.h - SMALL_TEXTURE_OFFSET;

			CLAMP(tu)
			CLAMP(tv)
			CLAMP(tu2)
			CLAMP(tv2)
		}
		else {
			float sx = (float)source_position.x + SMALL_TEXTURE_OFFSET;
			float sy = (float)source_position.y + SMALL_TEXTURE_OFFSET;
			tu = sx / (float)image->size.w;
			tv = sy / (float)image->size.h;
			tu2 = (source_position.x + source_size.w - SMALL_TEXTURE_OFFSET) / image->size.w;
			tv2 = (source_position.y + source_size.h - SMALL_TEXTURE_OFFSET) / image->size.h;

			CLAMP(tu)
			CLAMP(tv)
			CLAMP(tu2)
			CLAMP(tv2)

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

void Vertex_Cache::cache(SDL_Colour vertex_colours[4], Point<float> source_position, Size<float> source_size, Point<float> dest_position, Size<float> dest_size, int flags)
{
	cache_z(vertex_colours, source_position, source_size, dest_position, 0.0f, dest_size, flags);
}

void Vertex_Cache::maybe_resize_cache(int increase)
{
	if (total - count >= increase) {
		return;
	}

	if (vertices == 0) {
		vertices = (float *)malloc(sizeof(float)*9*increase);
	}
	else {
		vertices = (float *)realloc(vertices, sizeof(float)*9*(total+increase));
	}
	if (vertices == 0) {
		throw MemoryError("out of memory in maybe_resize_cache");
	}

	total += increase;
}

#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/vertex_cache.h"

using namespace Nooskewl_Engine;

Vertex_Cache::Vertex_Cache() :
	vertices(0),
	count(0),
	total(0),
	perspective_drawing(false)
{
}

Vertex_Cache::~Vertex_Cache()
{
	free(vertices);
}

void Vertex_Cache::init()
{
	maybe_resize_buffer(256);
}

void Vertex_Cache::start()
{
	this->image = 0;

#ifdef NOOSKEWL_ENGINE_WINDOWS
	if (noo.opengl == false) {
		m.d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m.d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m.d3d_device->SetFVF(FVF);
		m.effect->SetBool("use_tex", false);
		m.effect->Begin(&required_passes, 0);
		m.d3d_device->SetTexture(0, NULL);
	}
#endif
}

void Vertex_Cache::start(Image *image)
{
	this->image = image;

	if (noo.opengl) {
		glBindTexture(GL_TEXTURE_2D, image->internal->texture);
		printGLerror("glBindTexture");
		glBindVertexArray(image->internal->vao);
		printGLerror("glBindVertexArray");
		glBindBuffer(GL_ARRAY_BUFFER, image->internal->vbo);
		printGLerror("glBindBuffer");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		m.d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m.d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m.d3d_device->SetFVF(FVF);
		m.effect->SetBool("use_tex", true);
		m.effect->SetTexture("tex", image->internal->video_texture);
		m.effect->Begin(&required_passes, 0);
		m.d3d_device->SetTexture(0, image->internal->video_texture);
	}
#endif
}

void Vertex_Cache::end()
{
	if (noo.opengl) {
		GLint use_tex = glGetUniformLocation(m.current_shader, "use_tex");
		printGLerror("glGetUniformLocation");
		if (image) {
			glUniform1i(use_tex, true);
		}
		else {
			glUniform1i(use_tex, false);
		}
		printGLerror("glUniform1i");

		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9*count, vertices, GL_DYNAMIC_DRAW);
		printGLerror("glBufferData");
	
		GLint posAttrib = glGetAttribLocation(m.current_shader, "in_position");
		printGLerror("glGetAttribLocation");
		glEnableVertexAttribArray(posAttrib);
		printGLerror("glEnableVertexAttribArray");
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
		printGLerror("glVertexAttribPointer");

		GLint texcoordAttrib = glGetAttribLocation(m.current_shader, "in_texcoord");
		printGLerror("glGetAttribLocation");
		glEnableVertexAttribArray(texcoordAttrib);
		printGLerror("glEnableVertexAttribArray");
		glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		printGLerror("glVertexAttribPointer");

		GLint colAttrib = glGetAttribLocation(m.current_shader, "in_colour");
		printGLerror("glGetAttribLocation");
		glEnableVertexAttribArray(colAttrib);
		printGLerror("glEnableVertexAttribArray");
		glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
		printGLerror("glVertexAttribPointer");

		glDrawArrays(GL_TRIANGLES, 0, count);
		printGLerror("glDrawArrays");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		for (unsigned int i = 0; i < required_passes; i++) {
			m.effect->BeginPass(i);
			if (m.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, (void *)vertices, 9*sizeof(float)) != D3D_OK) {
				infomsg("DrawPrimitiveUP failed\n");
				return;
			}
			m.effect->EndPass();
		}
		m.effect->End();
	}
#endif

	count = 0;
}

void Vertex_Cache::buffer(Point<int> source_position, Size<int> source_size, Point<float> da, Point<float> db, Point<float> dc, Point<float> dd, SDL_Colour vertex_colours[4], int flags)
{
	maybe_resize_buffer(256);

	// Set vertex x, y
	vertices[9*(count+0)+0] = da.x;
	vertices[9*(count+0)+1] = da.y;
	vertices[9*(count+1)+0] = db.x;
	vertices[9*(count+1)+1] = db.y;
	vertices[9*(count+2)+0] = dc.x;
	vertices[9*(count+2)+1] = dc.y;
	vertices[9*(count+3)+0] = da.x;
	vertices[9*(count+3)+1] = da.y;
	vertices[9*(count+4)+0] = dc.x;
	vertices[9*(count+4)+1] = dc.y;
	vertices[9*(count+5)+0] = dd.x;
	vertices[9*(count+5)+1] = dd.y;

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

void Vertex_Cache::buffer_z(Point<int> source_position, Size<int> source_size, Point<int> dest_position, float z, Size<int> dest_size, SDL_Colour vertex_colours[4], int flags)
{
	maybe_resize_buffer(256);

	float dx = (float)dest_position.x;
	float dy = (float)dest_position.y;
	float dx2 = dx + (float)dest_size.w;
	float dy2 = dy + (float)dest_size.h;

	if (perspective_drawing) {
		dx /= (float)noo.screen_w;
		dy /= (float)noo.screen_h;
		dx2 /= (float)noo.screen_w;
		dy2 /= (float)noo.screen_h;
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

void Vertex_Cache::buffer(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4], int flags)
{
	buffer_z(source_position, source_size, dest_position, 0.0f, dest_size, vertex_colours, flags);
}

void Vertex_Cache::set_perspective_drawing(bool perspective_drawing)
{
	this->perspective_drawing = perspective_drawing;
}


void Vertex_Cache::maybe_resize_buffer(int increase)
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
		throw MemoryError("out of memory in maybe_resize_buffer");
	}

	total += increase;
}

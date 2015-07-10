#include "Nooskewl_Engine/global.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/log.h"
#include "Nooskewl_Engine/module.h"
#include "Nooskewl_Engine/vertex_accel.h"
#include "Nooskewl_Engine/video.h"

// FIXME:
extern GLuint current_shader;

Vertex_Accel::Vertex_Accel() :
	vertices(NULL),
	count(0),
	total(0),
	perspective_drawing(false)
{
}

Vertex_Accel::~Vertex_Accel()
{
	free(vertices);
}

void Vertex_Accel::init()
{
	maybe_resize_buffer(256);
}

void Vertex_Accel::init_new_texture()
{
	if (g.graphics.opengl) {
		// Specify the layout of the vertex data
		GLint posAttrib = glGetAttribLocation(current_shader, "in_position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);

		GLint texcoordAttrib = glGetAttribLocation(current_shader, "in_texcoord");
		glEnableVertexAttribArray(texcoordAttrib);
		glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

		GLint colAttrib = glGetAttribLocation(current_shader, "in_colour");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
	}
}

void Vertex_Accel::start()
{
	this->image = NULL;

#ifdef _MSC_VER
	if (g.graphics.opengl == false) {
		m.d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m.d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m.d3d_device->SetFVF(FVF);
		m.effect->Begin(&required_passes, 0);
	}
#endif
}

void Vertex_Accel::start(Image *image)
{
	this->image = image;

#ifdef _MSC_VER
	if (g.graphics.opengl == false) {
		m.d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		m.d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		m.d3d_device->SetFVF(FVF);
		m.effect->SetBool("use_tex", true);
		m.effect->SetTexture("tex", image->video_texture);
		m.effect->Begin(&required_passes, 0);
		m.d3d_device->SetTexture(0, image->video_texture);
	}
#endif
}

void Vertex_Accel::end()
{
	if (g.graphics.opengl) {
		GLint use_tex = glGetUniformLocation(current_shader, "use_tex");
		if (image) {
			glUniform1i(use_tex, true);
		}
		else {
			glUniform1i(use_tex, false);
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9*count, vertices, GL_DYNAMIC_DRAW);

		glDrawArrays(GL_TRIANGLES, 0, count);
	}
#ifdef _MSC_VER
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

void Vertex_Accel::buffer(Point<int> source_position, Size<int> source_size, Point<float> da, Point<float> db, Point<float> dc, Point<float> dd, SDL_Colour vertex_colours[4], int flags)
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
		vertices[9*(count+i)+2] = -5.0f; // set vertex z
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

void Vertex_Accel::buffer(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, SDL_Colour vertex_colours[4], int flags)
{
	maybe_resize_buffer(256);

	float dx = (float)dest_position.x;
	float dy = (float)dest_position.y;
	float dx2 = dx + (float)dest_size.w;
	float dy2 = dy + (float)dest_size.h;

	if (perspective_drawing) {
		dx /= (float)g.graphics.screen_w;
		dy /= (float)g.graphics.screen_h;
		dx2 /= (float)g.graphics.screen_w;
		dy2 /= (float)g.graphics.screen_h;
		dx -= 0.5f;
		dy -= 0.5f;
		dx2 -= 0.5f;
		dy2 -= 0.5f;
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

void Vertex_Accel::set_perspective_drawing(bool perspective_drawing)
{
	this->perspective_drawing = perspective_drawing;
}


void Vertex_Accel::maybe_resize_buffer(int increase)
{
	if (total - count >= increase) {
		return;
	}

	if (vertices == NULL) {
		vertices = (float *)malloc(sizeof(float)*9*increase);
	}
	else {
		vertices = (float *)realloc(vertices, sizeof(float)*9*(total+increase));
	}
	if (vertices == NULL) {
		throw MemoryError("out of memory in maybe_resize_buffer");
	}

	total += increase;
}

#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/vertex_accel.h"
#include "Nooskewl_Engine/video.h"

// FIXME:
extern GLuint current_shader;

Vertex_Accel *vertex_accel;

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
	if (opengl) {
		// Specify the layout of the vertex data
		GLint posAttrib = glGetAttribLocation(current_shader, "in_position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);

		GLint colAttrib = glGetAttribLocation(current_shader, "in_color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

		GLint texcoordAttrib = glGetAttribLocation(current_shader, "in_texcoord");
		glEnableVertexAttribArray(texcoordAttrib);
		glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
	}
}

void Vertex_Accel::start()
{
	this->image = NULL;
}

void Vertex_Accel::start(Image *image)
{
	this->image = image;
}

void Vertex_Accel::end()
{
	if (opengl) {
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
		vertices[9*(count+0)+7] = tu;
		vertices[9*(count+0)+8] = tv;
		vertices[9*(count+1)+7] = tu2;
		vertices[9*(count+1)+8] = tv;
		vertices[9*(count+2)+7] = tu2;
		vertices[9*(count+2)+8] = tv2;
		vertices[9*(count+3)+7] = tu;
		vertices[9*(count+3)+8] = tv;
		vertices[9*(count+4)+7] = tu2;
		vertices[9*(count+4)+8] = tv2;
		vertices[9*(count+5)+7] = tu;
		vertices[9*(count+5)+8] = tv2;
	}

	vertices[9*(count+0)+3+0] = (float)vertex_colours[0].r / 255.0f;
	vertices[9*(count+0)+3+1] = (float)vertex_colours[0].g / 255.0f;
	vertices[9*(count+0)+3+2] = (float)vertex_colours[0].b / 255.0f;
	vertices[9*(count+0)+3+3] = (float)vertex_colours[0].a / 255.0f;

	vertices[9*(count+1)+3+0] = (float)vertex_colours[1].r / 255.0f;
	vertices[9*(count+1)+3+1] = (float)vertex_colours[1].g / 255.0f;
	vertices[9*(count+1)+3+2] = (float)vertex_colours[1].b / 255.0f;
	vertices[9*(count+1)+3+3] = (float)vertex_colours[1].a / 255.0f;

	vertices[9*(count+2)+3+0] = (float)vertex_colours[2].r / 255.0f;
	vertices[9*(count+2)+3+1] = (float)vertex_colours[2].g / 255.0f;
	vertices[9*(count+2)+3+2] = (float)vertex_colours[2].b / 255.0f;
	vertices[9*(count+2)+3+3] = (float)vertex_colours[2].a / 255.0f;

	vertices[9*(count+3)+3+0] = (float)vertex_colours[0].r / 255.0f;
	vertices[9*(count+3)+3+1] = (float)vertex_colours[0].g / 255.0f;
	vertices[9*(count+3)+3+2] = (float)vertex_colours[0].b / 255.0f;
	vertices[9*(count+3)+3+3] = (float)vertex_colours[0].a / 255.0f;

	vertices[9*(count+4)+3+0] = (float)vertex_colours[2].r / 255.0f;
	vertices[9*(count+4)+3+1] = (float)vertex_colours[2].g / 255.0f;
	vertices[9*(count+4)+3+2] = (float)vertex_colours[2].b / 255.0f;
	vertices[9*(count+4)+3+3] = (float)vertex_colours[2].a / 255.0f;

	vertices[9*(count+5)+3+0] = (float)vertex_colours[3].r / 255.0f;
	vertices[9*(count+5)+3+1] = (float)vertex_colours[3].g / 255.0f;
	vertices[9*(count+5)+3+2] = (float)vertex_colours[3].b / 255.0f;
	vertices[9*(count+5)+3+3] = (float)vertex_colours[3].a / 255.0f;

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
		dx /= (float)screen_w;
		dy /= (float)screen_h;
		dx2 /= (float)screen_w;
		dy2 /= (float)screen_h;
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
		vertices[9*(count+0)+7] = tu;
		vertices[9*(count+0)+8] = tv;
		vertices[9*(count+1)+7] = tu2;
		vertices[9*(count+1)+8] = tv;
		vertices[9*(count+2)+7] = tu2;
		vertices[9*(count+2)+8] = tv2;
		vertices[9*(count+3)+7] = tu;
		vertices[9*(count+3)+8] = tv;
		vertices[9*(count+4)+7] = tu2;
		vertices[9*(count+4)+8] = tv2;
		vertices[9*(count+5)+7] = tu;
		vertices[9*(count+5)+8] = tv2;
	}

	vertices[9*(count+0)+3+0] = (float)vertex_colours[0].r / 255.0f;
	vertices[9*(count+0)+3+1] = (float)vertex_colours[0].g / 255.0f;
	vertices[9*(count+0)+3+2] = (float)vertex_colours[0].b / 255.0f;
	vertices[9*(count+0)+3+3] = (float)vertex_colours[0].a / 255.0f;

	vertices[9*(count+1)+3+0] = (float)vertex_colours[1].r / 255.0f;
	vertices[9*(count+1)+3+1] = (float)vertex_colours[1].g / 255.0f;
	vertices[9*(count+1)+3+2] = (float)vertex_colours[1].b / 255.0f;
	vertices[9*(count+1)+3+3] = (float)vertex_colours[1].a / 255.0f;

	vertices[9*(count+2)+3+0] = (float)vertex_colours[2].r / 255.0f;
	vertices[9*(count+2)+3+1] = (float)vertex_colours[2].g / 255.0f;
	vertices[9*(count+2)+3+2] = (float)vertex_colours[2].b / 255.0f;
	vertices[9*(count+2)+3+3] = (float)vertex_colours[2].a / 255.0f;

	vertices[9*(count+3)+3+0] = (float)vertex_colours[0].r / 255.0f;
	vertices[9*(count+3)+3+1] = (float)vertex_colours[0].g / 255.0f;
	vertices[9*(count+3)+3+2] = (float)vertex_colours[0].b / 255.0f;
	vertices[9*(count+3)+3+3] = (float)vertex_colours[0].a / 255.0f;

	vertices[9*(count+4)+3+0] = (float)vertex_colours[2].r / 255.0f;
	vertices[9*(count+4)+3+1] = (float)vertex_colours[2].g / 255.0f;
	vertices[9*(count+4)+3+2] = (float)vertex_colours[2].b / 255.0f;
	vertices[9*(count+4)+3+3] = (float)vertex_colours[2].a / 255.0f;

	vertices[9*(count+5)+3+0] = (float)vertex_colours[3].r / 255.0f;
	vertices[9*(count+5)+3+1] = (float)vertex_colours[3].g / 255.0f;
	vertices[9*(count+5)+3+2] = (float)vertex_colours[3].b / 255.0f;
	vertices[9*(count+5)+3+3] = (float)vertex_colours[3].a / 255.0f;

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

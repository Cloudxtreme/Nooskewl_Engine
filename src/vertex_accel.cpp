#include "starsquatters.h"
#include "image.h"
#include "vertex_accel.h"
#include "video.h"

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
	GLint use_tex = glGetUniformLocation(current_shader, "use_tex");
	if (image) {
		glUniform1i(use_tex, true);
	}
	else {
		glUniform1i(use_tex, false);
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9*count, vertices, GL_DYNAMIC_DRAW);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(current_shader, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(current_shader, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

	GLint texcoordAttrib = glGetAttribLocation(current_shader, "texcoord");
	glEnableVertexAttribArray(texcoordAttrib);
	glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

	glDrawArrays(GL_TRIANGLES, 0, count);

	count = 0;
}

void Vertex_Accel::buffer(float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, SDL_Colour vertex_colours[4], int flags)
{
	maybe_resize_buffer(256);

	float dx2 = dx + dw;
	float dy2 = dy + dh;

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

	float tu, tv, tu2, tv2;
	if (image) {
		float tu = sx / image->w;
		//float tv = 1.0f - (sy / image->h);
		float tv = sy / image->h;
		float tu2 = tu + sw / image->w;
		//float tv2 = tv - (sh / image->h);
		float tv2 = tv + sh / image->h;

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

	vertices[9*(count+0)+3+0] = vertex_colours[0].r / 255.0f;
	vertices[9*(count+0)+3+1] = vertex_colours[0].g / 255.0f;
	vertices[9*(count+0)+3+2] = vertex_colours[0].b / 255.0f;
	vertices[9*(count+0)+3+3] = vertex_colours[0].a / 255.0f;

	vertices[9*(count+1)+3+0] = vertex_colours[1].r / 255.0f;
	vertices[9*(count+1)+3+1] = vertex_colours[1].g / 255.0f;
	vertices[9*(count+1)+3+2] = vertex_colours[1].b / 255.0f;
	vertices[9*(count+1)+3+3] = vertex_colours[1].a / 255.0f;

	vertices[9*(count+2)+3+0] = vertex_colours[2].r / 255.0f;
	vertices[9*(count+2)+3+1] = vertex_colours[2].g / 255.0f;
	vertices[9*(count+2)+3+2] = vertex_colours[2].b / 255.0f;
	vertices[9*(count+2)+3+3] = vertex_colours[2].a / 255.0f;

	vertices[9*(count+3)+3+0] = vertex_colours[0].r / 255.0f;
	vertices[9*(count+3)+3+1] = vertex_colours[0].g / 255.0f;
	vertices[9*(count+3)+3+2] = vertex_colours[0].b / 255.0f;
	vertices[9*(count+3)+3+3] = vertex_colours[0].a / 255.0f;

	vertices[9*(count+4)+3+0] = vertex_colours[2].r / 255.0f;
	vertices[9*(count+4)+3+1] = vertex_colours[2].g / 255.0f;
	vertices[9*(count+4)+3+2] = vertex_colours[2].b / 255.0f;
	vertices[9*(count+4)+3+3] = vertex_colours[2].a / 255.0f;

	vertices[9*(count+5)+3+0] = vertex_colours[3].r / 255.0f;
	vertices[9*(count+5)+3+1] = vertex_colours[3].g / 255.0f;
	vertices[9*(count+5)+3+2] = vertex_colours[3].b / 255.0f;
	vertices[9*(count+5)+3+3] = vertex_colours[3].a / 255.0f;

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
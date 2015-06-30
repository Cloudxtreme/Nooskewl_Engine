#include "starsquatters.h"
#include "image.h"
#include "vertex_accel.h"

// FIXME:
extern GLuint current_shader;

Vertex_Accel *vertex_accel;

Vertex_Accel::Vertex_Accel() :
	vertices(NULL),
	count(0),
	total(0)
{
}

Vertex_Accel::~Vertex_Accel()
{
	free(vertices);
}

bool Vertex_Accel::init()
{
	if (!maybe_resize_buffer(256)) {
		return false;
	}

	return true;
}

void Vertex_Accel::start(Image *image)
{
	this->image = image;
}

void Vertex_Accel::end()
{
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

bool Vertex_Accel::buffer(float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	if (maybe_resize_buffer(256) == false) {
		return false;
	}

	float dx2 = dx + sw;
	float dy2 = dy + sh;

	// FIXME: target height
	//dy = 768 - dy;
	//dy2 = 768 - dy2;

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
		vertices[9*(count+i)+2] = 0; // set vertex z
	}

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

	// Set vertex r, g, b, a
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			vertices[9*(count+i)+3+j] = 1.0f; // r, g, b, a
		}
	}

	count += 6;

	return true;
}

bool Vertex_Accel::maybe_resize_buffer(int increase)
{
	if (total - count >= increase) {
		return true;
	}

	if (vertices == NULL) {
		vertices = (float *)malloc(sizeof(float)*9*increase);
	}
	else {
		vertices = (float *)realloc(vertices, sizeof(float)*9*(total+increase));
	}
	if (vertices == NULL) {
		return false;
	}

	total += increase;

	return true;
}
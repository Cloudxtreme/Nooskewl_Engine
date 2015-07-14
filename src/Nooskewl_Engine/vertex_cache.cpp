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

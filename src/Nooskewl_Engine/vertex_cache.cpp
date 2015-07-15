#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/vertex_cache.h"

#ifdef NOOSKEWL_ENGINE_WINDOWS
#define NOOSKEWL_ENGINE_FVF (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE4(1))
#endif

using namespace Nooskewl_Engine;

Vertex_Cache::Vertex_Cache() :
	vertices(0),
	count(0),
	total(0),
	perspective_drawing(false),
	repeat(false)
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

void Vertex_Cache::start(bool repeat)
{
	image = 0;
	this->repeat = repeat;

#ifdef NOOSKEWL_ENGINE_WINDOWS
	if (noo.opengl == false) {
		noo.d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		noo.d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		noo.d3d_device->SetFVF(NOOSKEWL_ENGINE_FVF);
		noo.current_shader.d3d_effect->SetBool("use_tex", false);
		noo.current_shader.d3d_effect->Begin(&required_passes, 0);
		noo.d3d_device->SetTexture(0, NULL);
	}
#endif
}

void Vertex_Cache::start(Image *image, bool repeat)
{
	this->image = image;
	this->repeat = repeat;

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
		noo.d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		noo.d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		noo.d3d_device->SetFVF(NOOSKEWL_ENGINE_FVF);
		noo.current_shader.d3d_effect->SetBool("use_tex", true);
		noo.current_shader.d3d_effect->SetTexture("tex", image->internal->video_texture);
		noo.current_shader.d3d_effect->Begin(&required_passes, 0);
		noo.d3d_device->SetTexture(0, image->internal->video_texture);
	}
#endif
}

void Vertex_Cache::end()
{
	if (noo.opengl) {
		GLint use_tex = glGetUniformLocation(noo.current_shader.opengl_shader, "use_tex");
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
	
		GLint posAttrib = glGetAttribLocation(noo.current_shader.opengl_shader, "in_position");
		printGLerror("glGetAttribLocation");
		glEnableVertexAttribArray(posAttrib);
		printGLerror("glEnableVertexAttribArray");
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
		printGLerror("glVertexAttribPointer");

		GLint texcoordAttrib = glGetAttribLocation(noo.current_shader.opengl_shader, "in_texcoord");
		printGLerror("glGetAttribLocation");
		glEnableVertexAttribArray(texcoordAttrib);
		printGLerror("glEnableVertexAttribArray");
		glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		printGLerror("glVertexAttribPointer");

		GLint colAttrib = glGetAttribLocation(noo.current_shader.opengl_shader, "in_colour");
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
			noo.current_shader.d3d_effect->BeginPass(i);
			if (noo.d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, (void *)vertices, 9*sizeof(float)) != D3D_OK) {
				infomsg("DrawPrimitiveUP failed\n");
				return;
			}
			noo.current_shader.d3d_effect->EndPass();
		}
		noo.current_shader.d3d_effect->End();
	}
#endif

	count = 0;
}

void Vertex_Cache::enable_perspective_drawing(int screen_w, int screen_h)
{
	perspective_drawing = true;
	this->screen_w = screen_w;
	this->screen_h = screen_h;
}

void Vertex_Cache::disable_perspective_drawing()
{
	perspective_drawing = false;
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

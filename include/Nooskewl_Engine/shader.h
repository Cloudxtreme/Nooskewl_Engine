#ifndef SHADER_H
#define SHADER_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Shader {
public:
	Shader(bool opengl, std::string vertex_source, std::string fragment_source);
	~Shader();

	void use();

	void set_texture(std::string name, Image *image);
	void set_matrix(std::string name, const float *matrix);
	void set_float(std::string name, float value);
	void set_bool(std::string name, bool value);

	GLuint get_opengl_shader();
#ifdef NOOSKEWL_ENGINE_WINDOWS
	LPD3DXEFFECT get_d3d_effect();
#endif

private:
	bool opengl;

	std::string vertex_source;
	std::string fragment_source;

	// OpenGL
	GLuint opengl_vertex_shader;
	GLuint opengl_fragment_shader;
	GLuint opengl_shader;

	// D3D
#ifdef NOOSKEWL_ENGINE_WINDOWS
	LPD3DXEFFECT d3d_effect;
	D3DXHANDLE d3d_technique;
#endif
};

} // End namespace Nooskewl_Engine

#endif // SHADER_H
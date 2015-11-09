#ifndef SHADER_H
#define SHADER_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Shader {
public:
	Shader(bool opengl, std::string vertex_source, std::string fragment_source);
	~Shader();

	static void release_all();
	static void reload_all();

	void use();

	void set_texture(std::string name, Image *image);
	void set_matrix(std::string name, const float *matrix);
	void set_float(std::string name, float value);
	bool set_float_vector(std::string name, int num_components, float *vector, int num_elements);
	void set_bool(std::string name, bool value);

	float get_global_alpha();
	void set_global_alpha(float global_alpha);

	GLuint get_opengl_shader();
#ifdef NOOSKEWL_ENGINE_WINDOWS
	LPD3DXEFFECT get_d3d_effect();
#endif

private:
	class Internal {
public:
		void release();
		void reload();

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

	Internal *internal;

	static std::vector<Internal *> loaded_shaders;

private:
	float global_alpha;
};

} // End namespace Nooskewl_Engine

#endif // SHADER_H

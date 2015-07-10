#ifndef MODULE_H
#define MODULE_H

// Module-global variables and stuff

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/load_dll.h"
#include "Nooskewl_Engine/sample.h"
#include "Nooskewl_Engine/vertex_cache.h"

#ifdef NOOSKEWL_ENGINE_BUILD

namespace Nooskewl_Engine {

struct Module {
	Map_Logic_Getter get_map_logic;
	// audio
	SDL_mutex *sample_mutex;
	SDL_AudioSpec device_spec;
	std::vector<SampleInstance *> playing_samples;
	// graphics
#ifdef _MSC_VER
	IDirect3DDevice9 *d3d_device;
	LPD3DXEFFECT effect;
#endif
	GLuint current_shader;
	Vertex_Cache *vertex_cache;
};

extern Module m;

} // End namespace Nooskewl_Engine

#endif

#endif // MODULE_H
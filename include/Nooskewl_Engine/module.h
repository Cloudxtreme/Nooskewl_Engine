#ifndef MODULE_H
#define MODULE_H

// Module-global variables and stuff

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/load_dll.h"
#include "Nooskewl_Engine/sample.h"

struct Module {
#ifdef _MSC_VER
	IDirect3DDevice9 *d3d_device;
	LPD3DXEFFECT effect;
#endif
	Map_Logic_Getter get_map_logic;
	// audio
	SDL_mutex *sample_mutex;
	SDL_AudioSpec device_spec;
	std::vector<SampleInstance *> playing_samples;
};

extern Module m;

#endif // MODULE_H
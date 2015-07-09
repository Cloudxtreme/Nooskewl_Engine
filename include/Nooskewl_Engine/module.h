#ifndef MODULE_H
#define MODULE_H

// Module-global variables and stuff

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/load_dll.h"

struct Module {
	IDirect3DDevice9 *d3d_device;
	LPD3DXEFFECT effect;
	Map_Logic_Getter get_map_logic;
};

extern Module m;

#endif // MODULE_H
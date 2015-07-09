#ifndef VIDEO_H
#define VIDEO_H

#include "Nooskewl_Engine/main.h"

EXPORT void clear(SDL_Colour colour);
EXPORT void flip();

void init_video(int argc, char **argv);
void shutdown_video();

EXPORT void set_default_projection();
void set_map_transition_projection(float angle);

void release_graphics();
void reload_graphics();

#define FVF (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE4(1))
extern IDirect3DDevice9 *d3d_device;
extern LPD3DXEFFECT effect;

#endif // VIDEO_H

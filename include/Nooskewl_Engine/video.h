#ifndef VIDEO_H
#define VIDEO_H

#include "Nooskewl_Engine/main.h"

EXPORT void clear(SDL_Colour colour);
EXPORT void flip();

EXPORT void init_video(int argc, char **argv);
EXPORT void shutdown_video();

EXPORT void set_default_projection();
EXPORT void set_map_transition_projection(float angle);

EXPORT void release_graphics();
EXPORT void reload_graphics();

EXPORT extern int screen_w;
EXPORT extern int screen_h;

EXPORT extern bool opengl;

#define FVF (D3DFVF_XYZ | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE4(1))
EXPORT extern IDirect3DDevice9 *d3d_device;
EXPORT extern LPD3DXEFFECT effect;

#endif // VIDEO_H

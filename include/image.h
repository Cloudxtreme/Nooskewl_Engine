#ifndef IMAGE_H
#define IMAGE_H

#include <string>

#include <windows.h>
#include <GL/gl.h>

#include <SDL_rwops.h>

class Image {
public:
	GLuint texture;
	int width;
	int height;

	Image();
	~Image();

	// Supports: TGA 16/24/32 bit
	bool load_tga(SDL_RWops *file);
};

#endif IMAGE_H
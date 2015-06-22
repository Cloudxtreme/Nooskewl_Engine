// http://paulbourke.net/dataformats/tga/

#include "starsquatters.h"

typedef struct {
	char idlength;
	char colourmaptype;
	char datatypecode;
	short int colourmaporigin;
	short int colourmaplength;
	char colourmapdepth;
	short int x_origin;
	short int y_origin;
	short width;
	short height;
	char bitsperpixel;
	char imagedescriptor;
} TGA_HEADER;

static void MergeBytes(unsigned char *pixel,unsigned char *p,int bytes)
{
	if (bytes == 4) {
		*pixel++ = p[2];
		*pixel++ = p[1];
		*pixel++ = p[0];
		*pixel++ = p[3];
	}
	else if (bytes == 3) {
		*pixel++ = p[2];
		*pixel++ = p[1];
		*pixel++ = p[0];
		*pixel++ = 255;
	}
	else if (bytes == 2) {
		*pixel++ = (p[1] & 0x7c) << 1;
		*pixel++ = ((p[1] & 0x03) << 6) | ((p[0] & 0xe0) >> 2);
		*pixel++ = (p[0] & 0x1f) << 3;
		*pixel++ = (p[1] & 0x80);
	}
}

// FIXME: MSVC inline
static inline unsigned char *pixel_ptr(unsigned char *p, int n, TGA_HEADER *h)
{
	int flipped = (h->imagedescriptor & 0x20) != 0;
	if (flipped) {
		int x = n % h->width;
		int y = n / h->width;
		return p + (h->width * 4) * (h->height-1) - (y * h->width * 4) +  x * 4;
	}
	else
		return p + n * 4;
}

Image::Image() :
	texture(0)
{
	// Set vertex constants: rgba and texture coordinates
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			vertices[9*i+3+j] = 1.0f; // r, g, b, a
		}
	}
	// texture coordinates
	vertices[9*0+7] = 0;
	vertices[9*0+8] = 1;
	vertices[9*1+7] = 1;
	vertices[9*1+8] = 1;
	vertices[9*2+7] = 1;
	vertices[9*2+8] = 0;
	vertices[9*3+7] = 0;
	vertices[9*3+8] = 1;
	vertices[9*4+7] = 1;
	vertices[9*4+8] = 0;
	vertices[9*5+7] = 0;
	vertices[9*5+8] = 0;
}

Image::~Image()
{
	glDeleteTextures(1, &texture);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

bool Image::load_tga(SDL_RWops *file)
{
	int n = 0, i, j;
	int bytes2read, skipover = 0;
	unsigned char p[5];
	TGA_HEADER header;
	unsigned char *pixels;

	/* Display the header fields */
	header.idlength = SDL_fgetc(file);
	header.colourmaptype = SDL_fgetc(file);
	header.datatypecode = SDL_fgetc(file);
	header.colourmaporigin = SDL_ReadLE16(file);
	header.colourmaplength = SDL_ReadLE16(file);
	header.colourmapdepth = SDL_fgetc(file);
	header.x_origin = SDL_ReadLE16(file);
	header.y_origin = SDL_ReadLE16(file);
	header.width = SDL_ReadLE16(file);
	header.height = SDL_ReadLE16(file);
	header.bitsperpixel = SDL_fgetc(file);
	header.imagedescriptor = SDL_fgetc(file);

	width = header.width;
	height = header.height;

	/* Allocate space for the image */
	if ((pixels = new unsigned char[header.width*header.height*4]) == NULL) {
		logmsg("malloc of image failed\n");
		return false;
	}

	/* What can we handle */
	if (header.datatypecode != 2 && header.datatypecode != 10) {
		logmsg("Can only handle image type 2 and 10\n");
		return false;
	}		
	if (header.bitsperpixel != 16 && 
		header.bitsperpixel != 24 && header.bitsperpixel != 32) {
		logmsg("Can only handle pixel depths of 16, 24, and 32\n");
		return false;
	}
	if (header.colourmaptype != 0 && header.colourmaptype != 1) {
		logmsg("Can only handle colour map types of 0 and 1\n");
		return false;
	}

	/* Skip over unnecessary stuff */
	skipover += header.idlength;
	skipover += header.colourmaptype * header.colourmaplength;
	SDL_RWseek(file, skipover, RW_SEEK_CUR);

	/* Read the image */
	bytes2read = header.bitsperpixel / 8;
	while (n < header.width * header.height) {
		if (header.datatypecode == 2) {                     /* Uncompressed */
			if (SDL_RWread(file, p, 1, bytes2read) != bytes2read) {
				logmsg("Unexpected end of file at pixel %d\n",i);
				delete[] pixels;
				return false;
			}
			MergeBytes(pixel_ptr(pixels, n, &header), p, bytes2read);
			n++;
		}
		else if (header.datatypecode == 10) {             /* Compressed */
			if (SDL_RWread(file, p, 1, bytes2read+1) != bytes2read+1) {
				logmsg("Unexpected end of file at pixel %d\n",i);
				delete[] pixels;
				return false;
			}
			j = p[0] & 0x7f;
			MergeBytes(pixel_ptr(pixels, n, &header), &(p[1]), bytes2read);
			n++;
			if (p[0] & 0x80) {         /* RLE chunk */
				for (i = 0; i < j; i++) {
					MergeBytes(pixel_ptr(pixels, n, &header), &(p[1]), bytes2read);
					n++;
				}
			}
			else {                   /* Normal chunk */
				for (i = 0; i < j; i++) {
					if (SDL_RWread(file, p, 1, bytes2read) != bytes2read) {
						logmsg("Unexpected end of file at pixel %d\n",i);
						delete[] pixels;
						return false;
					}
					MergeBytes(pixel_ptr(pixels, n, &header), p, bytes2read);
					n++;
				}
			}
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenTextures(1, &texture);
	if (texture == 0) {
		delete[] pixels;
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	delete[] pixels;

	return true;
}

void Image::draw(float x, float y, float z)
{
	// Set varying vertex attributes: xy
	vertices[9*0+0] = x;
	vertices[9*0+1] = y;
	vertices[9*1+0] = x+width-1;
	vertices[9*1+1] = y;
	vertices[9*2+0] = x+width-1;
	vertices[9*2+1] = y+height-1;
	vertices[9*3+0] = x;
	vertices[9*3+1] = y;
	vertices[9*4+0] = x+width-1;
	vertices[9*4+1] = y+height-1;
	vertices[9*5+0] = x;
	vertices[9*5+1] = y+height-1;

	for (int i = 0; i < 6; i++) {
		vertices[9*i+2] = z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9*6, vertices, GL_DYNAMIC_DRAW);

	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0);

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
// http://paulbourke.net/dataformats/tga/

#include "starsquatters.h"
#include "image.h"
#include "log.h"
#include "util.h"

// FIXME:
extern GLuint current_shader;

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

	w = header.width;
	h = header.height;

	/* Allocate space for the image */
	if ((pixels = new unsigned char[header.width*header.height*4]) == NULL) {
		errormsg("malloc of image failed\n");
		SDL_RWclose(file);
		return false;
	}

	/* What can we handle */
	if (header.datatypecode != 2 && header.datatypecode != 10) {
		errormsg("Can only handle image type 2 and 10\n");
		SDL_RWclose(file);
		return false;
	}		
	if (header.bitsperpixel != 16 && 
		header.bitsperpixel != 24 && header.bitsperpixel != 32) {
		errormsg("Can only handle pixel depths of 16, 24, and 32\n");
		SDL_RWclose(file);
		return false;
	}
	if (header.colourmaptype != 0 && header.colourmaptype != 1) {
		errormsg("Can only handle colour map types of 0 and 1\n");
		SDL_RWclose(file);
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
				errormsg("Unexpected end of file at pixel %d\n",i);
				delete[] pixels;
				SDL_RWclose(file);
				return false;
			}
			MergeBytes(pixel_ptr(pixels, n, &header), p, bytes2read);
			n++;
		}
		else if (header.datatypecode == 10) {             /* Compressed */
			if (SDL_RWread(file, p, 1, bytes2read+1) != bytes2read+1) {
				errormsg("Unexpected end of file at pixel %d\n",i);
				delete[] pixels;
				SDL_RWclose(file);
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
						errormsg("Unexpected end of file at pixel %d\n",i);
						delete[] pixels;
						SDL_RWclose(file);
						return false;
					}
					MergeBytes(pixel_ptr(pixels, n, &header), p, bytes2read);
					n++;
				}
			}
		}
	}

	if (upload(pixels) == false) {
		delete[] pixels;
		SDL_RWclose(file);
		return false;
	}

	delete[] pixels;

	SDL_RWclose(file);

	return true;
}

bool Image::from_surface(SDL_Surface *surface)
{
	unsigned char *pixels;
	SDL_Surface *tmp = NULL;

	if (surface->format->format == SDL_PIXELFORMAT_RGBA8888)
		pixels = (unsigned char *)surface->pixels;
	else {
		SDL_PixelFormat format;
		format.format = SDL_PIXELFORMAT_RGBA8888;
		format.palette = NULL;
		format.BitsPerPixel = 32;
		format.BytesPerPixel = 4;
		format.Rmask = 0xff;
		format.Gmask = 0xff00;
		format.Bmask = 0xff0000;
		format.Amask = 0xff000000;
		tmp = SDL_ConvertSurface(surface, &format, 0);
		if (tmp == NULL) {
			return false;
		}
		pixels = (unsigned char *)tmp->pixels;
	}

	w = surface->w;
	h = surface->h;

	bool ret = upload(pixels);

	if (tmp) SDL_FreeSurface(tmp);

	if (ret == false) {
		return false;
	}

	return true;
}

void Image::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Image::draw_region(float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	float dx2 = dx + sw;
	float dy2 = dy + sh;

	// Set varying vertex attributes: xy
	vertices[9*0+0] = dx;
	vertices[9*0+1] = dy;
	vertices[9*1+0] = dx2;
	vertices[9*1+1] = dy;
	vertices[9*2+0] = dx2;
	vertices[9*2+1] = dy2;
	vertices[9*3+0] = dx;
	vertices[9*3+1] = dy;
	vertices[9*4+0] = dx2;
	vertices[9*4+1] = dy2;
	vertices[9*5+0] = dx;
	vertices[9*5+1] = dy2;
	for (int i = 0; i < 6; i++) {
		vertices[9*i+2] = 0; // z
	}

	float tu = sx / w;
	float tv = 1.0f - (sy / h);
	float tu2 = tu + sw / w;
	float tv2 = tv - (sh / h);

	if (flags & FLIP_H) {
		float tmp = tu;
		tu = tu2;
		tu2 = tmp;
	}
	if (flags & FLIP_V) {
		float tmp = tv;
		tv = tv2;
		tv2 = tmp;
	}

	// texture coordinates
	vertices[9*0+7] = tu;
	vertices[9*0+8] = tv2;
	vertices[9*1+7] = tu2;
	vertices[9*1+8] = tv2;
	vertices[9*2+7] = tu2;
	vertices[9*2+8] = tv;
	vertices[9*3+7] = tu;
	vertices[9*3+8] = tv2;
	vertices[9*4+7] = tu2;
	vertices[9*4+8] = tv;
	vertices[9*5+7] = tu;
	vertices[9*5+8] = tv;

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9*6, vertices, GL_DYNAMIC_DRAW);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(current_shader, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(current_shader, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

	GLint texcoordAttrib = glGetAttribLocation(current_shader, "texcoord");
	glEnableVertexAttribArray(texcoordAttrib);
	glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Image::draw(float dx, float dy, int flags)
{
	draw_region(0.0f, 0.0f, (float)w, (float)h, dx, dy, flags);
}

bool Image::upload(unsigned char *pixels)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenTextures(1, &texture);
	if (texture == 0) {
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return true;
}
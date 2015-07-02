// http://paulbourke.net/dataformats/tga/

#include "starsquatters.h"
#include "image.h"
#include "log.h"
#include "util.h"
#include "vertex_accel.h"

typedef struct Colour {
	unsigned char r, g, b;
};

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
	Colour palette[256];
} TGA_Header;

static void merge_bytes(unsigned char *pixel, unsigned char *p, int bytes, TGA_Header *header)
{
	if (header->colourmaptype == 1) {
		// Paletted
		if (*p == 0) {
			// transparent
			*pixel++ = 0;
			*pixel++ = 0;
			*pixel++ = 0;
			*pixel++ = 0;
		}
		else {
			Colour *colour = &header->palette[*p];

			*pixel++ = colour->r;
			*pixel++ = colour->g;
			*pixel++ = colour->b;
			*pixel++ = 255;
		}
	}
	else {
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
}

// FIXME: MSVC inline
static inline unsigned char *pixel_ptr(unsigned char *p, int n, TGA_Header *h)
{
	/* OpenGL expects upside down, so that's what we provide */
	int flip = (h->imagedescriptor & 0x20) != 0;
	if (flip) {
		int x = n % h->width;
		int y = n / h->width;
		return p + (h->width * 4) * (h->height-1) - (y * h->width * 4) +  x * 4;
	}
	else
		return p + n * 4;
}

Image::Image() :
	texture(0),
	filename("")
{
}

Image::~Image()
{
	glDeleteTextures(1, &texture);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

bool Image::load_tga(std::string filename)
{
	this->filename = filename;

	SDL_RWops *file = open_file(filename);
	if (file == NULL) {
		return false;
	}

	int n = 0, i, j;
	int bytes2read, skipover = 0;
	unsigned char p[5];
	TGA_Header header;
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
	if (header.datatypecode != 1 && header.datatypecode != 2 && header.datatypecode != 9 && header.datatypecode != 10) {
		errormsg("Can only handle image type 1, 2, 9 and 10\n");
		SDL_RWclose(file);
		return false;
	}		
	if (header.bitsperpixel != 8 && header.bitsperpixel != 16 && header.bitsperpixel != 24 && header.bitsperpixel != 32) {
		errormsg("Can only handle pixel depths of 8, 16, 24 and 32\n");
		SDL_RWclose(file);
		return false;
	}
	if (header.colourmaptype != 0 && header.colourmaptype != 1) {
		errormsg("Can only handle colour map types of 0 and 1\n");
		SDL_RWclose(file);
		return false;
	}

	/* Skip over unnecessary stuff */
	SDL_RWseek(file, header.idlength, RW_SEEK_CUR);

	/* Read the palette if there is one */
	if (header.colourmaptype == 1) {
		if (header.colourmapdepth != 24) {
			errormsg("Can't handle anything but 24 bit palettes\n");
			SDL_RWclose(file);
			return false;
		}
		if (header.bitsperpixel != 8) {
			errormsg("Can only read 8 bpp paletted images");
			SDL_RWclose(file);
			return false;
		}
		int skip = header.colourmaporigin * (header.colourmapdepth / 8);
		SDL_RWseek(file, skip, RW_SEEK_CUR);
		// We can only read 256 colour palettes max, skip the rest
		int size = MIN(header.colourmaplength-skip, 256);
		skip = (header.colourmaplength - size) * (header.colourmapdepth / 8);
		for (i = 0; i < size; i++) {
			header.palette[i].b = SDL_fgetc(file);
			header.palette[i].g = SDL_fgetc(file);
			header.palette[i].r = SDL_fgetc(file);
		}
		printf("skip=%d\n", skip);
		SDL_RWseek(file, skip, RW_SEEK_CUR);
	}
	else {
		// Skip the palette on truecolour images
		SDL_RWseek(file, (header.colourmapdepth / 8) * header.colourmaplength, RW_SEEK_CUR);
	}

	/* Read the image */
	bytes2read = header.bitsperpixel / 8;
	while (n < header.width * header.height) {
		if (header.datatypecode == 1 || header.datatypecode == 2) {                     /* Uncompressed */
			if (SDL_RWread(file, p, 1, bytes2read) != bytes2read) {
				errormsg("Unexpected end of file at pixel %d\n",i);
				delete[] pixels;
				SDL_RWclose(file);
				return false;
			}
			merge_bytes(pixel_ptr(pixels, n, &header), p, bytes2read, &header);
			n++;
		}
		else if (header.datatypecode == 9 || header.datatypecode == 10) {             /* Compressed */
			if (SDL_RWread(file, p, 1, bytes2read+1) != bytes2read+1) {
				errormsg("Unexpected end of file at pixel %d\n",i);
				delete[] pixels;
				SDL_RWclose(file);
				return false;
			}
			j = p[0] & 0x7f;
			merge_bytes(pixel_ptr(pixels, n, &header), &(p[1]), bytes2read, &header);
			n++;
			if (p[0] & 0x80) {         /* RLE chunk */
				for (i = 0; i < j; i++) {
					merge_bytes(pixel_ptr(pixels, n, &header), &(p[1]), bytes2read, &header);
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
					merge_bytes(pixel_ptr(pixels, n, &header), p, bytes2read, &header);
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

void Image::start()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindTexture(GL_TEXTURE_2D, texture);

	vertex_accel->start(this);
}

void Image::draw_region(float sx, float sy, float sw, float sh, float dx, float dy, int flags)
{
	vertex_accel->buffer(sx, sy, sw, sh, dx, dy, flags);
}

void Image::draw(float dx, float dy, int flags)
{
	draw_region(0.0f, 0.0f, (float)w, (float)h, dx, dy, flags);
}

void Image::end()
{
	vertex_accel->end();
}

bool Image::upload(unsigned char *pixels)
{
	glGenVertexArrays(1, &vao);
	if (vao == 0) {
		return false;
	}
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	if (vbo == 0) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
		return false;
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenTextures(1, &texture);
	if (texture == 0) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
		glDeleteBuffers(1, &vbo);
		vbo = 0;
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
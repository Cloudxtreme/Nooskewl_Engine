// http://paulbourke.net/dataformats/tga/

#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/log.h"
#include "Nooskewl_Engine/util.h"
#include "Nooskewl_Engine/vertex_accel.h"

struct Colour {
	unsigned char r, g, b;
};

struct TGA_Header {
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
};

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

Image::Image(std::string filename, bool is_absolute_path) :
	filename(filename),
	texture(0)
{
	if (is_absolute_path == false) {
		filename = "graphics/" + filename;
	}

	this->filename = filename;

	SDL_RWops *file = open_file(filename);

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
		SDL_RWclose(file);
		throw MemoryError("malloc of image failed");
	}

	/* What can we handle */
	if (header.datatypecode != 1 && header.datatypecode != 2 && header.datatypecode != 9 && header.datatypecode != 10) {
		SDL_RWclose(file);
		throw LoadError("can only handle image type 1, 2, 9 and 10");
	}		
	if (header.bitsperpixel != 8 && header.bitsperpixel != 16 && header.bitsperpixel != 24 && header.bitsperpixel != 32) {
		SDL_RWclose(file);
		throw LoadError("can only handle pixel depths of 8, 16, 24 and 32");
	}
	if (header.colourmaptype != 0 && header.colourmaptype != 1) {
		SDL_RWclose(file);
		throw LoadError("can only handle colour map types of 0 and 1");
	}

	/* Skip over unnecessary stuff */
	SDL_RWseek(file, header.idlength, RW_SEEK_CUR);

	/* Read the palette if there is one */
	if (header.colourmaptype == 1) {
		if (header.colourmapdepth != 24) {
			SDL_RWclose(file);
			throw LoadError("can't handle anything but 24 bit palettes");
		}
		if (header.bitsperpixel != 8) {
			SDL_RWclose(file);
			throw LoadError("can only read 8 bpp paletted images");
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
				delete[] pixels;
				SDL_RWclose(file);
				throw LoadError("unexpected end of file at pixel " + itos(i));
			}
			merge_bytes(pixel_ptr(pixels, n, &header), p, bytes2read, &header);
			n++;
		}
		else if (header.datatypecode == 9 || header.datatypecode == 10) {             /* Compressed */
			if (SDL_RWread(file, p, 1, bytes2read+1) != bytes2read+1) {
				delete[] pixels;
				SDL_RWclose(file);
				throw LoadError("unexpected end of file at pixel " + itos(i));
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
						delete[] pixels;
						SDL_RWclose(file);
						throw LoadError("unexpected end of file at pixel " + itos(i));
					}
					merge_bytes(pixel_ptr(pixels, n, &header), p, bytes2read, &header);
					n++;
				}
			}
		}
	}

	try {
		upload(pixels);
	}
	catch (Error e) {
		delete[] pixels;
		SDL_RWclose(file);
		throw e;
	}

	delete[] pixels;

	SDL_RWclose(file);
}

Image::Image(std::string filename) :
	Image(filename, false)
{
}

Image::Image(SDL_Surface *surface) :
	filename("--FROM SURFACE--"),
	texture(0)
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
			throw Error("SDL_ConvertSurface returned NULL");
		}
		pixels = (unsigned char *)tmp->pixels;
	}

	w = surface->w;
	h = surface->h;

	try {
		upload(pixels);
	}
	catch (Error e) {
		if (tmp) SDL_FreeSurface(tmp);
		throw e;
	}

	if (tmp) SDL_FreeSurface(tmp);
}

Image::~Image()
{
	glDeleteTextures(1, &texture);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Image::start()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindTexture(GL_TEXTURE_2D, texture);

	vertex_accel->start(this);
}

void Image::stretch_region(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, int flags)
{
	vertex_accel->buffer(source_position, source_size, dest_position, dest_size, four_whites, flags);
}

void Image::draw_region(Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags)
{
	vertex_accel->buffer(source_position, source_size, dest_position, source_size, four_whites, flags);
}

void Image::draw(Point<int> dest_position, int flags)
{
	draw_region(Point<int>(0, 0), Size<int>(w, h), dest_position, flags);
}

void Image::stretch_region_single(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, int flags)
{
	start();
	stretch_region(source_position, source_size, dest_position, dest_size, flags);
	end();
}

void Image::draw_region_single(Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags)
{
	start();
	draw_region(source_position, source_size, dest_position, flags);
	end();
}

void Image::draw_single(Point<int> dest_position, int flags)
{
	start();
	draw(dest_position, flags);
	end();
}

void Image::end()
{
	vertex_accel->end();
}

void Image::upload(unsigned char *pixels)
{
	glGenVertexArrays(1, &vao);
	if (vao == 0) {
		throw GLError("glGenVertexArrays failed");
	}
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	if (vbo == 0) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
		throw GLError("glBenBuffers failed");
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenTextures(1, &texture);
	if (texture == 0) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
		glDeleteBuffers(1, &vbo);
		vbo = 0;
		throw GLError("glGenTextures failed");
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    vertex_accel->init_new_texture();
}

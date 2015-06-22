// http://paulbourke.net/dataformats/tga/

#include <SDL_rwops.h>

#include "tga.h"

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
} HEADER;

void MergeBytes(unsigned char *pixel,unsigned char *p,int bytes)
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
static __inline unsigned char *pixel_ptr(unsigned char *p, int n, HEADER *h)
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

GLuint load_tga(const char *filename)
{
	int n=0, i, j;
	int bytes2read, skipover = 0;
	unsigned char p[5];
	HEADER header;
	unsigned char *pixels;
	SDL_RWops *file;

	/* Open the file */
	if ((file = SDL_RWFromFile(filename, "rb")) == NULL) {
		//fprintf(stderr,"File open failed\n");
		return 0;
	}

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

	/* Allocate space for the image */
	if ((pixels = malloc(header.width*header.height*4)) == NULL) {
		//fprintf(stderr,"malloc of image failed\n");
		SDL_RWclose(file);
		return 0;
	}

	/* What can we handle */
	if (header.datatypecode != 2 && header.datatypecode != 10) {
		//fprintf(stderr,"Can only handle image type 2 and 10\n");
		SDL_RWclose(file);
		return 0;
	}		
	if (header.bitsperpixel != 16 && 
		header.bitsperpixel != 24 && header.bitsperpixel != 32) {
		//fprintf(stderr,"Can only handle pixel depths of 16, 24, and 32\n");
		SDL_RWclose(file);
		return 0;
	}
	if (header.colourmaptype != 0 && header.colourmaptype != 1) {
		//fprintf(stderr,"Can only handle colour map types of 0 and 1\n");
		SDL_RWclose(file);
		return 0;
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
				//fprintf(stderr,"Unexpected end of file at pixel %d\n",i);
				free(pixels);
				SDL_RWclose(file);
				return 0;
			}
			MergeBytes(pixel_ptr(pixels, n, &header), p, bytes2read);
			n++;
		}
		else if (header.datatypecode == 10) {             /* Compressed */
			if (SDL_RWread(file, p, 1, bytes2read+1) != bytes2read+1) {
				//fprintf(stderr,"Unexpected end of file at pixel %d\n",i);
				free(pixels);
				SDL_RWclose(file);
				return 0;
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
						//fprintf(stderr,"Unexpected end of file at pixel %d\n",i);
						free(pixels);
						SDL_RWclose(file);
						return 0;
					}
					MergeBytes(pixel_ptr(pixels, n, &header), p, bytes2read);
					n++;
				}
			}
		}
	}
	SDL_RWclose(file);

	GLuint texture;
	glGenTextures(1, &texture);
	if (texture == 0) {
		free(pixels);
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header.width, header.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	free(pixels);
	return texture;
}
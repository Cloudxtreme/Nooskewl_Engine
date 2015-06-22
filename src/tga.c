// http://paulbourke.net/dataformats/tga/

#include <stdio.h>

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
		*pixel++ = 0;
	}
	else if (bytes == 2) {
		*pixel++ = (p[1] & 0x7c) << 1;
		*pixel++ = ((p[1] & 0x03) << 6) | ((p[0] & 0xe0) >> 2);
		*pixel++ = (p[0] & 0x1f) << 3;
		*pixel++ = (p[1] & 0x80);
	}
}

GLuint load_tga(const char *filename)
{
	int n=0, i, j;
	int bytes2read, skipover = 0;
	unsigned char p[5];
	FILE *fptr;
	HEADER header;
	unsigned char *pixels;

	/* Open the file */
	if ((fptr = fopen(filename, "rb")) == NULL) {
		fprintf(stderr,"File open failed\n");
		return 0;
	}

	/* Display the header fields */
	header.idlength = fgetc(fptr);
	header.colourmaptype = fgetc(fptr);
	header.datatypecode = fgetc(fptr);
	fread(&header.colourmaporigin, 2, 1, fptr);
	fread(&header.colourmaplength, 2, 1, fptr);
	header.colourmapdepth = fgetc(fptr);
	fread(&header.x_origin, 2, 1, fptr);
	fread(&header.y_origin, 2, 1, fptr);
	fread(&header.width, 2, 1, fptr);
	fread(&header.height, 2, 1, fptr);
	header.bitsperpixel = fgetc(fptr);
	header.imagedescriptor = fgetc(fptr);

	/* Allocate space for the image */
	if ((pixels = malloc(header.width*header.height*4)) == NULL) {
		fprintf(stderr,"malloc of image failed\n");
		fclose(fptr);
		return 0;
	}

	/* What can we handle */
	if (header.datatypecode != 2 && header.datatypecode != 10) {
		printf(stderr,"Can only handle image type 2 and 10\n");
		fclose(fptr);
		return 0;
	}		
	if (header.bitsperpixel != 16 && 
		header.bitsperpixel != 24 && header.bitsperpixel != 32) {
		fprintf(stderr,"Can only handle pixel depths of 16, 24, and 32\n");
		fclose(fptr);
		return 0;
	}
	if (header.colourmaptype != 0 && header.colourmaptype != 1) {
		fprintf(stderr,"Can only handle colour map types of 0 and 1\n");
		fclose(fptr);
		return 0;
	}

	/* Skip over unnecessary stuff */
	skipover += header.idlength;
	skipover += header.colourmaptype * header.colourmaplength;
	fseek(fptr,skipover,SEEK_CUR);

	/* Read the image */
	bytes2read = header.bitsperpixel / 8;
	while (n < header.width * header.height) {
		if (header.datatypecode == 2) {                     /* Uncompressed */
			if (fread(p, 1, bytes2read, fptr) != bytes2read) {
				fprintf(stderr,"Unexpected end of file at pixel %d\n",i);
				free(pixels);
				fclose(fptr);
				return 0;
			}
			MergeBytes(pixels+n*4, p, bytes2read);
			n++;
		}
		else if (header.datatypecode == 10) {             /* Compressed */
			if (fread(p, 1, bytes2read+1, fptr) != bytes2read+1) {
				fprintf(stderr,"Unexpected end of file at pixel %d\n",i);
				free(pixels);
				fclose(fptr);
				return 0;
			}
			j = p[0] & 0x7f;
			MergeBytes(pixels+n*4, &(p[1]), bytes2read);
			n++;
			if (p[0] & 0x80) {         /* RLE chunk */
				for (i=0;i<j;i++) {
					MergeBytes(pixels+n*4, &(p[1]), bytes2read);
					n++;
				}
			}
			else {                   /* Normal chunk */
				for (i = 0; i < j; i++) {
					if (fread(p, 1, bytes2read, fptr) != bytes2read) {
						fprintf(stderr,"Unexpected end of file at pixel %d\n",i);
						free(pixels);
						fclose(fptr);
					}
					MergeBytes(pixels+n*4, p, bytes2read);
					n++;
				}
			}
		}
	}
	fclose(fptr);

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
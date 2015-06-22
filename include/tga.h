#include <windows.h>
#include <GL/gl.h>

typedef struct Image {
	GLuint texture;
	int width;
	int height;
} Image;

Image load_tga(const char *filename);
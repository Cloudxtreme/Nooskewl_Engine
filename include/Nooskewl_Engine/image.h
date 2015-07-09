#ifndef IMAGE_H
#define IMAGE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/graphics.h"
#include "Nooskewl_Engine/types.h"

class EXPORT Image {
public:
	enum Flags {
		FLIP_H = 1,
		FLIP_V = 2
	};

	std::string filename;
	int w;
	int h;
	bool loaded;

	GLuint texture;
#ifdef _MSC_VER
	LPDIRECT3DTEXTURE9 video_texture;
#endif

	Image(std::string filename, bool is_absolute_path = false) throw (Error);
	Image(SDL_Surface *surface) throw (Error);
	~Image();

	void release();
	void reload() throw (Error);

	void start();
	void stretch_region(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, int flags = 0);
	void draw_region(Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags = 0);
	void draw(Point<int> dest_position, int flags = 0);
	void end(); // call after every group of draws

	// These ones call start/end automatically
	void stretch_region_single(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, int flags = 0);
	void draw_region_single(Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags = 0);
	void draw_single(Point<int> dest_position, int flags = 0);

private:
	void upload(unsigned char *data) throw (Error);

	GLuint vao;
	GLuint vbo;
};

#endif IMAGE_H

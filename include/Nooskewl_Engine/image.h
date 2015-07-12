#ifndef IMAGE_H
#define IMAGE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/types.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Image {
public:
	friend class NOOSKEWL_ENGINE_EXPORT Vertex_Cache;

	enum Flags {
		FLIP_H = 1,
		FLIP_V = 2
	};

	std::string filename;
	int w, h;

	Image(std::string filename, bool is_absolute_path = false);
	Image(SDL_Surface *surface);
	~Image();

	static void release_all();
	static void reload_all();
	static int get_unfreed_count();

	void release();
	void reload();

	void start(); // call before every group of draws of the same Image
	void end(); // call after every group of draws

	void stretch_region(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, int flags = 0);
	void draw_region_tinted(SDL_Colour colour, Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags = 0);
	void draw_region(Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags = 0);
	void draw_tinted(SDL_Colour colour, Point<int> dest_position, int flags = 0);
	void draw(Point<int> dest_position, int flags = 0);

	// These ones call start/end automatically each time
	void stretch_region_single(Point<int> source_position, Size<int> source_size, Point<int> dest_position, Size<int> dest_size, int flags = 0);
	void draw_region_tinted_single(SDL_Colour colour, Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags = 0);
	void draw_region_single(Point<int> source_position, Size<int> source_size, Point<int> dest_position, int flags = 0);
	void draw_tinted_single(SDL_Colour, Point<int> dest_position, int flags = 0);
	void draw_single(Point<int> dest_position, int flags = 0);

private:
	struct Internal {
		Internal(std::string filename);
		Internal(unsigned char *pixels, int w, int h);
		~Internal();

		void upload(unsigned char *pixels);

		void release();
		void reload();

		std::string filename;
		int w, h;
		int refcount;

	#ifdef _MSC_VER
		LPDIRECT3DTEXTURE9 video_texture;
	#endif
		GLuint vao;
		GLuint vbo;
		GLuint texture;
	};

	static std::vector<Internal *> loaded_images;

	Internal *internal;
};

} // End namespace Nooskewl_Engine

#endif // IMAGE_H

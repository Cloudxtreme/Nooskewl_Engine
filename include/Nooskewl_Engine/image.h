#ifndef IMAGE_H
#define IMAGE_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/basic_types.h"

namespace Nooskewl_Engine {

class Shader;

class NOOSKEWL_ENGINE_EXPORT Image {
public:
	friend class NOOSKEWL_ENGINE_EXPORT Shader;

	enum Flags {
		FLIP_H = 1,
		FLIP_V = 2
	};

	std::string filename;
	Size<int> size;

	Image(std::string filename, bool is_absolute_path = false);
	Image(SDL_Surface *surface);
	~Image();

	static void release_all();
	static void reload_all();
	static int get_unfreed_count();
	static unsigned char *read_tga(std::string filename, Size<int> &out_size);

	void release();
	void reload();

	void start(bool repeat = false); // call before every group of draws of the same Image
	void end(); // call after every group of draws

	void stretch_region_tinted_repeat(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags = 0);
	void stretch_region_tinted(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags = 0);
	void stretch_region(Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags = 0);
	void draw_region_tinted(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, int flags = 0);
	void draw_region_z(Point<float> source_position, Size<int> source_size, Point<float> dest_position, float z, int flags = 0);
	void draw_region(Point<float> source_position, Size<int> source_size, Point<float> dest_position, int flags = 0);
	void draw_z(Point<float> dest_position, float z, int flags = 0);
	void draw_tinted(SDL_Colour tint, Point<float> dest_position, int flags = 0);
	void draw(Point<float> dest_position, int flags = 0);

	// These ones call start/end automatically each time
	void stretch_region_tinted_repeat_single(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags = 0);
	void stretch_region_tinted_single(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags = 0);
	void stretch_region_single(Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags = 0);
	void draw_region_tinted_single(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, int flags = 0);
	void draw_region_z_single(Point<float> source_position, Size<int> source_size, Point<float> dest_position, float z, int flags = 0);
	void draw_region_single(Point<float> source_position, Size<int> source_size, Point<float> dest_position, int flags = 0);
	void draw_tinted_single(SDL_Colour, Point<float> dest_position, int flags = 0);
	void draw_z_single(Point<float> dest_position, float z, int flags = 0);
	void draw_tinted_single(Point<float> dest_position, int flags = 0);
	void draw_single(Point<float> dest_position, int flags = 0);

private:
	struct Internal {
		Internal(std::string filename);
		Internal(unsigned char *pixels, Size<int> size);
		~Internal();

		void upload(unsigned char *pixels);

		void release();
		void reload();

		std::string filename;
		Size<int> size;
		int refcount;

	#ifdef NOOSKEWL_ENGINE_WINDOWS
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

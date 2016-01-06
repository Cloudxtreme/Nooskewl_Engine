// http://paulbourke.net/dataformats/tga/

#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/vertex_cache.h"

static inline unsigned char *pixel_ptr(unsigned char *p, int n, bool flip, int w, int h)
{
	/* OpenGL expects upside down, so that's what we provide */
	if (flip) {
		int x = n % w;
		int y = n / w;
		return p + (w * 4) * (h-1) - (y * w * 4) +  x * 4;
	}
	else
		return p + n * 4;
}

using namespace Nooskewl_Engine;

bool Image::dumping_colours = false;
bool Image::keep_data = false;
bool Image::save_rle = false;
bool Image::ignore_palette;

std::vector<Image::Internal *> Image::loaded_images;

void Image::release_all()
{
	for (size_t i = 0; i < loaded_images.size(); i++) {
		loaded_images[i]->release();
	}
}

void Image::reload_all()
{
	for (size_t i = 0; i < loaded_images.size(); i++) {
		loaded_images[i]->reload(false);
	}
}

int Image::get_unfreed_count()
{
	for (size_t i = 0; i < loaded_images.size(); i++) {
		infomsg("Unfreed: %s\n", loaded_images[i]->filename.c_str());
	}
	return loaded_images.size();
}

unsigned char *Image::read_tga(std::string filename, Size<int> &out_size, SDL_Colour *out_palette)
{
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

	int w, h;
	out_size.w = w = header.width;
	out_size.h = h = header.height;

	/* Allocate space for the image */
	if ((pixels = new unsigned char[header.width*header.height*4]) == 0) {
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

	bool flip = (header.imagedescriptor & 0x20) != 0;

	/* Read the image */
	bytes2read = header.bitsperpixel / 8;
	while (n < header.width * header.height) {
		if (header.datatypecode == 1 || header.datatypecode == 2) {                     /* Uncompressed */
			if (SDL_RWread(file, p, 1, bytes2read) != bytes2read) {
				delete[] pixels;
				SDL_RWclose(file);
				throw LoadError("unexpected end of file at pixel " + itos(i));
			}
			merge_bytes(pixel_ptr(pixels, n, flip, w, h), p, bytes2read, &header);
			n++;
		}
		else if (header.datatypecode == 9 || header.datatypecode == 10) {             /* Compressed */
			if (SDL_RWread(file, p, 1, bytes2read+1) != bytes2read+1) {
				delete[] pixels;
				SDL_RWclose(file);
				throw LoadError("unexpected end of file at pixel " + itos(i));
			}
			j = p[0] & 0x7f;
			merge_bytes(pixel_ptr(pixels, n, flip, w, h), &(p[1]), bytes2read, &header);
			n++;
			if (p[0] & 0x80) {         /* RLE chunk */
				for (i = 0; i < j; i++) {
					merge_bytes(pixel_ptr(pixels, n, flip, w, h), &(p[1]), bytes2read, &header);
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
					merge_bytes(pixel_ptr(pixels, n, flip, w, h), p, bytes2read, &header);
					n++;
				}
			}
		}
	}

	SDL_RWclose(file);

	if (out_palette != 0) {
		memcpy(out_palette, header.palette, 256 * 3);
	}

	return pixels;
}

void Image::merge_bytes(unsigned char *pixel, unsigned char *p, int bytes, TGA_Header *header)
{
	if (header->colourmaptype == 1) {
		SDL_Colour *colour;
		if (ignore_palette) {
			colour = &noo.colours[*p];
		}
		else {
			colour = &header->palette[*p];
		}
		// Magic pink
		// Paletted
		if (colour->r == 255 && colour->g == 0 && colour->b == 255) {
			// transparent
			*pixel++ = 0;
			*pixel++ = 0;
			*pixel++ = 0;
			*pixel++ = 0;
		}
		else {
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

Image::Image(std::string filename, bool is_absolute_path)
{
	if (is_absolute_path == false) {
		filename = "images/" + filename;
	}

	this->filename = filename;

	reload();
}

Image::Image(SDL_Surface *surface) :
	filename("--FROM SURFACE--")
{
	unsigned char *pixels;
	SDL_Surface *tmp = 0;

	if (surface->format->format == SDL_PIXELFORMAT_RGBA8888)
		pixels = (unsigned char *)surface->pixels;
	else {
		SDL_PixelFormat format;
		format.format = SDL_PIXELFORMAT_RGBA8888;
		format.palette = 0;
		format.BitsPerPixel = 32;
		format.BytesPerPixel = 4;
		format.Rmask = 0xff;
		format.Gmask = 0xff00;
		format.Bmask = 0xff0000;
		format.Amask = 0xff000000;
		tmp = SDL_ConvertSurface(surface, &format, 0);
		if (tmp == 0) {
			throw Error("SDL_ConvertSurface returned 0");
		}
		pixels = (unsigned char *)tmp->pixels;
	}

	size = Size<int>(surface->w, surface->h);

	try {
		internal = new Internal(pixels, size);
	}
	catch (Error e) {
		if (tmp) SDL_FreeSurface(tmp);
		throw e;
	}

	if (tmp) {
		SDL_FreeSurface(tmp);
	}
}

Image::Image(Size<int> size) :
	filename("--FROM SURFACE--"), // handled the same
	size(size)
{
	unsigned char *pixels = (unsigned char *)calloc(1, size.w * size.h * 4);

	try {
		internal = new Internal(pixels, size, true); // support render to texture
	}
	catch (Error e) {
		free(pixels);
		throw e;
	}

	free(pixels);
}

Image::~Image()
{
	release();
}

void Image::release()
{
	if (filename == "--FROM SURFACE--") {
		delete internal;
		return;
	}

	for (size_t i = 0; i < loaded_images.size(); i++) {
		Internal *ii = loaded_images[i];
		if (ii->filename == filename) {
			ii->refcount--;
			if (ii->refcount == 0) {
				delete ii;
				loaded_images.erase(loaded_images.begin()+i);
				return;
			}
		}
	}
}

void Image::reload()
{
	if (filename == "--FROM SURFACE--") {
		return;
	}

	for (size_t i = 0; i < loaded_images.size(); i++) {
		Internal *ii = loaded_images[i];
		if (ii->filename == filename) {
			ii->refcount++;
			internal = ii;
			size = internal->size;
			return;
		}
	}

	internal = new Internal(filename, keep_data);
	size = internal->size;
	loaded_images.push_back(internal);
}

bool Image::save(std::string filename)
{
	unsigned char *loaded_data = internal->loaded_data;
	unsigned char header[] = { 0x00, 0x01, save_rle ? 0x09 : 0x01, 0x00, 0x00, 0x00, 0x01, 0x18, 0x00, 0x00, 0x00, 0x00, size.w & 0xff, (size.w >> 8) & 0xff, size.h & 0xff, (size.h >> 8) & 0xff, 0x08, 0x00 };
	int header_size = 18;

	SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "wb");
	if (file == 0) {
		throw new Error("Couldn't open " + filename + " for writing");
	}

	for (int i = 0; i < header_size; i++) {
		if (SDL_fputc(header[i], file) == EOF) {
			throw new Error("Write error writing to " + filename);
		}
	}

	for (int i = 0; i < 256; i++) {
		if (SDL_fputc(noo.colours[i].b, file) == EOF) {
			throw new Error("Write error writing to " + filename);
		}
		if (SDL_fputc(noo.colours[i].g, file) == EOF) {
			throw new Error("Write error writing to " + filename);
		}
		if (SDL_fputc(noo.colours[i].r, file) == EOF) {
			throw new Error("Write error writing to " + filename);
		}
	}

	#define R(n) *(pixel_ptr(loaded_data, n, false, size.w, size.h)+0)
	#define G(n) *(pixel_ptr(loaded_data, n, false, size.w, size.h)+1)
	#define B(n) *(pixel_ptr(loaded_data, n, false, size.w, size.h)+2)

	if (save_rle) {
		for (int i = 0; i < size.w * size.h;) {
			int j, count;
			for (j = i, count = 0; j < size.w * size.h - 1 && count < 127; j++, count++) {
				if (R(j) != R(j+1) || G(j) != G(j+1) || B(j) != B(j+1)) {
					break;
				}
			}
			int run_length = j - i + 1;
			if (run_length > 1) {
				SDL_fputc((run_length-1) | 0x80, file);
				SDL_fputc(find_colour_in_palette(&R(j)), file);
				i += run_length;
			}
			else {
				for (j = i, count = 0; j < size.w * size.h - 1 && count < 127; j++, count++) {
					if (R(j) == R(j+1) && G(j) == G(j+1) && B(j) == B(j+1)) {
						break;
					}
				}
				run_length = j - i + 1;
				SDL_fputc(run_length-1, file);
				for (j = 0; j < run_length; j++) {
					SDL_fputc(find_colour_in_palette(&R(i+j)), file);
				}
				i += run_length;
			}
		}
	}
	else {
		for (int i = 0; i < size.w * size.h; i++) {
			SDL_fputc(find_colour_in_palette(&R(i)), file);
		}
	}

	SDL_RWclose(file);

	return true;
}

unsigned char Image::find_colour_in_palette(unsigned char *p)
{
	for (unsigned int i = 0; i < 256; i++) {
		if (p[0] == noo.colours[i].r && p[1] == noo.colours[i].g && p[2] == noo.colours[i].b) {
			return i;
		}
	}

	errormsg("Error: colour %d,%d,%d not found!", p[0], p[1], p[2]);

	return 0;
}

void Image::start(bool repeat)
{
	m.vertex_cache->start(this, repeat);
}

void Image::end()
{
	m.vertex_cache->end();
}

void Image::stretch_region_tinted_repeat(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags)
{
	SDL_Colour colours[4];
	colours[0] = colours[1] = colours[2] = colours[3] = tint;

	int wt = dest_size.w / source_size.w;
	if (dest_size.w % source_size.w != 0) {
		wt++;
	}
	int ht = dest_size.h / source_size.h;
	if (dest_size.h % source_size.h != 0) {
		ht++;
	}

	int drawn_h = 0;
	for (int y = 0; y < ht; y++) {
		int drawn_w = 0;
		int h = source_size.h;
		if (dest_size.h - drawn_h < h) {
			h = dest_size.h- drawn_h;
		}
		for (int x = 0; x < wt; x++) {
			int w = source_size.w;
			if (dest_size.w - drawn_w < w) {
				w = dest_size.w - drawn_w;
			}
			Size<int> sz(w, h);
			m.vertex_cache->cache(colours, source_position, sz, Point<float>(dest_position.x + x * source_size.w, dest_position.y + y * source_size.h), sz, flags);
			drawn_w += w;
		}
		drawn_h += h;
	}
}

void Image::stretch_region_tinted(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags)
{
	SDL_Colour colours[4];
	colours[0] = colours[1] = colours[2] = colours[3] = tint;
	m.vertex_cache->cache(colours, source_position, source_size, dest_position, dest_size, flags);
}

void Image::stretch_region(Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags)
{
	m.vertex_cache->cache(noo.four_whites, source_position, source_size, dest_position, dest_size, flags);
}

void Image::draw_region_tinted(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, int flags)
{
	SDL_Colour colours[4];
	colours[0] = colours[1] = colours[2] = colours[3] = tint;
	m.vertex_cache->cache(colours, source_position, source_size, dest_position, source_size, flags);
}

void Image::draw_region_z(Point<float> source_position, Size<int> source_size, Point<float> dest_position, float z, int flags)
{
	m.vertex_cache->cache_z(noo.four_whites, source_position, source_size, dest_position, z, source_size, flags);
}

void Image::draw_region(Point<float> source_position, Size<int> source_size, Point<float> dest_position, int flags)
{
	draw_region_z(source_position, source_size, dest_position, 0.0f, flags);
}

void Image::draw_z(Point<float> dest_position, float z, int flags)
{
	draw_region_z(Point<float>(0, 0), size, dest_position, z, flags);
}

void Image::draw_tinted(SDL_Colour tint, Point<float> dest_position, int flags)
{
	draw_region_tinted(tint, Point<float>(0, 0), size, dest_position, flags);
}

void Image::draw(Point<float> dest_position, int flags)
{
	draw_z(dest_position, 0.0f, flags);
}

void Image::stretch_region_tinted_repeat_single(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags)
{
	start();
	stretch_region_tinted_repeat(tint, source_position, source_size, dest_position, dest_size, flags);
	end();
}

void Image::stretch_region_single(Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags)
{
	start();
	stretch_region(source_position, source_size, dest_position, dest_size, flags);
	end();
}

void Image::stretch_region_tinted_single(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, Size<int> dest_size, int flags)
{
	start();
	stretch_region_tinted(tint, source_position, source_size, dest_position, dest_size, flags);
	end();
}

void Image::draw_region_tinted_single(SDL_Colour tint, Point<float> source_position, Size<int> source_size, Point<float> dest_position, int flags)
{
	start();
	draw_region_tinted(tint, source_position, source_size, dest_position, flags);
	end();
}

void Image::draw_region_z_single(Point<float> source_position, Size<int> source_size, Point<float> dest_position, float z, int flags)
{
	start();
	draw_region_z(source_position, source_size, dest_position, z, flags);
	end();
}

void Image::draw_region_single(Point<float> source_position, Size<int> source_size, Point<float> dest_position, int flags)
{
	start();
	draw_region(source_position, source_size, dest_position, flags);
	end();
}

void Image::draw_z_single(Point<float> dest_position, float z, int flags)
{
	start();
	draw_z(dest_position, z, flags);
	end();
}

void Image::draw_tinted_single(SDL_Colour tint, Point<float> dest_position, int flags)
{
	start();
	draw_tinted(tint, dest_position, flags);
	end();
}

void Image::draw_single(Point<float> dest_position, int flags)
{
	draw_z_single(dest_position, 0.0f, flags);
}

void Image::set_target()
{
	glm::mat4 proj;

	if (noo.opengl) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, internal->fbo);
		glViewport(0, 0, size.w, size.h);
		glDisable(GL_SCISSOR_TEST);

		proj = glm::ortho(0.0f, (float)size.w, (float)size.h, 0.0f);
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		noo.d3d_device->EndScene();

		if (internal->video_texture->GetSurfaceLevel(0, &internal->render_target) != D3D_OK) {
			infomsg("Image::set_target: Unable to get texture surface level\n");
			return;
		}
		if (noo.d3d_device->SetRenderTarget(0, internal->render_target) != D3D_OK) {
			infomsg("Image::set_target: Unable to set render target to texture surface\n");
			internal->render_target->Release();
			return;
		}

		D3DVIEWPORT9 viewport;
		viewport.MinZ = 0;
		viewport.MaxZ = 1;
		viewport.X = 0;
		viewport.Y = 0;
		viewport.Width = size.w;
		viewport.Height = size.h;
		noo.d3d_device->SetViewport(&viewport);

		noo.set_initial_d3d_state();

		noo.d3d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

		proj = glm::ortho(0.0f, (float)size.w, 0.0f, (float)size.h);
	}
#endif

	// Set an ortho projection the size of the image
	glm::mat4 model = glm::mat4();
	glm::mat4 view = glm::mat4();

	noo.set_matrices(model, view, proj);
	noo.update_projection();
}

void Image::release_target()
{
	if (noo.opengl) {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		noo.d3d_device->EndScene();

		internal->render_target->Release();

		if (noo.d3d_device->SetRenderTarget(0, noo.render_target) != D3D_OK) {
			infomsg("Image::release_target: Unable to set render target to backbuffer\n");
		}

		noo.set_initial_d3d_state();
	}
#endif

	noo.set_default_projection();

	noo.set_screen_size(noo.real_screen_size.w, noo.real_screen_size.h); // this sets the viewport and scissor
}

//--

Image::Internal::Internal(std::string filename, bool keep_data, bool support_render_to_texture) :
	loaded_data(0),
	filename(filename),
	refcount(1),
	has_render_to_texture(support_render_to_texture)
{
	unsigned char *pixels = reload(keep_data);

	if (pixels == 0) {
		loaded_data = 0;
	}
	else {
		delete[] loaded_data;
		loaded_data = pixels;
	}
}

Image::Internal::Internal(unsigned char *pixels, Size<int> size, bool support_render_to_texture) :
	loaded_data(0),
	size(size),
	has_render_to_texture(support_render_to_texture)
{
	filename = "--FROM SURFACE--";
	upload(pixels);
}

Image::Internal::~Internal()
{
	release();

	delete[] loaded_data;
	loaded_data = 0;
}

void Image::Internal::release()
{
	if (noo.opengl) {
		if (has_render_to_texture) {
			glDeleteFramebuffersEXT(1, &fbo);
		}

		glDeleteTextures(1, &texture);
		printGLerror("glDeleteTextures");
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		video_texture->Release();

		if (has_render_to_texture) {
			system_texture->Release();
		}
	}
#endif
}

unsigned char *Image::Internal::reload(bool keep_data)
{
	unsigned char *pixels = Image::read_tga(filename, size);

	try {
		upload(pixels);
	}
	catch (Error e) {
		delete[] pixels;
		throw e;
	}

	if (keep_data == false) {
		delete[] pixels;
		return 0;
	}
	else {
		return pixels;
	}
}

void Image::Internal::upload(unsigned char *pixels)
{
	// To get a complete palette..
	if (dumping_colours) {
		unsigned char *rgb = pixels;
		for (int i = 0; i < size.w*size.h; i++) {
			if (rgb[3] != 0) {
				printf("rgb: %d %d %d\n", rgb[0], rgb[1], rgb[2]);
			}
			rgb += 4;
		}
	}

	if (noo.opengl) {
		glGenTextures(1, &texture);
		printGLerror("glGenTextures");
		if (texture == 0) {
			throw GLError("glGenTextures failed");
		}

		glActiveTexture(GL_TEXTURE0);
		printGLerror("glActiveTexture");

		glBindTexture(GL_TEXTURE_2D, texture);
		printGLerror("glBindTexture");

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.w, size.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		printGLerror("glTexImage2D");

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		printGLerror("glTexParameteri");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		printGLerror("glTexParameteri");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		printGLerror("glTexParameteri");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		printGLerror("glTexParameteri");

		if (has_render_to_texture) {
			// Create an FBO for render-to-texture
			GLuint old_fbo;

			glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, (GLint *)&old_fbo);
			printGLerror("glGetIntegerv");

			glGenFramebuffersEXT(1, &fbo);
			printGLerror("glGenFramebuffersEXT");

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
			printGLerror("glBindFramebufferEXT");

			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);
			printGLerror("glFramebufferTexture2DEXT");

			// Create a depth buffer
			glGenRenderbuffers(1, &depth_buffer);
			printGLerror("glGenRenderbuffers");

			glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
			printGLerror("glBindRenderbuffer");

			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.w, size.h);
			printGLerror("glRenderbufferStorage");

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
			printGLerror("glFramebufferRenderbuffer");

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, old_fbo);
			printGLerror("glBindFramebufferEXT");
		}
	}
#ifdef NOOSKEWL_ENGINE_WINDOWS
	else {
		int err;

		if (has_render_to_texture) {
			err = noo.d3d_device->CreateTexture(size.w, size.h, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &video_texture, 0);
			if (err != D3D_OK) {
				infomsg("CreateTexture failed for video texture\n");
			}

			err = noo.d3d_device->CreateTexture(size.w, size.h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &system_texture, 0);
			if (err != D3D_OK) {
				infomsg("CreateTexture failed for system texture\n");
			}

			D3DLOCKED_RECT locked_rect;
			if (system_texture->LockRect(0, &locked_rect, 0, 0) == D3D_OK) {
				for (int y = 0; y < size.h; y++) {
					unsigned char *dest = ((unsigned char *)locked_rect.pBits) + y * locked_rect.Pitch;
					for (int x = 0; x < size.w; x++) {
						unsigned char r = *pixels++;
						unsigned char g = *pixels++;
						unsigned char b = *pixels++;
						unsigned char a = *pixels++;
						*dest++ = b;
						*dest++ = g;
						*dest++ = r;
						*dest++ = a;
					}
				}
				system_texture->UnlockRect(0);
			}
			else {
				infomsg("Unable to lock system texture\n");
			}

			if (noo.d3d_device->UpdateTexture((IDirect3DBaseTexture9 *)system_texture, (IDirect3DBaseTexture9 *)video_texture) != D3D_OK) {
				infomsg("UpdateTexture failed\n");
			}
		}
		else {
			err = noo.d3d_device->CreateTexture(size.w, size.h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &video_texture, 0);
			if (err != D3D_OK) {
				infomsg("CreateTexture failed for video texture\n");
			}

			D3DLOCKED_RECT locked_rect;
			if (video_texture->LockRect(0, &locked_rect, 0, 0) == D3D_OK) {
				for (int y = 0; y < size.h; y++) {
					unsigned char *dest = ((unsigned char *)locked_rect.pBits) + y * locked_rect.Pitch;
					for (int x = 0; x < size.w; x++) {
						unsigned char r = *pixels++;
						unsigned char g = *pixels++;
						unsigned char b = *pixels++;
						unsigned char a = *pixels++;
						*dest++ = b;
						*dest++ = g;
						*dest++ = r;
						*dest++ = a;
					}
				}
				video_texture->UnlockRect(0);
			}
			else {
				infomsg("Unable to lock video texture\n");
			}
		}

		render_target = 0;
	}
#endif
}

#ifndef FONT_H
#define FONT_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/basic_types.h"

namespace Nooskewl_Engine {

class Image;

class NOOSKEWL_ENGINE_EXPORT Font {
public:
	enum Shadow_Type {
		NO_SHADOW,
		DROP_SHADOW,
		FULL_SHADOW
	};

	Font(std::string filename, int size);
	~Font();

	void clear_cache();

	float get_text_width(std::string text);
	float get_height();

	void enable_shadow(SDL_Colour shadow_colour, Shadow_Type shadow_type);
	void disable_shadow();

	void draw(SDL_Colour colour, std::string text, Point<int> dest_position);
	// Returns number of characters drawn, plus whether or not it filled the max in bool &full
	int draw_wrapped(SDL_Colour colour, std::string text, Point<int> dest_position, int w, int line_height, int max_lines, int started_time, int delay, bool dry_run, bool &full, int &num_lines, int &width);

private:
	void cache_glyph(Uint32 ch);
	void cache_glyphs(std::string text);

	SDL_RWops *file;
	TTF_Font *font;

	std::map<Uint32, Image *> glyphs;

	SDL_Colour shadow_colour;
	Shadow_Type shadow_type;

	int size;
};

} // End namespace Nooskewl_Engine

#endif // FONT_H

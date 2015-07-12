#ifndef FONT_H
#define FONT_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/types.h"

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

	int get_text_width(std::string text);

	void enable_shadow(SDL_Colour shadow_colour, Shadow_Type shadow_type);
	void disable_shadow();

	void draw(SDL_Colour colour, std::string text, Point<int> dest_position);
	// Returns number of characters drawn, plus whether or not it filled the max in bool &full
	int draw_wrapped(SDL_Colour colour, std::string text, Point<int> dest_position, int w, int line_height, int max_lines, int started_time, int delay, bool &full);

private:
	void cache_glyph(int ch);
	void cache_glyphs_if_needed(std::string text);

	SDL_RWops *file;
	TTF_Font *font;

	std::map<int, Image *> glyphs;

	SDL_Colour shadow_colour;
	Shadow_Type shadow_type;
};

} // End namespace Nooskewl_Engine

#endif // FONT_H
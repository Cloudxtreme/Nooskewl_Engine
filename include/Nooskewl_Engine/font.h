#ifndef FONT_H
#define FONT_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/types.h"

class Image;

class EXPORT Font {
public:
	static const int CHAR_DELAY = 50; // ms, for wrapped drawing with advancing text

	Font(std::string filename, int size) throw (Error);
	~Font();

	void clear_cache();

	int get_width(std::string text);

	void draw(std::string text, Point<int> dest_position, SDL_Color colour);
	// Returns number of characters drawn
	int draw_wrapped(std::string text, Point<int> dest_position, int w, int line_height, int max_lines, int started_time, SDL_Color colour, bool &full);

private:
	struct Glyph {
		Image *image;
		SDL_Color colour;
	};

	void cache(int ch, SDL_Color colour);
	void cache(std::string text, SDL_Color colour);

	SDL_RWops *file;
	TTF_Font *font;
	std::multimap<int, Glyph *> glyphs;
};

EXPORT void load_fonts() throw (Error);
EXPORT void release_fonts();
EXPORT void init_font() throw (Error);
EXPORT void shutdown_font();

EXPORT extern Font *font;
EXPORT extern Font *bold_font;

#endif // FONT_H
#ifndef FONT_H
#define FONT_H

#include "starsquatters.h"
#include "error.h"

class Image;

class Font {
public:
	Font(std::string filename, int size) throw (Error);
	~Font();

	void clear_cache();

	int get_width(std::string text);

	void draw(std::string text, float x, float y, SDL_Color colour);
	// Returns number of characters drawn
	int draw_wrapped(std::string text, float x, float y, int w, int line_height, int max_lines, SDL_Color colour);

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

bool init_font();
void shutdown_font();

#endif // FONT_H
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

	void draw(std::string text, float x, float y, SDL_Color color);

private:
	struct Glyph {
		Image *image;
		SDL_Color color;
	};

	void cache(int ch, SDL_Color color);
	void cache(std::string text, SDL_Color color);

	SDL_RWops *file;
	TTF_Font *font;
	std::multimap<int, Glyph *> glyphs;
};

bool init_font();
void shutdown_font();

#endif // FONT_H
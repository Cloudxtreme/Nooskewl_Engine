#include "starsquatters.h"
#include "font.h"
#include "image.h"
#include "log.h"
#include "util.h"

Font::Font() :
	font(NULL)
{
}

Font::~Font()
{
	if (font) {
		TTF_CloseFont(font);
	}
	SDL_RWclose(file);
}

void Font::load_ttf(std::string filename, int size)
{
	file = open_file(filename);
	font = TTF_OpenFontRW(file, true, size);
	if (font == NULL) {
		SDL_RWclose(file);
		throw LoadError("TTF_OpenFontRW failed");
	}
}

void Font::clear_cache()
{
	std::multimap<int, Glyph *>::iterator it = glyphs.begin();
	while (it != glyphs.end()) {
		std::pair<int, Glyph *> p = *it;
		Glyph *g = p.second;
		delete g->image;
		delete g;
		it = glyphs.erase(it);
	}
}

void Font::draw(std::string text, float x, float y, SDL_Color color)
{
	cache(text, color);
	const char *p = text.c_str();
	while (*p) {
		std::pair<std::multimap<int, Glyph *>::const_iterator, std::multimap<int, Glyph *>::const_iterator> matches = glyphs.equal_range(*p);
		std::multimap<int, Glyph *>::const_iterator it = matches.first;
		Glyph *found = NULL;
		while (it != matches.second) {
			const std::pair<int, Glyph *> pair = *it;
			Glyph *g = pair.second;
			if (memcmp(&g->color, &color, sizeof(SDL_Color)) == 0) {
				found = g;
				break;
			}
			it++;
		}
		if (found != NULL) {
			found->image->start();
			/* When we upload the glyph, it's right-side up, but OpenGL
			 * expects it upside-down. It's quicker to just flip it
			 * like this than to flip all the pixels.
			 */
			found->image->draw(x, y, Image::FLIP_V);
			found->image->end();
			x += found->image->w;
		}

		p++;
	}
}

void Font::cache(int ch, SDL_Color color)
{
	Glyph *g = new Glyph;
	if (g == NULL) {
		errormsg("Error caching glyph");
		return;
	}

	SDL_Surface *surface = TTF_RenderGlyph_Solid(font, ch, color);
	if (surface == NULL) {
		errormsg("Error rendering glyph");
		delete g;
		return;
	}

	g->image = new Image();
	try {
		g->image->from_surface(surface);
	}
	catch (Error e) {
		SDL_FreeSurface(surface);
		delete g;
		throw e;
	}

	g->color = color;

	std::pair<int, Glyph *> p;
	p.first = ch;
	p.second = g;

	glyphs.insert(p);
}

void Font::cache(std::string text, SDL_Color color)
{
	const char *p = text.c_str();
	while (*p) {
		std::pair<std::multimap<int, Glyph *>::const_iterator, std::multimap<int, Glyph *>::const_iterator> matches = glyphs.equal_range(*p);
		std::multimap<int, Glyph *>::const_iterator it = matches.first;
		bool found = false;
		while (it != matches.second) {
			const std::pair<int, Glyph *> g = *it;
			if (memcmp(&g.second->color, &color, sizeof(SDL_Color)) == 0) {
					found = true;
				break;
			}
			it++;
		}
		if (found == false) {
			cache(*p, color);
		}
		p++;
	}
}

bool init_font()
{
	if (TTF_Init() == -1) {
		return false;
	}

	return true;
}

void shutdown_font()
{
	TTF_Quit();
}
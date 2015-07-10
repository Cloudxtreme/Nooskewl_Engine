#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/global.h"
#include "Nooskewl_Engine/graphics.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/log.h"
#include "Nooskewl_Engine/util.h"

Font::Font(std::string filename, int size)
{
	filename = "fonts/" + filename;
	file = open_file(filename);
	font = TTF_OpenFontRW(file, true, size);
	if (font == NULL) {
		SDL_RWclose(file);
		throw LoadError("TTF_OpenFontRW failed");
	}
}

Font::~Font()
{
	if (font) {
		TTF_CloseFont(font);
	}
	SDL_RWclose(file);
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

int Font::get_width(std::string text)
{
	cache(text, g.graphics.white);
	const char *p = text.c_str();
	int width = 0;
	while (*p) {
		std::pair<std::multimap<int, Glyph *>::const_iterator, std::multimap<int, Glyph *>::const_iterator> matches = glyphs.equal_range(*p);
		const std::pair<int, Glyph *> pair = *matches.first;
		Glyph *g = pair.second;
		width += g->image->w;
		p++;
	}
	return width;
}

void Font::draw(std::string text, Point<int> dest_position, SDL_Color colour)
{
	cache(text, colour);
	const char *p = text.c_str();
	while (*p) {
		std::pair<std::multimap<int, Glyph *>::const_iterator, std::multimap<int, Glyph *>::const_iterator> matches = glyphs.equal_range(*p);
		std::multimap<int, Glyph *>::const_iterator it = matches.first;
		Glyph *found = NULL;
		while (it != matches.second) {
			const std::pair<int, Glyph *> pair = *it;
			Glyph *g = pair.second;
			if (memcmp(&g->colour, &colour, sizeof(SDL_Color)) == 0) {
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
			found->image->draw(dest_position, Image::FLIP_V);
			found->image->end();
			dest_position.x += found->image->w;
		}

		p++;
	}
}

int Font::draw_wrapped(std::string text, Point<int> dest_position, int w, int line_height, int max_lines, int started_time, SDL_Colour colour, bool &full)
{
	full = false;
	const char *p = text.c_str();
	char buf[2] = { 0 };
	int curr_y = dest_position.y;
	bool done = false;
	int lines = 0;
	if (max_lines == -1) {
		max_lines = 1000000;
	}
	Uint32 elapsed;
	if (started_time < 0) {
		elapsed = 1000000;
	}
	else {
		elapsed = SDL_GetTicks() - started_time;
	}
	int chars_to_draw = elapsed / CHAR_DELAY;
	int chars_drawn = 0;
	while (done == false && lines < max_lines) {
		int count = 0;
		int max = 0;
		int this_w = 0;
		int chars_drawn_this_time = 0;
		while (p[count]) {
			buf[0] = p[count];
			this_w += get_width(buf);
			if (this_w >= w) {
				if (count == 0) {
					done = true;
				}
				else {
					if (this_w > w) {
						count--;
					}
				}
				break;
			}
			if (p[count] == ' ') {
				max = count;
			}
			count++;
			if (chars_drawn+count < chars_to_draw) {
				chars_drawn_this_time++;
			}
		}
		if (p[count] == 0) {
			max = count;
		}
		int old_max = max;
		max = MIN(chars_drawn_this_time, max);
		if (done == false) {
			std::string s = std::string(p).substr(0, max);
			draw(s, Point<int>(dest_position.x, curr_y), colour);
			p += max;
			if (*p == ' ') p++;
			chars_drawn = p - text.c_str();
			curr_y += line_height;
			if (max < old_max) {
				done = true;
			}
			else {
				lines++;
				if (lines >= max_lines) {
					full = true;
				}
			}
		}
		if (*p == 0) {
			done = true;
			full = true;
		}
		if (chars_drawn >= chars_to_draw) {
			done = true;
		}
	}

	return chars_drawn;
}

void Font::cache(int ch, SDL_Color colour)
{
	Glyph *g = new Glyph;
	if (g == NULL) {
		errormsg("Error caching glyph");
		return;
	}

	SDL_Surface *surface = TTF_RenderGlyph_Solid(font, ch, colour);
	if (surface == NULL) {
		errormsg("Error rendering glyph");
		delete g;
		return;
	}

	try {
		g->image = new Image(surface);
	}
	catch (Error e) {
		SDL_FreeSurface(surface);
		delete g;
		throw e;
	}

	g->colour = colour;

	std::pair<int, Glyph *> p;
	p.first = ch;
	p.second = g;

	glyphs.insert(p);
}

void Font::cache(std::string text, SDL_Color colour)
{
	const char *p = text.c_str();
	while (*p) {
		std::pair<std::multimap<int, Glyph *>::const_iterator, std::multimap<int, Glyph *>::const_iterator> matches = glyphs.equal_range(*p);
		std::multimap<int, Glyph *>::const_iterator it = matches.first;
		bool found = false;
		while (it != matches.second) {
			const std::pair<int, Glyph *> g = *it;
			if (memcmp(&g.second->colour, &colour, sizeof(SDL_Color)) == 0) {
					found = true;
				break;
			}
			it++;
		}
		if (found == false) {
			cache(*p, colour);
		}
		p++;
	}
}

void load_fonts()
{
	g.graphics.font = new Font("fff_majestica.ttf", 8);
	g.graphics.bold_font = new Font("fff_majestica_bold.ttf", 8);
}

void release_fonts()
{
	delete g.graphics.font;
	delete g.graphics.bold_font;
}

void init_font()
{
	if (TTF_Init() == -1) {
		throw Error("TTF_Init failed");
	}

	load_fonts();
}

void shutdown_font()
{
	release_fonts();

	TTF_Quit();
}

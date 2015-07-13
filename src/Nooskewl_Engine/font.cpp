#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"

using namespace Nooskewl_Engine;

Font::Font(std::string filename, int size) :
	height(size)
{
	filename = "fonts/" + filename;

	file = open_file(filename);

	font = TTF_OpenFontRW(file, true, size);

	if (font == 0) {
		SDL_RWclose(file);
		throw LoadError("TTF_OpenFontRW failed");
	}
}

Font::~Font()
{
	clear_cache();

	if (font) {
		TTF_CloseFont(font);
	}
}

void Font::clear_cache()
{
	std::map<int, Image *>::iterator it;
	for  (it = glyphs.begin(); it != glyphs.end(); it++) {
		std::pair<int, Image *> p = *it;
		delete p.second;
	}
	glyphs.clear();
}

int Font::get_text_width(std::string text)
{
	cache_glyphs_if_needed(text);

	const char *p = text.c_str();
	int width = 0;

	while (*p) {
		Image *g = glyphs[*p];
		width += g->w;
		p++;
	}

	return width;
}

int Font::get_height()
{
	return height;
}

int Font::get_descent()
{
	return TTF_FontDescent(font);
}

void Font::enable_shadow(SDL_Colour shadow_colour, Shadow_Type shadow_type)
{
	this->shadow_colour = shadow_colour,
	this->shadow_type = shadow_type;
}

void Font::disable_shadow()
{
	this->shadow_type = NO_SHADOW;
}

void Font::draw(SDL_Colour colour, std::string text, Point<int> dest_position)
{
	cache_glyphs_if_needed(text);

	const char *p = text.c_str();

	while (*p) {
		Image *g = glyphs[*p];

		/* Glyphs are rendered upside down, so we FLIP_V them rather than flip the memory which would be slow */

		g->start();
		if (shadow_type == DROP_SHADOW) {
			g->draw_tinted(shadow_colour, dest_position+Point<int>(1, 0), Image::FLIP_V);
			g->draw_tinted(shadow_colour, dest_position+Point<int>(0, 1), Image::FLIP_V);
			g->draw_tinted(shadow_colour, dest_position+1, Image::FLIP_V);
		}
		else if (shadow_type == FULL_SHADOW) {
			g->draw_tinted(shadow_colour, dest_position+Point<int>(-1, -1), Image::FLIP_V);
			g->draw_tinted(shadow_colour, dest_position+Point<int>(0, -1), Image::FLIP_V);
			g->draw_tinted(shadow_colour, dest_position+Point<int>(1, -1), Image::FLIP_V);
			g->draw_tinted(shadow_colour, dest_position+Point<int>(-1, 0), Image::FLIP_V);
			g->draw_tinted(shadow_colour, dest_position+Point<int>(1, 0), Image::FLIP_V);
			g->draw_tinted(shadow_colour, dest_position+Point<int>(-1, 1), Image::FLIP_V);
			g->draw_tinted(shadow_colour, dest_position+Point<int>(0, 1), Image::FLIP_V);
			g->draw_tinted(shadow_colour, dest_position+Point<int>(1, 1), Image::FLIP_V);
		}
		g->draw_tinted(colour, dest_position, Image::FLIP_V);
		g->end();

		dest_position.x += g->w;

		p++;
	}
}

int Font::draw_wrapped(SDL_Colour colour, std::string text, Point<int> dest_position, int w, int line_height, int max_lines, int started_time, int delay, bool &full)
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
	int chars_to_draw;
	if (delay == 0) {
		chars_to_draw = 1000000;
	}
	else {
		chars_to_draw = elapsed / delay;
	}
	int chars_drawn = 0;
	while (done == false && lines < max_lines) {
		int count = 0;
		int max = 0;
		int this_w = 0;
		int chars_drawn_this_time = 0;
		while (p[count]) {
			buf[0] = p[count];
			this_w += get_text_width(buf);
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
			draw(colour, s, Point<int>(dest_position.x, curr_y));
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

void Font::cache_glyph(int ch)
{
	SDL_Surface *surface = TTF_RenderGlyph_Solid(font, ch, noo.white);
	if (surface == 0) {
		errormsg("Error rendering glyph");
		return;
	}

	Image *g = new Image(surface);

	SDL_FreeSurface(surface);

	glyphs[ch] = g;
}

void Font::cache_glyphs_if_needed(std::string text)
{
	const char *p = text.c_str();
	while (*p) {
		if (glyphs.find(*p) == glyphs.end()) {
			cache_glyph(*p);
		}
		p++;
	}
}

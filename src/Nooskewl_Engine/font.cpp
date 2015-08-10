#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/error.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/vertex_cache.h"
#include "Nooskewl_Engine/utf8.h"

// FIXME: define font oddities in xml file in data (such as wrong line heights etc)

using namespace Nooskewl_Engine;

Font::Font(std::string filename, int size) :
	size(size)
{
	filename = "fonts/" + filename;

	file = open_file(filename);

	font = TTF_OpenFontRW(file, true, int(size * noo.scale / noo.font_scale));

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
	std::map<Uint32, Image *>::iterator it;
	for  (it = glyphs.begin(); it != glyphs.end(); it++) {
		std::pair<int, Image *> p = *it;
		delete p.second;
	}
	glyphs.clear();
}

float Font::get_text_width(std::string text)
{
	cache_glyphs(text);

	int width = 0;
	int offset = 0;
	int ch;

	while ((ch = utf8_char_next(text, offset)) != 0) {
		Image *g = glyphs[ch];
		width += g->size.w;
	}

	return width / noo.scale * noo.font_scale;
}

float Font::get_height()
{
	return size - 3.0f;
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

void Font::draw(SDL_Colour colour, std::string text, Point<float> dest_position)
{
	cache_glyphs(text);

	dest_position.x = dest_position.x * noo.scale / noo.font_scale;
	dest_position.y = (dest_position.y - size + 1) * noo.scale / noo.font_scale;

	Point<float> pos = dest_position;

	int offset = 0;
	int ch;

	m.vertex_cache->enable_font_scaling(true);

	// Optionally draw a shadow
	if (shadow_type != NO_SHADOW) {
		noo.enable_depth_buffer(true);
		noo.clear_depth_buffer(1.0f);

		while ((ch = utf8_char_next(text, offset)) != 0) {
			Image *g = glyphs[ch];

			g->start();

			if (shadow_type == DROP_SHADOW) {
				g->draw_tinted(shadow_colour, Point<float>(pos.x+noo.scale/2.0f, pos.y), Image::FLIP_V);
				g->draw_tinted(shadow_colour, Point<float>(pos.x, pos.y+noo.scale/2.0f), Image::FLIP_V);
				g->draw_tinted(shadow_colour, Point<float>(pos.x+noo.scale/2.0f, pos.y+noo.scale/2.0f), Image::FLIP_V);
			}
			else if (shadow_type == FULL_SHADOW) {
				g->draw_tinted(shadow_colour, pos+Point<float>(-2.0f, -2.0f), Image::FLIP_V);
				g->draw_tinted(shadow_colour, pos+Point<float>(0.0f, -2.0f), Image::FLIP_V);
				g->draw_tinted(shadow_colour, pos+Point<float>(2.0f, -2.0f), Image::FLIP_V);
				g->draw_tinted(shadow_colour, pos+Point<float>(-2.0f, 0.0f), Image::FLIP_V);
				g->draw_tinted(shadow_colour, pos+Point<float>(2.0f, 0.0f), Image::FLIP_V);
				g->draw_tinted(shadow_colour, pos+Point<float>(-2.0f, 2.0f), Image::FLIP_V);
				g->draw_tinted(shadow_colour, pos+Point<float>(0.0f, 2.0f), Image::FLIP_V);
				g->draw_tinted(shadow_colour, pos+Point<float>(2.0f, 2.0f), Image::FLIP_V);
			}
			g->end();

			pos.x += g->size.w;
		}

		noo.enable_depth_buffer(false);
	}

	pos.x = dest_position.x;
	offset = 0;

	while ((ch = utf8_char_next(text, offset)) != 0) {
		Image *g = glyphs[ch];

		/* Glyphs are rendered upside down, so we FLIP_V them rather than flip the memory which would be slow */

		g->start();
		g->draw_tinted(colour, pos, Image::FLIP_V);
		g->end();

		pos.x += g->size.w;
	}

	m.vertex_cache->enable_font_scaling(false);
}

int Font::draw_wrapped(SDL_Colour colour, std::string text, Point<float> dest_position, int w, int line_height, int max_lines, int started_time, int delay, bool dry_run, bool &full, int &num_lines, int &width)
{
	full = false;
	char buf[2] = { 0 };
	float curr_y = dest_position.y;
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
	float max_width = 0.0f;
	std::string p = text;
	int total_position = 0;
	while (done == false && lines < max_lines) {
		int count = 0;
		int max = 0;
		float this_w = 0.0f;
		int chars_drawn_this_time = 0;
		Uint32 ch = utf8_char(p, count);
		while (ch) {
			cache_glyph(ch);
			Image *g = glyphs[ch];
			this_w += g->size.w / noo.scale * noo.font_scale;
			if (this_w >= (float)w) {
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
			if (ch == ' ') {
				max = count;
			}
			count++;
			ch =  utf8_char(p, count);
			if (chars_drawn+count < chars_to_draw) {
				chars_drawn_this_time++;
			}
		}
		if (utf8_char(p, count) == 0) {
			max = count;
		}
		int old_max = max;
		max = MIN(chars_drawn_this_time, max);
		if (done == false) {
			std::string s = utf8_substr(p, 0, max);
			float line_w = (float)get_text_width(s);
			if (line_w > max_width) {
				max_width = line_w;
			}
			if (dry_run == false) {
				draw(colour, s, Point<float>(dest_position.x, curr_y));
			}
			total_position += max;
			p = utf8_substr(text, total_position);
			Uint32 ch = utf8_char(p, 0);
			if (ch == ' ') {
				total_position++;
				p = utf8_substr(text, total_position);
			}
			chars_drawn = total_position;
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
		if (utf8_char(p, 0) == 0) {
			done = true;
			full = true;
		}
		if (chars_drawn >= chars_to_draw) {
			done = true;
		}
	}

	width = (int)max_width;
	num_lines = lines;

	return chars_drawn;
}

void Font::cache_glyph(Uint32 ch)
{
	if (glyphs.find(ch) != glyphs.end()) {
		return;
	}

	std::string s = utf8_char_to_string(ch);
	SDL_Surface *surface = TTF_RenderUTF8_Solid(font, s.c_str(), noo.white);
	if (surface == 0) {
		errormsg("Error rendering glyph\n");
		return;
	}

	Image *g = new Image(surface);

	SDL_FreeSurface(surface);

	glyphs[ch] = g;
}

void Font::cache_glyphs(std::string text)
{
	int offset = 0;
	int ch;
	while ((ch = utf8_char_next(text, offset)) != 0) {
		cache_glyph(ch);
	}
}

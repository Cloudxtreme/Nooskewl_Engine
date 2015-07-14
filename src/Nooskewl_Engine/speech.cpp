#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/speech.h"

using namespace Nooskewl_Engine;

Speech::Speech(std::string text) :
	text(text),
	offset(0),
	advance(false),
	done(false)
{
	size_t pipe = text.find('|');
	if (pipe != std::string::npos) {
		offset = pipe+1;
		name = text.substr(0, pipe);
	}
}

void Speech::start()
{
	start_time = SDL_GetTicks();
}

bool Speech::handle_event(TGUI_Event *event)
{
	if ((event->type == TGUI_KEY_DOWN && (event->keyboard.code == TGUIK_RETURN || event->keyboard.code == TGUIK_SPACE)) || event->type == TGUI_JOY_DOWN || event->type == TGUI_MOUSE_DOWN) {
		if (done) {
			return false;
		}
		else if (advance) {
			offset += skip;
			advance = false;
			start_time = SDL_GetTicks();
		}
		else {
			start_time = -1000000;
		}
	}
	return true;
}

void Speech::draw()
{
	int deco_size = noo.window_image->w / 3;
	int inner_pad = deco_size;
	int pad = 0;
	int line_height = noo.font->get_height() + 3;
	int win_w = noo.screen_w - pad * 2;
	int win_h = line_height * 3 + (deco_size + inner_pad) * 2;
	int win_x = pad;
	int win_y = noo.screen_h - pad - win_h;
	pad = deco_size + inner_pad;

	bool full;
	int num_lines, width;
	int drawn = noo.font->draw_wrapped(noo.white, text.substr(offset), Point<int>(win_x + pad, win_y + pad + 2), win_w - pad * 2, line_height, 3, start_time, TEXT_DELAY, true, full, num_lines, width);

	if (full) {
		if (unsigned(offset+drawn) >= text.length()) {
			done = true;
		}
		else {
			advance = true;
			skip = drawn;
		}
	}

	if (name != "") {
		int name_len = noo.font->get_text_width(name);
		int name_w = name_len + (deco_size + inner_pad) * 2;
		int name_h = noo.font->get_height() + (deco_size + inner_pad) * 2;
		int name_x = win_x;
		int name_y = win_y - name_h;
		noo.draw_window(Point<int>(name_x, name_y), Size<int>(name_w, name_h), false, false);
		noo.font->draw(noo.white, name, Point<int>(name_x+pad, name_y+pad));
	}
	noo.draw_window(Point<int>(win_x, win_y), Size<int>(win_w, win_h), !done, done);

	noo.font->draw_wrapped(noo.white, text.substr(offset), Point<int>(win_x + pad, win_y + pad), win_w - pad * 2, line_height, 3, start_time, TEXT_DELAY, false, full, num_lines, width);
}
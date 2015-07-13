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
	noo.draw_window(Point<int>(5, noo.screen_h/2), Size<int>(noo.screen_w-10, noo.screen_h/2-5), advance, done);

	noo.font->enable_shadow(noo.black, Font::DROP_SHADOW);

	bool full;
	int num_lines, width;
	int drawn = noo.font->draw_wrapped(noo.white, text.substr(offset), Point<int>(14, noo.screen_h/2+9), noo.screen_w-28, 14, 4, start_time, TEXT_DELAY, false, full, num_lines, width);

	noo.font->disable_shadow();

	if (full) {
		if (unsigned(offset+drawn) >= text.length()) {
			done = true;
		}
		else {
			advance = true;
			skip = drawn;
		}
	}
}
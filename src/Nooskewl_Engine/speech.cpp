#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/global.h"
#include "Nooskewl_Engine/graphics.h"
#include "Nooskewl_Engine/speech.h"
#include "Nooskewl_Engine/video.h"

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
	if (event->type == TGUI_KEY_DOWN || event->type == TGUI_JOY_DOWN || event->type == TGUI_MOUSE_DOWN) {
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
	draw_window(Point<int>(5, g.graphics.screen_h/2), Size<int>(g.graphics.screen_w-10, g.graphics.screen_h/2-5), advance, done);

	bool full;
	int drawn = g.graphics.font->draw_wrapped(text.substr(offset), Point<int>(14, g.graphics.screen_h/2+9), g.graphics.screen_w-28, 14, 4, start_time, g.graphics.white, full);

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
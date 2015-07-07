#include "font.h"
#include "graphics.h"
#include "speech.h"
#include "video.h"

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

bool Speech::handle_event(SDL_Event *event)
{
	if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_RETURN) {
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
	draw_window(Point<int>(5, screen_h/2), Size<int>(screen_w-10, screen_h/2-5), advance, done);

	bool full;
	int drawn = font->draw_wrapped(text.substr(offset), Point<int>(14, screen_h/2+9), screen_w-28, 14, 4, start_time, white, full);

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
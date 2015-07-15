#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/speech.h"

using namespace Nooskewl_Engine;

Sprite *Speech::speech_advance;

void Speech::static_start()
{
	speech_advance = new Sprite("speech_advance");
	speech_advance->start();
}

void Speech::static_end()
{
	delete speech_advance;
}

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

Speech::~Speech()
{
}

void Speech::start()
{
	start_time = SDL_GetTicks();
}

bool Speech::handle_event(TGUI_Event *event)
{
	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == noo.key_b1) || (event->type == TGUI_JOY_DOWN && event->joystick.button == noo.joy_b1) || (event->type == TGUI_MOUSE_DOWN && event->mouse.button == 1)) {
		noo.button_mml->play(false);
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
	noo.font->enable_shadow(noo.shadow_colour, Font::DROP_SHADOW);

	int margin = 3;
	int pad = noo.window_image->size.w / 3;
	int bottom_padding = 3; // for the advance icon
	int line_height = noo.font->get_height();
	int win_w = noo.screen_size.w - margin * 2;;
	int win_h = line_height * 3 + pad * 2 + bottom_padding ;
	int win_x = margin;
	int win_y = noo.screen_size.h - win_h - margin;

	bool full;
	int num_lines, width;
	// NOTE: keep bottom line in sync with this
	int drawn = noo.font->draw_wrapped(noo.white, text.substr(offset), Point<int>(win_x + pad + noo.font->get_padding(), win_y + pad), win_w - pad * 2 - noo.font->get_padding() * 2, line_height, 3, start_time, TEXT_DELAY, true, full, num_lines, width);

	if (full) {
		if (unsigned(offset+drawn) >= text.length()) {
			done = true;
		}
		else {
			advance = true;
			skip = drawn;
		}
	}

	noo.draw_9patch(name == "" ? noo.window_image : noo.window_image_with_name, Point<int>(win_x, win_y), Size<int>(win_w, win_h));

	if (name != "") {
		int name_len = noo.font->get_text_width(name);
		int name_w = name_len + pad * 2 + noo.font->get_padding() * 2;
		int name_h = noo.font->get_height() + pad * 2; // cover the top line of the window
		int name_x = win_x;
		int name_y = win_y - name_h + pad;
		noo.draw_9patch(noo.name_box_image, Point<int>(name_x, name_y), Size<int>(name_w, name_h));
		noo.font->draw(noo.white, name, Point<int>(name_x+pad+noo.font->get_padding(), name_y+pad));
	}

	drawn = noo.font->draw_wrapped(noo.white, text.substr(offset), Point<int>(win_x + pad + noo.font->get_padding(), win_y + pad), win_w - pad * 2 - noo.font->get_padding() * 2, line_height, 3, start_time, TEXT_DELAY, false, full, num_lines, width);

	if (done) {
		speech_advance->set_animation("done");
	}
	else {
		speech_advance->set_animation("more");
	}
	Image *image = speech_advance->get_current_image();
	image->draw_single(Point<int>(win_x+win_w-image->size.w-2, win_y+win_h-image->size.h-2));

	noo.font->disable_shadow();
}
#include "Nooskewl_Engine/basic_types.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/speech.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/tokenizer.h"

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

Speech::Speech(std::string text, Callback callback, void *callback_data) :
	text(text),
	offset(0),
	advance(false),
	done(false),
	top(false),
	right(false),
	callback(callback),
	callback_data(callback_data)
{
	size_t pipe = this->text.find('|');
	if (pipe != std::string::npos) {
		std::string options = this->text.substr(0, pipe);
		Tokenizer t(options, ',');
		std::string s;
		while ((s = t.next()) != "") {
			token(s);
		}
		this->text = this->text.substr(pipe + 1);
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
	if ((event->type == TGUI_KEY_DOWN && (event->keyboard.code == noo.key_b1 || event->keyboard.code == TGUIK_RETURN)) || (event->type == TGUI_JOY_DOWN && event->joystick.button == noo.joy_b1) || (event->type == TGUI_MOUSE_DOWN && event->mouse.button == 1)) {
		noo.button_mml->play(false);
		if (done) {
			if (callback != 0) {
				callback(callback_data);
				callback = 0;
			}
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

	int margin = 2;
	int pad = noo.window_image->size.h / 3;
	int extra_per_line = 3;
	int bottom_padding = speech_advance->get_current_image()->size.h - extra_per_line;
	int line_height = noo.font->get_height() + extra_per_line;
	int win_w = noo.screen_size.w - margin * 2;;
	int win_h = line_height * 3 + pad * 2 + bottom_padding;
	int win_x = margin;
	int win_y = top ? margin : noo.screen_size.h - win_h - margin;

	bool full;
	int num_lines, width;
	// NOTE: keep bottom line in sync with this
	int drawn = noo.font->draw_wrapped(noo.white, text.substr(offset), Point<int>(win_x + pad, win_y + pad), win_w - pad * 2, line_height, 3, start_time, TEXT_DELAY, true, full, num_lines, width);

	if (full) {
		if (unsigned(offset+drawn) >= text.length()) {
			done = true;
		}
		else {
			advance = true;
			skip = drawn;
		}
	}

	noo.draw_9patch(noo.window_image, Point<int>(win_x, win_y), Size<int>(win_w, win_h));

	if (name != "") {
		int name_len = noo.font->get_text_width(name);
		int name_w = name_len + pad * 2;
		int name_h = line_height + pad * 2; // cover the top line of the window
		int name_x = right ? win_x + win_w - name_w : win_x;
		int name_y = top ? win_y + win_h -noo.window_image->size.w/3 : win_y - name_h + noo.window_image->size.w / 3;
		noo.draw_9patch(top ? (right ? noo.name_box_image_bottom_right : noo.name_box_image_bottom) : (right ? noo.name_box_image_top_right : noo.name_box_image_top), Point<int>(name_x, name_y), Size<int>(name_w, name_h));
		noo.font->draw(noo.white, name, Point<int>(name_x+pad, name_y+pad+(top ? pad-1 : 0)));
	}

	drawn = noo.font->draw_wrapped(noo.white, text.substr(offset), Point<int>(win_x + pad, win_y + pad), win_w - pad * 2, line_height, 3, start_time, TEXT_DELAY, false, full, num_lines, width);

	if (done) {
		speech_advance->set_animation("done");
	}
	else {
		speech_advance->set_animation("more");
	}
	Image *image = speech_advance->get_current_image();
	image->draw_single(Point<int>(right ? win_x+2 : win_x+win_w-image->size.w-2, win_y+win_h-image->size.h-1));

	noo.font->disable_shadow();
}

void Speech::token(std::string s)
{
	if (s.substr(0, 5) == "name=") {
		name = s.substr(5);
	}
	else if (s.substr(0, 11) == "+milestone=") {
		int num = noo.milestone_name_to_number(s.substr(11));
		if (num >= 0) {
			noo.set_milestone(num, true);
		}
	}
	else if (s.substr(0, 11) == "-milestone=") {
		int num = noo.milestone_name_to_number(s.substr(11));
		if (num >= 0) {
			noo.set_milestone(num, false);
		}
	}
	else if (s == "top") {
		top = true;
	}
	else if (s == "right") {
		right = true;
	}
}

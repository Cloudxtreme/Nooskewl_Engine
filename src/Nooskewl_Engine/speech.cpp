#include "Nooskewl_Engine/basic_types.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/gui.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/speech.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/tokenizer.h"

using namespace Nooskewl_Engine;

void Speech::multiple_choice_callback(void *data)
{
	Multiple_Choice_GUI::Callback_Data *d = static_cast<Multiple_Choice_GUI::Callback_Data *>(data);

	Speech::Multiple_Choice_Data *mc_data = static_cast<Speech::Multiple_Choice_Data *>(d->userdata);

	noo.map->add_speech(mc_data->paths[d->choice]);

	for (size_t i = 0; i < mc_data->paths.size(); i++) {
		if (i != d->choice) {
			delete mc_data->paths[i];
		}
	}

	if (mc_data->callback != 0) {
		Callback_Data cbd;
		cbd.was_multiple_choice = true;
		cbd.choice = d->choice;
		cbd.userdata = mc_data->callback_data;

		mc_data->callback(&cbd);
	}

	delete mc_data;
}

Sprite *Speech::speech_advance;

void Speech::static_start()
{
	speech_advance = new Sprite("speech_advance");
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
	callback_data(callback_data),
	milestone(-1),
	multiple_choice_data(0)
{
	Tokenizer t(this->text, '>');
	std::vector<std::string> parts;
	std::string part;
	while ((part = t.next()) != "") {
		parts.push_back(part);
	}

	if (parts.size() > 1) {
		this->text = parts[0];

		multiple_choice_data = new Multiple_Choice_Data;

		multiple_choice_data->callback = callback;
		multiple_choice_data->callback_data = callback_data;

		for (size_t i = 1; i < parts.size(); i++) {
			multiple_choice_data->paths.push_back(new Speech(parts[i], 0, 0));
		}
	}

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
			if (milestone >= 0) {
				noo.set_milestone(milestone, milestone_on_off);
			}
			if (multiple_choice_data != 0) {
				Multiple_Choice_GUI *gui = new Multiple_Choice_GUI(multiple_choice_caption, multiple_choice_options, -1, multiple_choice_callback, multiple_choice_data);
				gui->start();
				noo.guis.push_back(gui);
			}
			else {
				if (callback != 0) {
					Callback_Data cbd;
					cbd.was_multiple_choice = false;
					cbd.userdata = callback_data;
					callback(&cbd);
					callback = 0;
				}
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
	int margin = 2;
	int pad = noo.speech_window_image->size.h / 3;
	int extra_per_line = 3;
	int bottom_padding = speech_advance->get_current_image()->size.h - extra_per_line + 1;
	int line_height = (int)noo.font->get_height() + extra_per_line;
	int win_w = noo.screen_size.w - margin * 2;;
	int win_h = line_height * 3 + pad * 2 + bottom_padding;
	int win_x = margin;
	int win_y = top ? margin : noo.screen_size.h - win_h - margin;

	bool full;
	int num_lines, width;
	// NOTE: keep bottom line in sync with this
	int drawn = noo.font->draw_wrapped(noo.black, text.substr(offset), Point<int>(win_x + pad, win_y + pad), win_w - pad * 2, line_height, 3, start_time, TEXT_DELAY, true, full, num_lines, width);

	if (full) {
		if (unsigned(offset+drawn) >= text.length()) {
			done = true;
		}
		else {
			advance = true;
			skip = drawn;
		}
	}

	noo.draw_9patch(noo.speech_window_image, Point<int>(win_x, win_y), Size<int>(win_w, win_h));

	if (name != "") {
		int name_len = (int)noo.font->get_text_width(name);
		int name_w = name_len + pad * 2;
		int name_h = line_height + pad * 2; // cover the top line of the window
		int name_x = win_x;
		int name_y = top ? win_y + win_h -noo.speech_window_image->size.w/3 : win_y - name_h + noo.speech_window_image->size.w / 3;
		noo.draw_9patch(top ? noo.name_box_image_bottom : noo.name_box_image_top, Point<int>(name_x, name_y), Size<int>(name_w, name_h));
		noo.font->draw(noo.black, name, Point<int>(name_x+pad, name_y+pad+(top ? pad-1 : 0)));
	}

	drawn = noo.font->draw_wrapped(noo.black, text.substr(offset), Point<int>(win_x + pad, win_y + pad), win_w - pad * 2, line_height, 3, start_time, TEXT_DELAY, false, full, num_lines, width);

	if (done) {
		speech_advance->set_animation("done");
	}
	else {
		speech_advance->set_animation("more");
	}
	Image *image = speech_advance->get_current_image();
	image->draw_single(Point<int>(win_x+win_w-image->size.w-2, win_y+win_h-image->size.h-2));
}

void Speech::token(std::string s)
{
	if (s.substr(0, 5) == "name=") {
		name = s.substr(5);
	}
	else if (s.substr(0, 11) == "+milestone=") {
		milestone = noo.milestone_name_to_number(s.substr(11));
		milestone_on_off = true;
	}
	else if (s.substr(0, 11) == "-milestone=") {
		milestone = noo.milestone_name_to_number(s.substr(11));
		milestone_on_off = false;
	}
	else if (s == "top") {
		top = true;
	}
	else if (s == "right") {
		right = true;
	}
	else if (s.substr(0, 11) == "mc_caption=") {
		multiple_choice_caption = s.substr(11);
	}
	else if (s.substr(0, 10) == "mc_option=") {
		multiple_choice_options.push_back(s.substr(10));
	}
}

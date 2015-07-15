#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/types.h"
#include "Nooskewl_Engine/widgets.h"

using namespace Nooskewl_Engine;

Image *MO3_Widget::focus_image;
Image *MO3_Widget::button_image;
Image *MO3_Widget::button_image_pressed;

void MO3_Widget::static_start()
{
	focus_image = new Image("gui_focus.tga");
	button_image = new Image("button.tga");
	button_image_pressed = new Image("button_pressed.tga");
}

void MO3_Widget::static_end()
{
	delete focus_image;
	delete button_image;
	delete button_image_pressed;
}

void MO3_Widget::enable_focus_shader(bool enable)
{
	static Engine::Shader bak;

	if (enable) {
		float add = (SDL_GetTicks() % 500) / 500.0f * 2.0f;
		if (add >= 1.0f) {
			add = 1.0f - (add - 1.0f);
		}
		add *= 0.5f;
		bak = noo.current_shader;
		noo.current_shader = noo.brighten_shader;
		noo.use_shader(noo.current_shader);
		noo.set_shader_float(noo.current_shader, "add", add);
	}
	else {
		noo.current_shader = bak;
		noo.use_shader(noo.current_shader);
	}
}

MO3_Widget::MO3_Widget(int w, int h) :
	TGUI_Widget(w, h)
{
}

MO3_Widget::MO3_Widget(float percent_w, float percent_h) :
	TGUI_Widget(percent_w, percent_h)
{
}

MO3_Widget::MO3_Widget(int w, float percent_h) :
	TGUI_Widget(w, percent_h)
{
}

MO3_Widget::MO3_Widget(float percent_w, int h) :
	TGUI_Widget(percent_w, h)
{
}

MO3_Widget::~MO3_Widget()
{
}

// --

MO3_Button::MO3_Button(int w, int h) :
	MO3_Widget(w, h),
	_pressed(false),
	_released(false),
	_hover(false)
{
	accepts_focus = true;
}

MO3_Button::~MO3_Button()
{
}

void MO3_Button::handle_event(TGUI_Event *event)
{
	if (event->type == TGUI_MOUSE_AXIS) {
		if (event->mouse.x >= calculated_x && event->mouse.x < calculated_x+calculated_w && event->mouse.y >= calculated_y && event->mouse.y < calculated_y+calculated_h) {
			_hover = true;
		}
		else {
			_hover = false;
		}
	}
	if (gui->get_event_owner(event) == this) {
		if (event->type == TGUI_KEY_DOWN) {
			if (event->keyboard.code == TGUIK_RETURN || event->keyboard.code == TGUIK_SPACE) {
				_pressed = true;
			}
			else {
				_pressed = false;
			}
		}
		else if (event->type == TGUI_JOY_DOWN) {
			if (event->joystick.button == noo.joy_b1) {
				_pressed = true;
			}
			else {
				_pressed = false;
			}
		}
		else if (event->type == TGUI_MOUSE_DOWN) {
			if (event->mouse.button == 1) {
				_pressed = true;
			}
			else {
				_pressed = false;
			}
		}
		else if (event->type == TGUI_KEY_UP) {
			if (_pressed && (event->keyboard.code == TGUIK_RETURN || event->keyboard.code == TGUIK_SPACE)) {
				_released = true;
			}
			else {
				_pressed = false;
			}
		}
		else if (event->type == TGUI_JOY_UP) {
			if (_pressed && (event->joystick.button == noo.joy_b1)) {
				_released = true;
			}
			else {
				_pressed = false;
			}
		}
		else if (event->type == TGUI_MOUSE_UP) {
			if (_pressed && (event->mouse.button == 1)) {
				_released = true;
			}
			else {
				_pressed = false;
			}
		}
	}
	else {
		if (event->type == TGUI_KEY_UP) {
			_pressed = false;
		}
		else if (event->type == TGUI_JOY_UP) {
			_pressed = false;
		}
		else if (event->type == TGUI_MOUSE_UP) {
			_pressed = false;
		}
	}
}

bool MO3_Button::pressed()
{
	bool r = _released;
	_released = false;
	return r;
}

// --

MO3_Text_Button::MO3_Text_Button(std::string text, Size<int> size) :
	MO3_Button(size.w, size.h),
	text(text)
{
	padding = button_image->w / 3;
	set_size(size);
	set_default_colours();
}

MO3_Text_Button::MO3_Text_Button(std::string text) :
	MO3_Button(-1, -1),
	text(text)
{
	padding = button_image->w / 3;
	set_size(Size<int>(-1, -1));
	set_default_colours();
}

MO3_Text_Button::~MO3_Text_Button()
{
}

void MO3_Text_Button::draw()
{
	if (_pressed && _hover) {
		noo.draw_9patch(button_image_pressed, Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h));
	}
	else if (gui->get_focus() == this) {
		enable_focus_shader(true);
		noo.draw_9patch(button_image, Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h));
		enable_focus_shader(false);
	}
	else {
		noo.draw_9patch(button_image, Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h));
	}
	noo.font->enable_shadow(noo.shadow_colour, Font::DROP_SHADOW);
	if (_pressed && _hover) {
		SDL_Colour colour = text_colour;
		colour.r *= 0.75f;
		colour.g *= 0.75f;
		colour.b *= 0.75f;
		noo.font->draw(colour, text, Point<int>(calculated_x+calculated_w/2-noo.font->get_text_width(text)/2, calculated_y+padding));
	}
	else {
		noo.font->draw(text_colour, text, Point<int>(calculated_x+calculated_w/2-noo.font->get_text_width(text)/2, calculated_y+padding));
	}
	noo.font->disable_shadow();
}

void MO3_Text_Button::set_default_colours()
{
	button_colour = noo.magenta;
	text_colour = noo.white;
}

void MO3_Text_Button::set_size(Size<int> size)
{
	if (size.w < 0) {
		w = noo.font->get_text_width(text) + padding * 2 + noo.font->get_padding() * 2;
	}
	if (size.h < 0) {
		h = noo.font->get_height() + padding * 2;
	}
}

// --

MO3_Window::MO3_Window(int w, int h) :
	MO3_Widget(w, h)
{
	set_default_colours();
}

MO3_Window::MO3_Window(float percent_w, float percent_h) :
	MO3_Widget(percent_w, percent_h)
{
	set_default_colours();
}

MO3_Window::MO3_Window(int w, float percent_h) :
	MO3_Widget(w, percent_h)
{
	set_default_colours();
}

MO3_Window::MO3_Window(float percent_w, int h) :
	MO3_Widget(percent_w, h)
{
	set_default_colours();
}

MO3_Window::~MO3_Window()
{
}

void MO3_Window::draw()
{
	noo.draw_9patch(noo.window_image, Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h));
}

void MO3_Window::set_default_colours()
{
	background_colour = noo.magenta;
}

// --

MO3_Label::MO3_Label(std::string text, int max_w) :
	MO3_Widget(0, 0),
	text(text),
	max_w(max_w)
{
	colour = noo.white;
	bool full;
	int num_lines, width;
	int line_height = noo.font->get_height() + 1;
	noo.font->draw_wrapped(colour, text, Point<int>(calculated_x, calculated_y), max_w, line_height, -1, -1, 0, true, full, num_lines, width);
	w = width;
	h = line_height * num_lines;
}

void MO3_Label::draw()
{
	bool full;
	int num_lines, width;
	noo.font->enable_shadow(noo.shadow_colour, Font::DROP_SHADOW);
	noo.font->draw_wrapped(colour, text, Point<int>(calculated_x, calculated_y), max_w, noo.font->get_height()+1, -1, -1, 0, false, full, num_lines, width);
	noo.font->disable_shadow();
}

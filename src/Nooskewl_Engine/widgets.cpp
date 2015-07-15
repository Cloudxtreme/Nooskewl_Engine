#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/types.h"
#include "Nooskewl_Engine/widgets.h"

using namespace Nooskewl_Engine;

Image *MO3_Widget::focus_image;
Image *MO3_Widget::button_image;

void MO3_Widget::static_start()
{
	focus_image = new Image("gui_focus.tga");
	button_image = new Image("button.tga");
}

void MO3_Widget::static_end()
{
	delete button_image;
}

void MO3_Widget::draw_focus(TGUI_Widget *widget)
{
	float f = (SDL_GetTicks() % 1000) / 1000.0f;
	float alpha;
	if (f >= 0.5f) {
		alpha = 1.0f - ((f - 0.5f) / 0.5f);
	}
	else {
		alpha = f / 0.5f;
	}
	SDL_Colour colour;
	colour.r = 255;
	colour.g = 255;
	colour.b = 255;
	colour.a = int(alpha * 255.0f);
	noo.draw_9patch_tinted(colour, focus_image, Point<int>(widget->get_x(), widget->get_y()), Size<int>(widget->get_width(), widget->get_height()));
}

MO3_Widget::MO3_Widget(int w, int h) :
	TGUI_Widget(w, h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

MO3_Widget::MO3_Widget(float percent_w, float percent_h) :
	TGUI_Widget(percent_w, percent_h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

MO3_Widget::MO3_Widget(int w, float percent_h) :
	TGUI_Widget(w, percent_h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

MO3_Widget::MO3_Widget(float percent_w, int h) :
	TGUI_Widget(percent_w, h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

MO3_Widget::~MO3_Widget()
{
}

void MO3_Widget::draw()
{
	if (gui->get_focus() == this) {
		draw_focus(this);
	}
}

void MO3_Widget::handle_event(TGUI_Event *event)
{
	TGUI_Widget *owner = gui->get_event_owner(event);
	if (owner == this) {
		got_event = true;
		if (event->type == TGUI_MOUSE_DOWN || event->type == TGUI_MOUSE_UP || event->type == TGUI_MOUSE_AXIS) {
			TGUI_Event new_event = tgui_get_relative_event(this, event);
			event_x = new_event.mouse.x;
			event_y = new_event.mouse.y;
		}
		else {
			event_x = event_y = -1;
		}
	}
	else if (owner != 0) {
		got_event = false;
		event_x = event_y = -1;
	}
}

// --

MO3_Button::MO3_Button(int w, int h) :
	MO3_Widget(w, h),
	_pressed(false)
{
	accepts_focus = true;
}

MO3_Button::~MO3_Button()
{
}

void MO3_Button::handle_event(TGUI_Event *event)
{
	if (gui->get_event_owner(event) == this) {
		if ((event->type == TGUI_KEY_DOWN && (event->keyboard.code == TGUIK_RETURN || event->keyboard.code == TGUIK_SPACE)) || (event->type == TGUI_JOY_DOWN && event->joystick.button == noo.joy_b1) || (event->type == TGUI_MOUSE_DOWN && event->mouse.button == 1)) {
			_pressed = true;
		}
	}
}

bool MO3_Button::pressed()
{
	bool p = _pressed;
	_pressed = false;
	return  p;
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
	noo.draw_9patch(button_image, Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h));
	noo.font->draw(text_colour, text, Point<int>(calculated_x+calculated_w/2-noo.font->get_text_width(text)/2, calculated_y+padding));

	MO3_Widget::draw();
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
	noo.font->draw_wrapped(colour, text, Point<int>(calculated_x, calculated_y), max_w, noo.font->get_height()+1, -1, -1, 0, false, full, num_lines, width);
}

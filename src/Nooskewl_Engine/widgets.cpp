#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/types.h"
#include "Nooskewl_Engine/widgets.h"

using namespace Nooskewl_Engine;

static void draw_focus(TGUI_Widget *widget)
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
	colour.b = 0;
	colour.a = int(alpha * 255.0f);
	int padding_left = widget->get_padding_left();
	int padding_right = widget->get_padding_right();
	int padding_top = widget->get_padding_top();
	int padding_bottom = widget->get_padding_bottom();
	int x = widget->get_x(); // - padding_left;
	int y = widget->get_y(); // - padding_top;
	int w = widget->get_width(); // + padding_left + padding_right;
	int h = widget->get_height(); // + padding_top + padding_bottom;
	noo.draw_line(Point<int>(x, y), Point<int>(x+w, y), colour);
	noo.draw_line(Point<int>(x+w, y), Point<int>(x+w, y+h), colour);
	noo.draw_line(Point<int>(x+w, y+h), Point<int>(x, y+h), colour);
	noo.draw_line(Point<int>(x, y+h), Point<int>(x, y), colour);
}

SS_Widget::SS_Widget(int w, int h) :
	TGUI_Widget(w, h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

SS_Widget::SS_Widget(float percent_w, float percent_h) :
	TGUI_Widget(percent_w, percent_h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

SS_Widget::SS_Widget(int w, float percent_h) :
	TGUI_Widget(w, percent_h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

SS_Widget::SS_Widget(float percent_w, int h) :
	TGUI_Widget(percent_w, h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

SS_Widget::~SS_Widget()
{
}

void SS_Widget::draw()
{
	if (gui->get_focus() == this) {
		draw_focus(this);
	}
}

void SS_Widget::handle_event(TGUI_Event *event)
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

SS_Button::SS_Button(int w, int h) :
	SS_Widget(w, h),
	_pressed(false)
{
}

SS_Button::~SS_Button()
{
}

void SS_Button::handle_event(TGUI_Event *event)
{
	if (gui->get_event_owner(event) == this) {
		if ((event->type == TGUI_KEY_DOWN && (event->keyboard.code == TGUIK_RETURN || event->keyboard.code == TGUIK_SPACE)) || (event->type == TGUI_JOY_DOWN && (1/*FIXME*/)) || (event->type == TGUI_MOUSE_DOWN && event->mouse.button == 1)) {
			_pressed = true;
		}
	}
}

bool SS_Button::pressed()
{
	bool p = _pressed;
	_pressed = false;
	return  p;
}

// --

SS_Text_Button::SS_Text_Button(std::string text) :
	SS_Button(0, 0),
	text(text)
{
	w = noo.font->get_text_width(text) + PAD_X * 2;
	h = noo.font->get_height() + PAD_Y * 2;
}

SS_Text_Button::~SS_Text_Button()
{
}

void SS_Text_Button::draw()
{
	SDL_Colour test = { 255, 0, 255, 255 };
	noo.draw_quad(Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h), test);
	noo.font->enable_shadow(noo.black, Font::FULL_SHADOW);
	noo.font->draw(noo.white, text, Point<int>(calculated_x+PAD_X, calculated_y+PAD_Y-2));
	noo.font->disable_shadow();

	SS_Widget::draw();
}
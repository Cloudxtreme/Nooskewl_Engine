#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/global.h"
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
	engine.draw_line(Point<int>(x, y), Point<int>(x+w, y), colour);
	engine.draw_line(Point<int>(x+w, y), Point<int>(x+w, y+h), colour);
	engine.draw_line(Point<int>(x+w, y+h), Point<int>(x, y+h), colour);
	engine.draw_line(Point<int>(x, y+h), Point<int>(x, y), colour);
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

void SS_Widget::draw()
{
	SDL_Colour green = { 0, 255, 0, 255 };
	SDL_Colour red = { 255, 0, 0, 255 };

	engine.draw_quad(Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h), g.black);
	engine.draw_quad(Point<int>(calculated_x, calculated_y)+1, Size<int>(calculated_w, calculated_h)-2, got_event ? green : g.white);

	if (event_x >= 0 || event_y >= 0) {
		engine.draw_quad(Point<int>(event_x, event_y)+Point<int>(calculated_x, calculated_y)-1, Size<int>(2, 2), red);
	}

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
	else if (owner != NULL) {
		got_event = false;
		event_x = event_y = -1;
	}
}
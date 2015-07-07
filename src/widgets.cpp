#include "graphics.h"
#include "types.h"
#include "widgets.h"

static void draw_focus(TGUI_Div *div)
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
	int padding_left = div->get_padding_left();
	int padding_right = div->get_padding_right();
	int padding_top = div->get_padding_top();
	int padding_bottom = div->get_padding_bottom();
	int x = div->get_x(); // - padding_left;
	int y = div->get_y(); // - padding_top;
	int w = div->get_width(); // + padding_left + padding_right;
	int h = div->get_height(); // + padding_top + padding_bottom;
	draw_line(Point<int>(x, y), Point<int>(x+w, y), colour);
	draw_line(Point<int>(x+w, y), Point<int>(x+w, y+h), colour);
	draw_line(Point<int>(x+w, y+h), Point<int>(x, y+h), colour);
	draw_line(Point<int>(x, y+h), Point<int>(x, y), colour);
}

SS_Div::SS_Div(int w, int h) :
	TGUI_Div(w, h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

SS_Div::SS_Div(float percent_w, float percent_h) :
	TGUI_Div(percent_w, percent_h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

SS_Div::SS_Div(int w, float percent_h) :
	TGUI_Div(w, percent_h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

SS_Div::SS_Div(float percent_w, int h) :
	TGUI_Div(percent_w, h),
	got_event(false),
	event_x(-1),
	event_y(-1)
{
}

void SS_Div::draw()
{
	SDL_Colour green = { 0, 255, 0, 255 };
	SDL_Colour red = { 255, 0, 0, 255 };

	draw_quad(Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h), black);
	draw_quad(Point<int>(calculated_x, calculated_y)+1, Size<int>(calculated_w, calculated_h)-2, got_event ? green : white);

	if (event_x >= 0 || event_y >= 0) {
		draw_quad(Point<int>(event_x, event_y)+Point<int>(calculated_x, calculated_y)-1, Size<int>(2, 2), red);
	}

	if (gui->get_focus() == this) {
		draw_focus(this);
	}
}

void SS_Div::handle_event(TGUI_Event *event)
{
	TGUI_Div *owner = gui->get_event_owner(event);
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
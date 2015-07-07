#include "graphics.h"
#include "types.h"
#include "widgets.h"

static void draw_focus(TGUI_Div *div, int x, int y)
{
	int w, h;
	tgui_get_size(div->get_parent(), div, &w, &h);
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
	draw_line(Point<int>(x, y), Point<int>(x+w, y), colour);
	draw_line(Point<int>(x+w, y), Point<int>(x+w, y+h), colour);
	draw_line(Point<int>(x+w, y+h), Point<int>(x, y+h), colour);
	draw_line(Point<int>(x, y+h), Point<int>(x, y), colour);
}

SS_Div::SS_Div(int w, int h) :
	TGUI_Div(w, h)
{
}

SS_Div::SS_Div(float percent_w, float percent_h) :
	TGUI_Div(percent_w, percent_h)
{
}

SS_Div::SS_Div(int w, float percent_h) :
	TGUI_Div(w, percent_h)
{
}

SS_Div::SS_Div(float percent_w, int h) :
	TGUI_Div(percent_w, h)
{
}

void SS_Div::draw(TGUI_Div *parent, int x, int y) {
	int width, height;
	tgui_get_size(parent, this, &width, &height);
	width -= padding_left + padding_right;
	height -= padding_top + padding_bottom;
	SDL_Colour blacks[] = {
		{ 0, 0, 0, 255 },
		{ 0, 0, 0, 255 },
		{ 0, 0, 0, 255 },
		{ 0, 0, 0, 255 }
	};
	SDL_Colour whites[] = {
		{ 255, 255, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 255, 255, 255 },
		{ 255, 255, 255, 255 }
	};
	draw_quad(Point<int>(x, y), Size<int>(width, height), blacks);
	draw_quad(Point<int>(x, y)+1, Size<int>(width, height)-2, whites);

	if (gui->get_focus() == this) {
		draw_focus(this, x-padding_left, y-padding_top);
	}
}
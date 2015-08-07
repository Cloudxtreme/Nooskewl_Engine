#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/widgets.h"

using namespace Nooskewl_Engine;

Image *Widget::button_image;
Image *Widget::button_image_pressed;

void Widget::static_start()
{
	button_image = new Image("button.tga");
	button_image_pressed = new Image("button_pressed.tga");
}

void Widget::static_end()
{
	delete button_image;
	delete button_image_pressed;
}

void Widget::enable_focus_shader(bool enable)
{
	static Shader *bak;

	if (enable) {
		float add = (SDL_GetTicks() % 500) / 500.0f * 2.0f;
		if (add >= 1.0f) {
			add = 1.0f - (add - 1.0f);
		}
		add *= 0.5f;
		bak = noo.current_shader;
		noo.current_shader = noo.brighten_shader;
		noo.current_shader->use();
		noo.current_shader->set_float("add_r", add);
		noo.current_shader->set_float("add_g", add);
		noo.current_shader->set_float("add_b", add);
	}
	else {
		noo.current_shader = bak;
		noo.current_shader->use();
	}
}

Widget::Widget(int w, int h) :
	TGUI_Widget(w, h)
{
	background_colour.a = 0;
}

Widget::Widget(float percent_w, float percent_h) :
	TGUI_Widget(percent_w, percent_h)
{
	background_colour.a = 0;
}

Widget::Widget(int w, float percent_h) :
	TGUI_Widget(w, percent_h)
{
	background_colour.a = 0;
}

Widget::Widget(float percent_w, int h) :
	TGUI_Widget(percent_w, h)
{
	background_colour.a = 0;
}

Widget::~Widget()
{
}

void Widget::draw()
{
	if (background_colour.a != 0) {
		noo.draw_quad(background_colour, Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h));
	}
}

void Widget::set_background_colour(SDL_Colour background_colour)
{
	this->background_colour = background_colour;
}

// --

Widget_Button::Widget_Button(int w, int h) :
	Widget(w, h),
	_pressed(false),
	_released(false),
	_hover(false)
{
	accepts_focus = true;
}

Widget_Button::Widget_Button(float w, float h) :
	Widget(w, h),
	_pressed(false),
	_released(false),
	_hover(false)
{
	accepts_focus = true;
}

Widget_Button::Widget_Button(int w, float h) :
	Widget(w, h),
	_pressed(false),
	_released(false),
	_hover(false)
{
	accepts_focus = true;
}

Widget_Button::Widget_Button(float w, int h) :
	Widget(w, h),
	_pressed(false),
	_released(false),
	_hover(false)
{
	accepts_focus = true;
}

Widget_Button::~Widget_Button()
{
}

void Widget_Button::handle_event(TGUI_Event *event)
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
			if (event->keyboard.code == TGUIK_RETURN || event->keyboard.code == noo.key_b1) {
				_pressed = true;
				_hover = true;
			}
			else {
				_pressed = false;
				_hover = false;
			}
		}
		else if (event->type == TGUI_JOY_DOWN) {
			if (event->joystick.button == noo.joy_b1) {
				_pressed = true;
				_hover = true;
			}
			else {
				_pressed = false;
				_hover = false;
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
			if (_pressed && (event->keyboard.code == TGUIK_RETURN || event->keyboard.code == noo.key_b1)) {
				_released = true;
				_hover = false;
				noo.button_mml->play(false);
			}
			else {
				_pressed = false;
				_hover = false;
			}
		}
		else if (event->type == TGUI_JOY_UP) {
			if (_pressed && (event->joystick.button == noo.joy_b1)) {
				_released = true;
				_hover = false;
				noo.button_mml->play(false);
			}
			else {
				_pressed = false;
				_hover = false;
			}
		}
		else if (event->type == TGUI_MOUSE_UP) {
			if (_pressed && (event->mouse.button == 1)) {
				_released = true;
				noo.button_mml->play(false);
			}
			else {
				_pressed = false;
			}
		}
	}
	else {
		if (event->type == TGUI_KEY_UP) {
			_pressed = false;
			_hover = false;
		}
		else if (event->type == TGUI_JOY_UP) {
			_pressed = false;
			_hover = false;
		}
		else if (event->type == TGUI_MOUSE_UP) {
			_pressed = false;
			_hover = false;
		}
	}
}

bool Widget_Button::pressed()
{
	bool r = _released;
	if (_released) {
		_pressed = _released = _hover = false;
	}
	return r;
}

// --

Widget_Text_Button::Widget_Text_Button(std::string text, int w, int h) :
	Widget_Button(w, h),
	text(text)
{
	padding = button_image->size.h / 3;
	set_size((float)w, (float)h);
	set_default_colours();
}

Widget_Text_Button::Widget_Text_Button(std::string text, float w, float h) :
	Widget_Button(w, h),
	text(text)
{
	padding = button_image->size.h / 3;
	set_size(w, h);
	set_default_colours();
}

Widget_Text_Button::Widget_Text_Button(std::string text, int w, float h) :
	Widget_Button(w, h),
	text(text)
{
	padding = button_image->size.h / 3;
	set_size((float)w, h);
	set_default_colours();
}

Widget_Text_Button::Widget_Text_Button(std::string text, float w, int h) :
	Widget_Button(w, h),
	text(text)
{
	padding = button_image->size.h / 3;
	set_size(w, (float)h);
	set_default_colours();
}

Widget_Text_Button::Widget_Text_Button(std::string text) :
	Widget_Button(-1, -1),
	text(text)
{
	padding = button_image->size.h / 3;
	set_size(-1.0f, -1.0f);
	set_default_colours();
}

Widget_Text_Button::~Widget_Text_Button()
{
}

void Widget_Text_Button::draw()
{
	bool focussed = gui->get_focus() == this;
	Point<int> offset(0, 0);

	if (focussed) {
		enable_focus_shader(true);
		if (_pressed && _hover) {
			offset = Point<int>(1, 1);
			noo.draw_9patch(button_image_pressed, Point<int>(calculated_x+offset.x, calculated_y+offset.y), Size<int>(calculated_w, calculated_h));
		}
		else {
			noo.draw_9patch(button_image, Point<int>(calculated_x+offset.x, calculated_y+offset.y), Size<int>(calculated_w, calculated_h));
		}
		enable_focus_shader(false);
	}
	else {
		noo.draw_9patch(button_image, Point<int>(calculated_x+offset.x, calculated_y+offset.y), Size<int>(calculated_w, calculated_h));
	}
	noo.font->enable_shadow(noo.shadow_colour, Font::DROP_SHADOW);
	if (_pressed && _hover) {
		SDL_Colour colour = text_colour;
		colour.r = Uint8(colour.r * 0.75f);
		colour.r = Uint8(colour.g * 0.75f);
		colour.r = Uint8(colour.b * 0.75f);
		noo.font->draw(colour, text, Point<int>(calculated_x+calculated_w/2-noo.font->get_text_width(text)/2+offset.x, calculated_y+padding+offset.y-1));
	}
	else {
		noo.font->draw(text_colour, text, Point<int>(calculated_x+calculated_w/2-noo.font->get_text_width(text)/2+offset.x, calculated_y+padding+offset.y-1));
	}
	noo.font->disable_shadow();
}

void Widget_Text_Button::set_default_colours()
{
	button_colour = noo.magenta;
	text_colour = noo.white;
}

void Widget_Text_Button::set_size(float width, float height)
{
	if (width < 0) {
		w = noo.font->get_text_width(text) + padding * 2 - 2;
	}
	if (height < 0) {
		h = noo.font->get_height() + padding * 2 - 1;
	}
}

// --

Widget_Window::Widget_Window(int w, int h) :
	Widget(w, h)
{
	set_default_colours();
}

Widget_Window::Widget_Window(float percent_w, float percent_h) :
	Widget(percent_w, percent_h)
{
	set_default_colours();
}

Widget_Window::Widget_Window(int w, float percent_h) :
	Widget(w, percent_h)
{
	set_default_colours();
}

Widget_Window::Widget_Window(float percent_w, int h) :
	Widget(percent_w, h)
{
	set_default_colours();
}

Widget_Window::~Widget_Window()
{
}

void Widget_Window::draw()
{
	noo.draw_9patch(noo.window_image, Point<int>(calculated_x, calculated_y), Size<int>(calculated_w, calculated_h));
}

void Widget_Window::set_default_colours()
{
	background_colour = noo.magenta;
}

// --

Widget_Label::Widget_Label(std::string text, int max_w) :
	Widget(0, 0)
{
	if (max_w < 0) {
		this->max_w = INT_MAX;
	}
	else {
		this->max_w = max_w;
	}

	colour = noo.white;

	set_text(text);
}

void Widget_Label::draw()
{
	bool full;
	int num_lines, width;
	noo.font->enable_shadow(noo.shadow_colour, Font::DROP_SHADOW);
	noo.font->draw_wrapped(colour, text, Point<int>(calculated_x, calculated_y), max_w, noo.font->get_height()+1, -1, -1, 0, false, full, num_lines, width);
	noo.font->disable_shadow();
}

void Widget_Label::set_text(std::string text)
{
	this->text = text;
	bool full;
	int num_lines, width;
	int line_height = noo.font->get_height() + 2;
	noo.font->draw_wrapped(colour, text, Point<int>(calculated_x, calculated_y), max_w, line_height, -1, -1, 0, true, full, num_lines, width);
	w = width;
	h = line_height * num_lines;
}

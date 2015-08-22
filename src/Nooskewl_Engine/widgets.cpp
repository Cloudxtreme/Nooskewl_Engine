#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/gui.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/widgets.h"

using namespace Nooskewl_Engine;

Image *Widget::button_image;
Image *Widget::button_image_pressed;
Image *Widget::slider_image;
Image *Widget::radio_image;
Image *Widget::radio_selected_image;

void Widget::static_start()
{
	button_image = new Image("button.tga");
	button_image_pressed = new Image("button_pressed.tga");
	slider_image = new Image("slider_tab.tga");
	radio_image = new Image("radio.tga");
	radio_selected_image = new Image("radio_selected.tga");
}

void Widget::static_end()
{
	delete button_image;
	delete button_image_pressed;
	delete slider_image;
	delete radio_image;
	delete radio_selected_image;
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
		if (noo_gui) {
			noo.current_shader->set_float("global_alpha", noo_gui->global_alpha);
		}
	}
	else {
		noo.current_shader = bak;
		noo.current_shader->use();
	}
}

Widget::Widget(int w, int h) :
	TGUI_Widget(w, h)
{
	init();
}

Widget::Widget(float percent_w, float percent_h) :
	TGUI_Widget(percent_w, percent_h)
{
	init();
}

Widget::Widget(int w, float percent_h) :
	TGUI_Widget(w, percent_h)
{
	init();
}

Widget::Widget(float percent_w, int h) :
	TGUI_Widget(percent_w, h)
{
	init();
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

void Widget::init()
{
	noo_gui = 0;
	background_colour.a = 0;
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
		noo.font->draw(colour, text, Point<float>(calculated_x+calculated_w/2.0f-noo.font->get_text_width(text)/2.0f+offset.x, calculated_y+padding+offset.y-1.0f));
	}
	else {
		noo.font->draw(text_colour, text, Point<float>(calculated_x+calculated_w/2.0f-noo.font->get_text_width(text)/2.0f+offset.x, calculated_y+padding+offset.y-1.0f));
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
		w = int(noo.font->get_text_width(text) + padding * 2 - 2);
	}
	if (height < 0) {
		h = int(noo.font->get_height() + padding * 2 - 1);
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
	noo.font->draw_wrapped(colour, text, Point<int>(calculated_x, calculated_y), max_w, (int)noo.font->get_height()+1, -1, -1, 0, false, full, num_lines, width);
	noo.font->disable_shadow();
}

void Widget_Label::set_text(std::string text)
{
	this->text = text;
	bool full;
	int num_lines, width;
	int line_height = (int)noo.font->get_height() + 2;
	noo.font->draw_wrapped(colour, text, Point<int>(calculated_x, calculated_y), max_w, line_height, -1, -1, 0, true, full, num_lines, width);
	w = width;
	h = line_height * num_lines;
}

void Widget_Label::set_width(int width)
{
	w = width;
}

//--

Widget_Image::Widget_Image(Image *image, bool destroy) :
	Widget(image->size.w, image->size.h),
	image(image),
	destroy(destroy)
{
}

Widget_Image::~Widget_Image()
{
	if (destroy) {
		delete image;
	}
}

void Widget_Image::draw()
{
	image->draw_single(Point<int>(calculated_x, calculated_y));
}

//--

Widget_List::Widget_List(int w, int h) :
	Widget(w, h)
{
	init();
}

Widget_List::Widget_List(float w, float h) :
	Widget(w, h)
{
	init();
}

Widget_List::Widget_List(int w, float h) :
	Widget(w, h)
{
	init();
}

Widget_List::Widget_List(float w, int h) :
	Widget(w, h)
{
	init();
}

void Widget_List::handle_event(TGUI_Event *event)
{
	bool focussed = gui->get_focus() == this;

	if (focussed && event->type == TGUI_FOCUS) {
		if (event->focus.type == TGUI_FOCUS_UP) {
			up();
		}
		else if (event->focus.type == TGUI_FOCUS_DOWN) {
			down();
		}
	}
	else if (focussed && event->type == TGUI_KEY_DOWN) {
		if (event->keyboard.code == noo.key_b1 || event->keyboard.code == TGUIK_RETURN) {
			pressed_item = selected;
			noo.button_mml->play(false);
		}
	}
	else if (focussed && event->type == TGUI_JOY_DOWN) {
		if (event->joystick.button == noo.joy_b1) {
			pressed_item = selected;
			noo.button_mml->play(false);
		}
	}
	else if (event->type == TGUI_MOUSE_DOWN) {
		int mx = (int)event->mouse.x;
		int my = (int)event->mouse.y;
		// Check for clicks on arrows  first
		bool top_arrow = top > 0;
		bool bottom_arrow;
		int vr = visible_rows();
		if ((int)items.size() > vr && top < (int)items.size() - vr) {
			bottom_arrow = true;
		}
		else {
			bottom_arrow = false;
		}
		int height = used_height();
		if (top_arrow && mx >= calculated_x+calculated_w-8 && mx <= calculated_x+calculated_w && my >= calculated_y && my <= calculated_y+8) {
			change_top(-1);
		}
		else if (bottom_arrow && mx >= calculated_x+calculated_w-8 && mx <= calculated_x+calculated_w && my >= calculated_y+height-8 && my <= calculated_y+height) {
			change_top(1);
		}
		else if (mx >= calculated_x && mx < calculated_x+calculated_w-8 && my >= calculated_y && my < calculated_y+calculated_h) {
			selected = get_click_row((int)my);
			mouse_down = true;
			clicked = true;
			mouse_down_point = Point<int>((int)mx, (int)my);
			mouse_down_row = selected;
		}
	}
	else if (event->type == TGUI_MOUSE_UP) {
		if (mouse_down && clicked && event->mouse.x >= calculated_x && event->mouse.x < calculated_x+calculated_w && event->mouse.y >= calculated_y && event->mouse.y < calculated_y+calculated_h) {
			int row = get_click_row((int)event->mouse.y);
			if (row == mouse_down_row) {
				pressed_item = selected;
				noo.button_mml->play(false);
			}
		}
		mouse_down = false;
	}
	else if (event->type == TGUI_MOUSE_AXIS) {
		if (mouse_down) {
			Point<int> p((int)event->mouse.x, (int)event->mouse.y);
			Point<int> d = p - mouse_down_point;
			if (abs(d.y) >= row_h) {
				clicked = false;
			}
			if (clicked == false) {
				int rows = -d.y / row_h;
				if (rows != 0) {
					change_top(rows);
					mouse_down_point.y -= rows * row_h;
				}
			}
		}
	}
}

void Widget_List::draw()
{
	for (int i = top; i < (int)items.size() && i < top+visible_rows(); i++) {
		noo.font->enable_shadow(noo.shadow_colour, Font::DROP_SHADOW);
		int y = calculated_y + ((i - top) * row_h);
		if (i == selected) {
			bool focussed = gui->get_focus() == this;
			if (focussed) {
				enable_focus_shader(true);
			}
			noo.draw_quad(hilight_colour, Point<int>(calculated_x, y), Size<int>(calculated_w-8, row_h));
			if (focussed) {
				enable_focus_shader(false);
			}
		}
		SDL_Colour colour;
		if  (is_hilighted(i)) {
			colour.r = 255;
			colour.g = 255;
			colour.b = 0;
			colour.a = 255;
		}
		else {
			colour = noo.white;
		}
		noo.font->draw(colour, items[i], Point<int>(calculated_x+2, y+1));
		noo.font->disable_shadow();
	}

	if  (top != 0) {
		noo.draw_triangle(noo.white, Point<float>(calculated_x+calculated_w-4.5f, (float)calculated_y), Point<float>(calculated_x+calculated_w-8.0f, calculated_y+7.0f), Point<float>(calculated_x+calculated_w-1.0f, calculated_y+7.0f));
	}
	int vr = visible_rows();
	if ((int)items.size() > vr && top < (int)items.size() - vr) {
		int height = used_height();
		noo.draw_triangle(noo.white, Point<float>(calculated_x+calculated_w-4.5f, (float)calculated_y+height), Point<float>(calculated_x+calculated_w-8.0f, calculated_y+height-7.0f), Point<float>(calculated_x+calculated_w-1.0f, calculated_y+height-7.0f));
	}
}

void Widget_List::set_items(std::vector<std::string> new_items)
{
	items.clear();
	items.insert(items.begin(), new_items.begin(), new_items.end());
	accepts_focus = items.size() != 0;

	if (items.size() == 0) {
		selected = -1;
		if (gui->get_focus() == this) {
			gui->focus_something();
		}
	}
	else {
		if (selected < 0) {
			selected = 0;
		}
	}
}

int Widget_List::pressed()
{
	int ret = pressed_item;
	pressed_item = -1;
	return ret;
}

int Widget_List::get_selected()
{
	return selected;
}

void Widget_List::set_selected(int selected)
{
	this->selected = selected;
}

void Widget_List::set_hilight(int index, bool onoff)
{
	std::vector<int>::iterator it = std::find(hilight.begin(), hilight.end(), index);

	if (onoff) {
		if (it == hilight.end()) {
			hilight.push_back(index);
		}
	}
	else {
		if (it != hilight.end()) {
			hilight.erase(it);
		}
	}
}

bool Widget_List::is_hilighted(int index)
{
	return std::find(hilight.begin(), hilight.end(), index) != hilight.end();
}

void Widget_List::init()
{
	accepts_focus = true;
	top = 0;
	selected = -1;
	row_h = (int)noo.font->get_height() + 3;

	hilight_colour.r = 62;
	hilight_colour.g = 140;
	hilight_colour.b = 189;
	hilight_colour.a = 255;

	pressed_item = -1;

	mouse_down = false;
}

void Widget_List::up()
{
	if (selected > 0) {
		selected--;
		if (selected < top) {
			top--;
		}
	}
}

void Widget_List::down()
{
	if (selected < (int)items.size()-1) {
		selected++;
		if (top + visible_rows() <= selected) {
			top++;
		}
	}
}

int Widget_List::get_click_row(int y)
{
	int row = (y - calculated_y) / row_h + top;
	if (row < 0) {
		row = 0;
	}
	else if (row >= (int)items.size()) {
		row = items.size()-1;
	}
	return row;
}

void Widget_List::change_top(int rows)
{
	int vr = visible_rows();
	top += rows;
	if (top < 0) {
		top = 0;
	}
	else if ((int)items.size() <= vr) {
		top = 0;
	}
	else if (top > (int)items.size() - vr) {
		top = items.size() - vr;
	}
	if (selected < top) {
		selected = top;
	}
	else if (selected >= top + vr) {
		selected = MIN((int)items.size()-1, top + vr - 1);
	}
}

int Widget_List::visible_rows()
{
	return calculated_h / row_h;
}

int Widget_List::used_height()
{
	return visible_rows() * row_h;
}

//--

Widget_Slider::Widget_Slider(int width, int stops, int initial_value) :
	Widget(width, slider_image->size.h),
	stops(stops),
	value(initial_value),
	mouse_down(false)
{
	accepts_focus = true;
}

void Widget_Slider::handle_event(TGUI_Event *event)
{
	bool focussed = gui->get_focus() == this;

	if (focussed && event->type == TGUI_FOCUS) {
		if (event->focus.type == TGUI_FOCUS_LEFT) {
			if (value > 0) {
				value--;
			}
		}
		else if (event->focus.type == TGUI_FOCUS_RIGHT) {
			if (value < stops-1) {
				value++;
			}
		}
	}
	else if (event->type == TGUI_MOUSE_DOWN || (mouse_down && event->type == TGUI_MOUSE_AXIS)) {
		int old_value = value;
		if (mouse_down) {
			value = int(((event->mouse.x-calculated_x) / calculated_w) * stops);
			if (value < 0) {
				value = 0;
			}
			else if (value >= stops) {
				value = stops-1;
			}
		}
		else {
			TGUI_Event e = tgui_get_relative_event(this, event);
			if (e.mouse.x >= 0) {
				mouse_down = true;
				value = int((e.mouse.x / calculated_w) * stops);
			}
		}
		if (value != old_value) {
			noo.widget_mml->play(false);
		}
	}
	else if (event->type == TGUI_MOUSE_UP) {
		mouse_down = false;
	}
}

void Widget_Slider::draw()
{
	noo.draw_line(noo.black, Point<float>((float)calculated_x, calculated_y+2.5), Point<float>(float(calculated_x+calculated_w-1), calculated_y+2.5));

	int x = int((float)value / (stops-1) * (calculated_w-1) - slider_image->size.w / 2);

	bool focussed = gui->get_focus() == this;

	if (focussed) {
		enable_focus_shader(true);
	}

	slider_image->draw_single(Point<int>(calculated_x + x, calculated_y));

	if (focussed) {
		enable_focus_shader(false);
	}
}

int Widget_Slider::get_value()
{
	return value;
}

//--
	
Widget_Radio_Button::Widget_Radio_Button(std::string text) :
	Widget(0, 0),
	text(text),
	selected(false)
{
	w = int(radio_image->size.w + 1 + noo.font->get_text_width(text));
	h = int(noo.font->get_height()) + 2;

	accepts_focus = true;
}

void Widget_Radio_Button::handle_event(TGUI_Event *event)
{
	bool focussed = gui->get_focus() == this;

	if (focussed && event->type == TGUI_KEY_DOWN) {
		if (event->keyboard.code == TGUIK_SPACE || event->keyboard.code == TGUIK_RETURN) {
			select();
		}
	}
	else if (focussed && event->type == TGUI_JOY_DOWN) {
		if (event->joystick.button == noo.joy_b1) {
			select();
		}
	}
	else if (event->type == TGUI_MOUSE_DOWN) {
		TGUI_Event e = tgui_get_relative_event(this, event);

		if (e.mouse.x >= 0 && e.mouse.y >= 0) {
			select();
		}
	}
}

void Widget_Radio_Button::draw()
{
	Image *image;

	if (selected) {
		image = radio_selected_image;
	}
	else {
		image = radio_image;
	}

	bool focussed = gui->get_focus() == this;

	if (focussed) {
		enable_focus_shader(true);
	}

	image->draw_single(Point<int>(calculated_x, calculated_y));

	if (focussed) {
		enable_focus_shader(false);
	}

	noo.font->enable_shadow(noo.shadow_colour, Font::DROP_SHADOW);
	noo.font->draw(noo.white, text, Point<int>(calculated_x + image->size.w + 1, calculated_y));
	noo.font->disable_shadow();
}

bool Widget_Radio_Button::is_selected()
{
	return selected;
}

void Widget_Radio_Button::set_selected(bool selected)
{
	if (selected) {
		select();
	}
	else {
		this->selected = false;
	}
}

void Widget_Radio_Button::set_group(Group group)
{
	this->group = group;
}

void Widget_Radio_Button::select()
{
	noo.button_mml->play(false);

	selected = true;

	for (size_t i = 0; i < group.size(); i++) {
		Widget_Radio_Button *b = group[i];
		if (b != this) {
			b->set_selected(false);
		}
	}
}

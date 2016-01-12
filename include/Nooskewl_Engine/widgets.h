#ifndef WIDGETS_H
#define WIDGETS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class GUI;
class Image;
class Sprite;

class NOOSKEWL_ENGINE_EXPORT Widget : public TGUI_Widget {
public:
	GUI *noo_gui;

	static void static_start();
	static void static_end();

	void enable_focus_shader(bool enable);

	Widget(int w, int h);
	Widget(float percent_w, float percent_h);
	Widget(int w, float percent_h);
	Widget(float percent_w, int h);
	virtual ~Widget();

	virtual void draw();

	void set_background_colour(SDL_Colour background_colour);

protected:
	static Image *button_image;
	static Image *button_image_pressed;
	static Image *slider_image;
	static Image *radio_image;
	static Image *radio_selected_image;

	void init();

	SDL_Colour background_colour;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Button : public Widget {
public:
	Widget_Button(int w, int h);
	Widget_Button(float w, float h);
	Widget_Button(int w, float h);
	Widget_Button(float w, int h);
	virtual ~Widget_Button();

	void handle_event(TGUI_Event *event);

	bool pressed();

protected:
	bool _pressed;
	bool _released;
	bool _hover;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Text_Button : public Widget_Button
{
public:
	// Any size < 0 == don't care
	Widget_Text_Button(std::string, int w, int h);
	Widget_Text_Button(std::string, float w, float h);
	Widget_Text_Button(std::string, int w, float h);
	Widget_Text_Button(std::string, float w, int h);
	Widget_Text_Button(std::string text);
	virtual ~Widget_Text_Button();

	void draw();

	void set_enabled(bool enabled);
	bool is_enabled();

private:
	void set_size(float width, float height);
	void set_default_colours();

	SDL_Colour button_colour;
	SDL_Colour text_colour;
	std::string text;
	int padding;

	bool enabled;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Window : public Widget
{
public:
	Widget_Window(int w, int h);
	Widget_Window(float percent_w, float percent_h);
	Widget_Window(int w, float percent_h);
	Widget_Window(float percent_w, int h);
	virtual ~Widget_Window();

	void draw();

private:
	void set_default_colours();

	SDL_Colour background_colour;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Label : public Widget
{
public:
	Widget_Label(std::string text, int max_w);

	void draw();

	void set_text(std::string text);
	void set_width(int width);
	void set_colour(SDL_Colour colour);

private:
	std::string text;
	int max_w;
	SDL_Colour colour;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Image : public Widget
{
public:
	Widget_Image(Image *image, bool destroy = true);
	~Widget_Image();

	void draw();

private:
	Image *image;
	bool destroy;
};

class NOOSKEWL_ENGINE_EXPORT Widget_List : public Widget
{
public:
	Widget_List(int w, int h);
	Widget_List(float w, float h);
	Widget_List(int w, float h);
	Widget_List(float w, int h);

	void handle_event(TGUI_Event *event);
	void draw();

	void set_items(std::vector<std::string> new_items);
	int pressed();
	int get_selected();
	void set_selected(int selected);

	void set_hilight(int index, bool onoff);
	bool is_hilighted(int index);

private:
	void init();
	void up();
	void down();
	int get_click_row(int y);
	void change_top(int rows);
	int visible_rows();
	int used_height();

	std::vector<std::string> items;
	int top;
	int selected;
	int row_h;
	SDL_Colour hilight_colour;

	int pressed_item;

	bool mouse_down;
	bool clicked;
	Point<int> mouse_down_point;
	int mouse_down_row;

	std::vector<int> hilight;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Slider : public Widget
{
public:
	Widget_Slider(int width, int stops, int initial_value);

	void handle_event(TGUI_Event *event);
	void draw();

	int get_value();

private:
	int stops;
	int value;
	bool mouse_down;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Radio_Button : public Widget
{
public:
	typedef std::vector<Widget_Radio_Button *> Group;

	Widget_Radio_Button(std::string text);

	void handle_event(TGUI_Event *event);
	void draw();

	bool is_selected();
	void set_selected(bool selected);

	void set_group(Group group);

private:
	void select();

	std::string text;
	bool selected;
	Group group;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Sprite : public Widget
{
public:
	Widget_Sprite(Sprite *sprite, bool destroy = true);
	virtual ~Widget_Sprite();

	void draw();

	void set_animation(std::string name);

private:
	Sprite *sprite;
	bool destroy;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Sprite_Toggle : public Widget_Sprite
{
public:
	Widget_Sprite_Toggle(Sprite *sprite, bool value, bool destroy = true);
	~Widget_Sprite_Toggle();

	void handle_event(TGUI_Event *event);

	bool get_value();

private:
	bool value;
};

} // End namespace Nooskewl_Engine

#endif // WIDGETS_H

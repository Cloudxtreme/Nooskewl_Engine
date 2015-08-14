#ifndef WIDGETS_H
#define WIDGETS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class GUI;
class Image;

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

private:
	void set_size(float width, float height);
	void set_default_colours();

	SDL_Colour button_colour;
	SDL_Colour text_colour;
	std::string text;
	int padding;
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

	std::vector<std::string> &get_items();
	int pressed();
	int get_selected();

	void set_hilight(int hilight);

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

	int hilight;
};

} // End namespace Nooskewl_Engine

#endif // WIDGETS_H

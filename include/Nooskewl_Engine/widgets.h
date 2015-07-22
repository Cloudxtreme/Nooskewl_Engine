#ifndef WIDGETS_H
#define WIDGETS_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/types.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Widget_Widget : public TGUI_Widget {
public:
	static void static_start();
	static void static_end();
	static void enable_focus_shader(bool enable);

	Widget_Widget(int w, int h);
	Widget_Widget(float percent_w, float percent_h);
	Widget_Widget(int w, float percent_h);
	Widget_Widget(float percent_w, int h);
	virtual ~Widget_Widget();

	virtual void draw();

	void set_background_colour(SDL_Colour background_colour);

protected:
	static Image *button_image;
	static Image *button_image_pressed;

	SDL_Colour background_colour;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Button : public Widget_Widget {
public:
	Widget_Button(int w, int h);
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
	Widget_Text_Button(std::string, Size<int> size); // < 0 = don't care
	Widget_Text_Button(std::string text);
	virtual ~Widget_Text_Button();

	void draw();

private:
	void set_size(Size<int> size);
	void set_default_colours();

	SDL_Colour button_colour;
	SDL_Colour text_colour;
	std::string text;
	int padding;
};

class NOOSKEWL_ENGINE_EXPORT Widget_Window : public Widget_Widget
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

class NOOSKEWL_ENGINE_EXPORT Widget_Label : public Widget_Widget
{
public:
	Widget_Label(std::string text, int max_w);

	void draw();

private:
	std::string text;
	int max_w;
	SDL_Colour colour;
};

} // End namespace Nooskewl_Engine

#endif // WIDGETS_H

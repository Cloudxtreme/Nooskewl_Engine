#ifndef WIDGETS_H
#define WIDGETS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT MO3_Widget : public TGUI_Widget {
public:
	static void static_start();
	static void static_end();
	static void enable_focus_shader(bool enable);

	MO3_Widget(int w, int h);
	MO3_Widget(float percent_w, float percent_h);
	MO3_Widget(int w, float percent_h);
	MO3_Widget(float percent_w, int h);
	virtual ~MO3_Widget();

protected:
	static Image *focus_image;
	static Image *button_image;
	static Image *button_image_pressed;
};

class NOOSKEWL_ENGINE_EXPORT MO3_Button : public MO3_Widget {
public:
	MO3_Button(int w, int h);
	virtual ~MO3_Button();

	void handle_event(TGUI_Event *event);

	bool pressed();

protected:
	bool _pressed;
	bool _released;
	bool _hover;
};

class NOOSKEWL_ENGINE_EXPORT MO3_Text_Button : public MO3_Button
{
public:
	MO3_Text_Button(std::string, Size<int> size); // < 0 = don't care
	MO3_Text_Button(std::string text);
	virtual ~MO3_Text_Button();

	void draw();

private:
	void set_size(Size<int> size);
	void set_default_colours();

	SDL_Colour button_colour;
	SDL_Colour text_colour;
	std::string text;
	int padding;
};

class NOOSKEWL_ENGINE_EXPORT MO3_Window : public MO3_Widget
{
public:
	MO3_Window(int w, int h);
	MO3_Window(float percent_w, float percent_h);
	MO3_Window(int w, float percent_h);
	MO3_Window(float percent_w, int h);
	virtual ~MO3_Window();

	void draw();

private:
	void set_default_colours();

	SDL_Colour background_colour;
};

class NOOSKEWL_ENGINE_EXPORT MO3_Label : public MO3_Widget
{
public:
	MO3_Label(std::string text, int max_w);

	void draw();

private:
	std::string text;
	int max_w;
	SDL_Colour colour;
};

} // End namespace Nooskewl_Engine

#endif // WIDGETS_H

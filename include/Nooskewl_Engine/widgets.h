#ifndef WIDGETS_H
#define WIDGETS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT NOO_Widget : public TGUI_Widget {
public:
	static void static_start();
	static void static_end();
	static void enable_focus_shader(bool enable);

	NOO_Widget(int w, int h);
	NOO_Widget(float percent_w, float percent_h);
	NOO_Widget(int w, float percent_h);
	NOO_Widget(float percent_w, int h);
	virtual ~NOO_Widget();

	virtual void draw();

	void set_background_colour(SDL_Colour background_colour);

protected:
	static Image *button_image;
	static Image *button_image_pressed;

	SDL_Colour background_colour;
};

class NOOSKEWL_ENGINE_EXPORT NOO_Button : public NOO_Widget {
public:
	NOO_Button(int w, int h);
	virtual ~NOO_Button();

	void handle_event(TGUI_Event *event);

	bool pressed();

protected:
	bool _pressed;
	bool _released;
	bool _hover;
};

class NOOSKEWL_ENGINE_EXPORT NOO_Text_Button : public NOO_Button
{
public:
	NOO_Text_Button(std::string, Size<int> size); // < 0 = don't care
	NOO_Text_Button(std::string text);
	virtual ~NOO_Text_Button();

	void draw();

private:
	void set_size(Size<int> size);
	void set_default_colours();

	SDL_Colour button_colour;
	SDL_Colour text_colour;
	std::string text;
	int padding;
};

class NOOSKEWL_ENGINE_EXPORT NOO_Window : public NOO_Widget
{
public:
	NOO_Window(int w, int h);
	NOO_Window(float percent_w, float percent_h);
	NOO_Window(int w, float percent_h);
	NOO_Window(float percent_w, int h);
	virtual ~NOO_Window();

	void draw();

private:
	void set_default_colours();

	SDL_Colour background_colour;
};

class NOOSKEWL_ENGINE_EXPORT NOO_Label : public NOO_Widget
{
public:
	NOO_Label(std::string text, int max_w);

	void draw();

private:
	std::string text;
	int max_w;
	SDL_Colour colour;
};

} // End namespace Nooskewl_Engine

#endif // WIDGETS_H

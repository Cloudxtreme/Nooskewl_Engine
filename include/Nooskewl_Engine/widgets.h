#ifndef WIDGETS_H
#define WIDGETS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT SS_Widget : public TGUI_Widget {
public:
	SS_Widget(int w, int h);
	SS_Widget(float percent_w, float percent_h);
	SS_Widget(int w, float percent_h);
	SS_Widget(float percent_w, int h);

	void draw();
	void handle_event(TGUI_Event *event);

private:
	bool got_event;
	int event_x, event_y;
};

class NOOSKEWL_ENGINE_EXPORT SS_Button : public SS_Widget {
public:
	SS_Button(int w, int h);

	void handle_event(TGUI_Event *event);

	bool pressed();

protected:
	bool _pressed;
};

class NOOSKEWL_ENGINE_EXPORT SS_Text_Button : public SS_Button
{
public:
	static const int PAD_X = 10;
	static const int PAD_Y = 5;

	SS_Text_Button(std::string text);

	void draw();

private:
	std::string text;
};

} // End namespace Nooskewl_Engine

#endif // WIDGETS_H
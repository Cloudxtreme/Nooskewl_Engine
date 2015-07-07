#ifndef WIDGETS_H
#define WIDGETS_H

#include "tgui3.h"

class SS_Div : public TGUI_Div {
public:
	SS_Div(int w, int h);
	SS_Div(float percent_w, float percent_h);
	SS_Div(int w, float percent_h);
	SS_Div(float percent_w, int h);

	void draw();
	void handle_event(TGUI_Event *event);

private:
	bool got_event;
	int event_x, event_y;
};

#endif // WIDGETS_H
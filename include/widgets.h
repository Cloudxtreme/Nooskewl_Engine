#ifndef WIDGETS_H
#define WIDGETS_H

#include "tgui3.h"

class SS_Div : public TGUI_Div {
public:
	SS_Div(int w, int h);
	SS_Div(float percent_w, float percent_h);
	SS_Div(int w, float percent_h);
	SS_Div(float percent_w, int h);

	void draw(TGUI_Div *parent, int x, int y);
};

#endif // WIDGETS_H
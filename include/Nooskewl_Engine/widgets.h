#ifndef WIDGETS_H
#define WIDGETS_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

	class EXPORT SS_Widget : public TGUI_Widget {
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

}

#ifdef NOOSKEWL_ENGINE_BUILD
using namespace Nooskewl_Engine;
#endif

#endif // WIDGETS_H
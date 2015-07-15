#ifndef SPEECH_H
#define SPEECH_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Speech {
public:
	static const int TEXT_DELAY = 50; // millisecond

	static void static_start();
	static void static_end();

	Speech(std::string text);
	~Speech();

	void start();

	// Return false when done
	bool handle_event(TGUI_Event *event);
	void draw();

private:
	std::string text;
	std::string name;
	int start_time;
	int offset;
	bool advance;
	bool done;
	int skip;

	static Sprite *speech_advance;
};

} // End namespace Nooskewl_Engine

#endif // SPEECH_H
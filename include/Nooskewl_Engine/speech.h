#ifndef SPEECH_H
#define SPEECH_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Speech {
public:
	static const int TEXT_DELAY = 50; // millisecond

	Speech(std::string text);

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
};

} // End namespace Nooskewl_Engine

#endif // SPEECH_H
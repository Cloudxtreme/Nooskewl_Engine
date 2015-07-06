#ifndef SPEECH_H
#define SPEECH_H

#include "starsquatters.h"

class Speech {
public:
	Speech(std::string text);

	void start();

	// Return false when done
	bool handle_event(SDL_Event *event);
	void draw();

private:
	std::string text;
	int start_time;
	int offset;
	bool advance;
	bool done;
	int skip;
};

#endif // SPEECH_H
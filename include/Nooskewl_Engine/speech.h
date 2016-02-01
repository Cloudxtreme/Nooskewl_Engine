#ifndef SPEECH_H
#define SPEECH_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/callback_data.h"

namespace Nooskewl_Engine {

class Sprite;

class Speech {
public:
	struct Callback_Data : public Generic_Callback_Data {
		bool was_multiple_choice;
		int choice;
	};

	static void multiple_choice_callback(void *data);

	static const int TEXT_DELAY = 50; // millisecond

	static void static_start();
	static void static_end();

	Speech(std::string text, Callback callback = 0, void *callback_data = 0);
	~Speech();

	void start();

	// Return false when done
	bool handle_event(TGUI_Event *event);
	void draw();

private:
	struct Multiple_Choice_Data {
		std::vector<Speech *> paths;
		Callback callback;
		void *callback_data;
	};

	static Sprite *speech_advance;

	void token(std::string s);

	std::string text;
	std::string name;
	int start_time;
	int offset;
	bool advance;
	bool done;
	int skip;
	bool top;
	bool right;

	Callback callback;
	void *callback_data;

	int milestone;
	bool milestone_on_off;

	// for when speech has a multiple choice
	std::string multiple_choice_caption;
	std::vector<std::string> multiple_choice_options;
	Multiple_Choice_Data *multiple_choice_data;
};

} // End namespace Nooskewl_Engine

#endif // SPEECH_H

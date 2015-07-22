#ifndef TALK_BRAIN
#define TALK_BRAIN

#include "Nooskewl_Engine/brain.h"

using namespace Nooskewl_Engine;

class NOOSKEWL_ENGINE_EXPORT Talk_Brain : public Brain {
public:
	static void callback(void *data);
	struct Callback_Data {
		Direction direction;
		Map_Entity *entity;
		Callback user_callback;
		void *user_callback_data;
	} callback_data;

	Talk_Brain(std::string name, Callback callback = NULL, void *callback_data = NULL);
	~Talk_Brain();

	void activate(Map_Entity *activator, Map_Entity *activated);

	bool save(SDL_RWops *file);

private:
	struct Talk {
		int milestone;
		std::string text;
	};

	static bool compare_milestones(Talk *a, Talk *b);

	std::string name;

	// sorted: highest milestone first
	std::vector<Talk *> sayings;
	Callback user_callback;
	void *user_callback_data;
};

#endif // TALK_BRAIN
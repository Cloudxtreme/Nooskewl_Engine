#ifndef Widget_H
#define Widget_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/widgets.h"

namespace Nooskewl_Engine {

class Brain;
class Map_Entity;

class NOOSKEWL_ENGINE_EXPORT GUI {
public:
	TGUI *gui;
	TGUI_Widget *focus; // backup focus

	GUI();
	virtual ~GUI();

	virtual bool update();
	virtual bool update_background(); // called when the GUI is not the foremost
	virtual void draw_back() {}
	virtual void draw_fore() {}
};

class NOOSKEWL_ENGINE_EXPORT Title_GUI : public GUI {
public:
	static void game_loaded_callback(void *);

	Title_GUI();
	virtual ~Title_GUI();

	bool update();
	bool update_background();
	void draw_back();
	void draw_fore();

private:
	static bool game_loaded;

	bool did_intro;
	Uint32 intro_start;

	Image *logo;

	Widget_Text_Button *new_game_button;
	Widget_Text_Button *load_game_button;
};

class NOOSKEWL_ENGINE_EXPORT Pause_GUI : public GUI {
public:
	Pause_GUI();

	bool update();

private:
	Widget_Text_Button *resume_button;
	Widget_Text_Button *save_button;
	Widget_Text_Button *quit_button;
};

class NOOSKEWL_ENGINE_EXPORT Notification_GUI : public GUI {
public:
	Notification_GUI(std::string text);

	bool update();

private:
	Widget_Text_Button *ok_button;
};

class NOOSKEWL_ENGINE_EXPORT Save_Load_GUI : public GUI {
public:
	Save_Load_GUI(bool saving, Callback callback = 0);

	bool update();

private:
	bool save_game(SDL_RWops *file);
	bool save_milestones(SDL_RWops *file);
	bool load_game(SDL_RWops *file);
	bool load_milestones(SDL_RWops *file);
	bool load_map(SDL_RWops *file);
	Map_Entity *load_entity(SDL_RWops *file);
	Brain *load_brain(SDL_RWops *file);

	bool saving;
	Callback callback;

	Widget_Text_Button *ok_button;
};

} // End namespace Nooskewl_Engine

#endif // Widget_H
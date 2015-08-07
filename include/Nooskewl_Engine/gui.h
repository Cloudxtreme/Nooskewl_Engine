#ifndef Widget_H
#define Widget_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Brain;
class Image;
class Map_Entity;
class Sprite;
class Stats;
class Widget_Label;
class Widget_Text_Button;

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
	static void callback(void *);

	Title_GUI();
	virtual ~Title_GUI();

	bool update();
	bool update_background();
	void draw_back();
	void draw_fore();

private:
	bool check_loaded();

	static bool loading;
	static bool loaded;

	bool did_intro;
	Uint32 intro_start;

	Image *static_logo;
	Sprite *logo;

	Widget_Text_Button *new_game_button;
	Widget_Text_Button *load_game_button;

	Uint32 logo_animation_start;
};

class NOOSKEWL_ENGINE_EXPORT Pause_GUI : public GUI {
public:
	static void callback(void *data);

	Pause_GUI();

	bool update();
	bool update_background();

private:
	bool check_quit();
	void set_labels();

	static bool quitting;
	static bool quit;

	Widget_Text_Button *resume_button;
	Widget_Text_Button *save_button;
	Widget_Text_Button *quit_button;

	Stats *stats;

	Widget_Label *alignment_label;
	Widget_Label *sex_label;
	Widget_Label *hp_label;
	Widget_Label *mp_label;
	Widget_Label *attack_label;
	Widget_Label *defense_label;
	Widget_Label *agility_label;
	Widget_Label *karma_label;
	Widget_Label *luck_label;
	Widget_Label *speed_label;
	Widget_Label *strength_label;
	Widget_Label *experience_label;
	Widget_Label *name;
	Widget_Label *alignment;
	Widget_Label *sex;
	Widget_Label *hp;
	Widget_Label *mp;
	Widget_Label *attack;
	Widget_Label *defense;
	Widget_Label *agility;
	Widget_Label *karma;
	Widget_Label *luck;
	Widget_Label *speed;
	Widget_Label *strength;
	Widget_Label *experience;
};

class NOOSKEWL_ENGINE_EXPORT Notification_GUI : public GUI {
public:
	Notification_GUI(std::string text);

	bool update();

private:
	Widget_Text_Button *ok_button;
};

class NOOSKEWL_ENGINE_EXPORT Yes_No_GUI : public GUI {
public:
	Yes_No_GUI(std::string text, Callback callback);

	bool update();

private:
	Widget_Text_Button *yes_button;
	Widget_Text_Button *no_button;

	Callback callback;
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

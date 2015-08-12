#ifndef Widget_H
#define Widget_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/item.h"

namespace Nooskewl_Engine {

class Image;
class Sprite;
class Widget_Image;
class Widget_Label;
class Widget_List;
class Widget_Text_Button;

class NOOSKEWL_ENGINE_EXPORT GUI {
public:
	TGUI *gui;
	TGUI_Widget *focus; // backup focus
	float global_alpha;

	GUI();
	virtual ~GUI();

	void start(); // call after adding all widgets

	virtual void handle_event(TGUI_Event *event);
	virtual bool update();
	virtual bool update_background(); // called when the GUI is not the foremost
	virtual void draw_back();
	virtual void draw_fore();

	bool do_return(bool ret);

	virtual bool fade_done(bool fade_in) { return false; } // return true to cancel

protected:
	void set_noo_gui(TGUI_Widget *widget);

	bool fade;
	bool fading_in;
	bool fading_out;
	Uint32 fade_start;
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

	bool fade_done(bool fade_in);

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

	bool do_new_game;
};

class NOOSKEWL_ENGINE_EXPORT Pause_GUI : public GUI {
public:
	static void callback(void *data);

	Pause_GUI();

	void handle_event(TGUI_Event *event);
	bool update();
	bool update_background();

	bool fade_done(bool fading_in);

private:
	bool check_quit();
	void set_labels();

	static bool quitting;
	static bool quit;

	Widget_Text_Button *quit_button;
	Widget_Text_Button *save_button;
	Widget_Text_Button *resume_button;

	Stats *stats;

	Widget_Image *profile_image;
	Widget_Label *alignment_label;
	Widget_Label *sex_label;
	Widget_Label *hp_label;
	Widget_Label *mp_label;
	Widget_Label *experience_label;

	Widget_Label *weapon_label;
	Widget_Label *armour_label;

	Widget_Label *attack_label;
	Widget_Label *defense_label;
	Widget_Label *agility_label;
	Widget_Label *karma_label;
	Widget_Label *luck_label;
	Widget_Label *speed_label;
	Widget_Label *strength_label;
	Widget_Label *hunger_label;
	Widget_Label *thirst_label;
	Widget_Label *rest_label;
	Widget_Label *sobriety_label;

	Widget_Label *name;
	Widget_Label *alignment;
	Widget_Label *sex;
	Widget_Label *hp;
	Widget_Label *mp;
	Widget_Label *experience;

	Widget_Label *weapon;
	Widget_Label *armour;

	Widget_Label *attack;
	Widget_Label *defense;
	Widget_Label *agility;
	Widget_Label *karma;
	Widget_Label *luck;
	Widget_Label *speed;
	Widget_Label *strength;
	Widget_Label *hunger;
	Widget_Label *thirst;
	Widget_Label *rest;
	Widget_Label *sobriety;

	Widget_Text_Button *items_button;
	Widget_Text_Button *weapons_button;
	Widget_Text_Button *armour_button;

	bool exit_menu;
};

class NOOSKEWL_ENGINE_EXPORT Items_GUI : public GUI {
public:
	Items_GUI(Item::Type type);

	void handle_event(TGUI_Event *event);
	bool update();

private:
	void set_labels();

	Stats *stats;

	Widget_List *list;
	Widget_Text_Button *done_button;

	Widget_Label *condition_label;
	Widget_Label *weight_label;
	Widget_Label *value_label;
	Widget_Label *properties_label;

	std::vector<int> indices; // index into inventory

	bool exit_menu;
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
	enum Save_Load {
		ERR,
		SAVE,
		LOAD
	};

	Save_Load_GUI(bool saving, Callback callback = 0);

	bool update();

private:
	TGUI *create_notify_gui(std::string caption);

	bool saving;
	Callback callback;

	Widget_Text_Button *ok_button;
};

} // End namespace Nooskewl_Engine

#endif // Widget_H

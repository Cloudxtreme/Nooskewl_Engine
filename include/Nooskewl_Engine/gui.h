#ifndef GUI_H
#define GUI_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/item.h"

namespace Nooskewl_Engine {

class Image;
class Sprite;
class Widget_Image;
class Widget_Label;
class Widget_List;
class Widget_Radio_Button;
class Widget_Slider;
class Widget_Text_Button;

class NOOSKEWL_ENGINE_EXPORT GUI {
public:
	TGUI *gui;
	TGUI_Widget *focus; // backup focus
	float global_alpha;
	float global_alpha_backup; // value before we changed it

	GUI();
	virtual ~GUI();

	bool is_fading_out() { return fading_out; }
	bool is_fadeout_finished();

	void start(); // call after adding all widgets

	virtual void handle_event(TGUI_Event *event);

	virtual void update() {}
	virtual void update_background() {} // called when the GUI is not the foremost

	virtual void draw_back();
	virtual void draw();
	virtual void draw_fore();

	bool do_return(bool ret);

	virtual bool fade_done(bool fade_in) { return false; } // return true to cancel

protected:
	void exit();
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

	void update();
	void update_background();
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

class NOOSKEWL_ENGINE_EXPORT Notification_GUI : public GUI {
public:
	Notification_GUI(std::string text);

	void update();

private:
	Widget_Text_Button *ok_button;
};

class NOOSKEWL_ENGINE_EXPORT Yes_No_GUI : public GUI {
public:
	struct Callback_Data {
		bool choice;
		void *userdata;
	};

	Yes_No_GUI(std::string text, Callback callback, void *callback_data = 0);

	void update();

private:
	Widget_Text_Button *yes_button;
	Widget_Text_Button *no_button;

	Callback callback;
	void *callback_data;
};

class NOOSKEWL_ENGINE_EXPORT Yes_No_Always_GUI : public GUI {
public:
	Yes_No_Always_GUI(std::string text, int milestone, Callback callback);

	void update();

private:
	Widget_Text_Button *yes_button;
	Widget_Text_Button *no_button;
	Widget_Text_Button *always_button;

	int milestone;

	Callback callback;
};

class NOOSKEWL_ENGINE_EXPORT Get_Number_GUI : public GUI {
public:
	Get_Number_GUI(std::string text, int stops, int initial_value, Callback callback);

	void handle_event(TGUI_Event *event);
	void update();

private:
	Widget_Slider *slider;
	Widget_Label *value_label;
	Widget_Text_Button *ok_button;
	Widget_Text_Button *cancel_button;

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

	void update();

private:
	TGUI *create_notify_gui(std::string caption);

	bool saving;
	Callback callback;

	Widget_Text_Button *ok_button;
};

class Multiple_Choice_GUI : public GUI {
public:
	struct Callback_Data {
		int choice;
		void *userdata;
	};

	Multiple_Choice_GUI(std::string caption, std::vector<std::string> choices, int escape_choice, Callback callback, void *callback_data);

	void handle_event(TGUI_Event *event);
	void update();

private:
	Widget_Label *caption_label;
	Widget_List *list;

	Callback callback;
	void *callback_data;

	bool exit_menu;

	int escape_choice; // escape activates this choice. If it's -1, escape does nothing. If it's -2, escape dismisses the dialog with no action.
};

} // End namespace Nooskewl_Engine

#endif // GUI_H

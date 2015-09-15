#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/font.h"
#include "Nooskewl_Engine/gui.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/mml.h"
#include "Nooskewl_Engine/player_brain.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/stats.h"
#include "Nooskewl_Engine/translation.h"
#include "Nooskewl_Engine/widgets.h"

using namespace Nooskewl_Engine;

bool Title_GUI::loading;
bool Title_GUI::loaded;

GUI::GUI() :
	focus(0)
{
	fade = true;
	fading_in = true;
	fading_out = false;
	fade_start = SDL_GetTicks();
}

GUI::~GUI()
{
	delete gui;
}

void GUI::start()
{
	if (gui) {
		TGUI_Widget *main_widget = gui->get_main_widget();

		set_noo_gui(main_widget);
	}
}

void GUI::handle_event(TGUI_Event *event) {
	if (gui) {
		gui->handle_event(event);
	}
}

void GUI::draw_back()
{
	if (fade == false) {
		return;
	}

	if (fading_in) {
		float p = (SDL_GetTicks() - fade_start) / 200.0f;
		if (p >= 1.0f) {
			p = 1.0f;
			fading_in = false;
			fade_done(true);
		}
		global_alpha = p;
		noo.current_shader->set_float("global_alpha", global_alpha);
	}
	else if (fading_out) {
		float p = (SDL_GetTicks() - fade_start) / 200.0f;
		if (p >= 1.0f) {
			p = 1.0f;
		}
		p = 1.0f - p;
		global_alpha = p;
		noo.current_shader->set_float("global_alpha", global_alpha);
	}
}

void GUI::draw_fore()
{
	global_alpha = 1.0f;
	noo.current_shader->set_float("global_alpha", global_alpha);
}

bool GUI::is_fadeout_finished() {
	if (fading_out) {
		if (fade == false) {
			return true;
		}
		else if ((SDL_GetTicks() - fade_start) >= 200) {
			if (fade_done(false) == false) {
				return true;
			}
			else {
				fading_out = false;
			}
		}
	}

	return false;
}

void GUI::exit()
{
	fading_out = true;

	if (fade) {
		fade_start = SDL_GetTicks();
	}
}

void GUI::set_noo_gui(TGUI_Widget *widget)
{
	Widget *noo_widget = dynamic_cast<Widget *>(widget);
	if (noo_widget) {
		noo_widget->noo_gui = this;
	}

	std::vector<TGUI_Widget *> &children = widget->get_children();

	for (size_t i = 0; i < children.size(); i++) {
		set_noo_gui(children[i]);
	}
}

//--

void Title_GUI::callback(void *data)
{
	if (loading) {
		loading = false;
		loaded = (Save_Load_GUI::Save_Load)((int64_t)data) == Save_Load_GUI::LOAD;
	}
}

Title_GUI::Title_GUI() :
	did_intro(false),
	intro_start(SDL_GetTicks())
{
	loading = loaded = false;

	noo.play_music("title.mml");

	static_logo = new Image("static_logo.tga");
	logo = new Sprite("logo");

	logo->reset();
	logo_animation_start = SDL_GetTicks();

	Widget *main_widget = new Widget(1.0f, 1.0f);

	Widget *bottom_floater = new Widget(1.0f, 0.33f);
	bottom_floater->set_float_bottom(true);
	bottom_floater->set_parent(main_widget);

	new_game_button = new Widget_Text_Button(noo.t->translate(5));
	new_game_button->set_center_x(true);
	new_game_button->set_center_y(true);
	new_game_button->set_padding_right(2);
	new_game_button->set_parent(bottom_floater);

	load_game_button = new Widget_Text_Button(noo.t->translate(4));
	load_game_button->set_center_x(true);
	load_game_button->set_center_y(true);
	load_game_button->set_padding_left(2);
	load_game_button->set_parent(bottom_floater);

	gui = new TGUI(main_widget, noo.screen_size.w, noo.screen_size.h);
	gui->set_focus(new_game_button);
}

Title_GUI::~Title_GUI()
{
	delete static_logo;
	delete logo;
}

void Title_GUI::update()
{
	if (check_loaded() == false) {
		exit();
		return;
	}

	if (new_game_button->pressed()) {
		do_new_game = true;
		exit();
		return;
	}
	else if (load_game_button->pressed()) {
		do_new_game = false;

		loading = true;
		loaded = false;

		Save_Load_GUI *save_load_gui = new Save_Load_GUI(false, callback);
		save_load_gui->start();
		noo.guis.push_back(save_load_gui);
	}
}

void Title_GUI::update_background()
{
	if (check_loaded() == false) {
		exit();
	}
}

void Title_GUI::draw_back()
{
	int scale1 = 4;
	int x1 = -static_logo->size.w / 2 * scale1;
	int y1 = -static_logo->size.h / 2 * scale1;
	int scale2 = 7;
	int x2 = -static_logo->size.w / 4 * scale2;
	int y2 = -static_logo->size.h / 4 * scale2;
	int dx1 = x1 + static_logo->size.w / 2 * scale1;
	int dy2 = y2 + static_logo->size.h / 4 * scale2;
	float p1 = (SDL_GetTicks() % 20000) / 20000.0f * 2.0f;
	if (p1 >= 1.0f) {
		p1 = 1.0f - (p1 - 1.0f);
	}
	float p2 = (SDL_GetTicks() % 30000) / 30000.0f * 2.0f;
	if (p2 >= 1.0f) {
		p2 = 1.0f - (p2 - 1.0f);
	}
	SDL_Colour tint1 = noo.white;
	tint1.a = 32;
	static_logo->stretch_region_tinted_single(tint1, Point<float>(0, 0), static_logo->size, Point<float>(x1 + (dx1 - x1) * p1, (float)y1), static_logo->size * scale1);
	SDL_Colour tint2 = noo.white;
	tint2.a = 64;
	static_logo->stretch_region_tinted_single(tint2, Point<float>(0, 0), static_logo->size, Point<float>((float)x2, y2 + (dy2 - y2) * p2), static_logo->size * scale2);

	GUI::draw_back();
}

void Title_GUI::draw_fore()
{
	GUI::draw_fore();

	Point<float> pos;
	Size<int> size;

	Image *logo_image = logo->get_current_image();

	if (did_intro == false) {
		int max_w = logo_image->size.w * 16 - logo_image->size.w;
		int max_h = logo_image->size.h * 16 - logo_image->size.h;
		float p = (SDL_GetTicks() - intro_start) / 2000.0f;
		if (p > 1.0f) {
			p = 1.0f;
			did_intro = true;
		}
		float w = (1.0f - p) * max_w + logo_image->size.w;
		float h = (1.0f - p) * max_h + logo_image->size.h;
		size = Size<int>((int)w, (int)h);
		pos.x = noo.screen_size.w / 2 - w / 2;
		pos.y = noo.screen_size.h / 3 - h / 2;
	}
	else {
		pos.x = float(noo.screen_size.w / 2 - logo_image->size.w / 2);
		pos.y = float(noo.screen_size.h / 3 - logo_image->size.h / 2);
		size = logo_image->size;
	}

	SDL_Colour tint = { 255, 255, 255, 32 };

	int r = (rand() % 3 + 4) * 2;

	for (int i = 0; i < 5; i++) {
		logo_image->stretch_region_tinted_single(tint, Point<int>(0, 0), logo_image->size, pos-(r/2.0f), size+r);

		r--;
	}

	Shader *bak;
	Uint32 ticks = (SDL_GetTicks()-logo_animation_start) % 5896; // FIXME: hardcoded length
	float add;
	if (ticks < 500) {
		add = 0.0f;
	}
	else {
		add = (ticks - 500) / 200.0f * 2.0f;
		if (add > 1.0f) {
			if (add > 2.0f) {
				add = 2.0f;
			}
			add = 1.0f - (add - 1.0f);
		}
	}
	bak = noo.current_shader;
	noo.current_shader = noo.brighten_shader;
	noo.current_shader->use();
	noo.current_shader->set_float("add_r", add);
	noo.current_shader->set_float("add_g", add);
	noo.current_shader->set_float("add_b", add);

	logo_image->stretch_region_single(Point<float>(0.0f, 0.0f), logo_image->size, pos, size);

	noo.current_shader = bak;
	noo.current_shader->use();
}

bool Title_GUI::check_loaded()
{
	if (loaded) {
		loaded = false;
		return false;
	}

	return true;
}

bool Title_GUI::fade_done(bool fade_in)
{
	if (fade_in == false) {
		noo.new_game_started();

		if (do_new_game) {
			noo.player = new Map_Entity("player");
			noo.player->set_brain(new Player_Brain());
			noo.player->load_sprite("player");
			noo.player->load_stats("player");
			noo.map = new Map("start.map", false, 0);
			noo.map->add_entity(noo.player);
			noo.map->start();
			noo.map->update_camera();

			noo.reset_fancy_draw();
		}
		else {
			std::string filename;
#ifdef __APPLE__
			filename = SDL_GetBasePath();
			const char *p = filename.c_str();
			const char *p2 = strstr(p, ".app");
			if (p2) {
				filename = filename.substr(0, p2-p + 4) + "/../test.save";
			}
			else {
				filename = ""; // error
			}
#else
			filename = "test.save";
#endif

			SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "r");

			std::string caption = "";

			if (file != NULL) {
				Map::new_game_started();

				int loaded_time;
				bool result = noo.load_game(file, &loaded_time);

				SDL_RWclose(file);

				if (result == true) {
					noo.game_loaded(loaded_time);

					noo.last_map_name = "--LOADED--";

					noo.map->start();
					noo.map->update_camera();
				}
				else {
					if (noo.map) {
						delete noo.map;
						noo.map = 0;
					}

					caption = noo.t->translate(1);
				}
			}
			else {
				caption = TRANSLATE("No saved games found")END;
			}

			if (caption != "") {
				Notification_GUI *notification_gui = new Notification_GUI(caption);
				notification_gui->start();
				noo.guis.push_back(notification_gui);
				return true;
			}
		}
	}

	return false;
}

//--

Notification_GUI::Notification_GUI(std::string text)
{
	Widget *modal_main_widget = new Widget(1.0f, 1.0f);
	SDL_Colour background_colour = { 0, 0, 0, 192 };
	modal_main_widget->set_background_colour(background_colour);

	Widget_Window *window = new Widget_Window(100, 50);
	window->set_center_x(true);
	window->set_center_y(true);
	window->set_parent(modal_main_widget);

	Widget_Label *label = new Widget_Label(text, window->get_width() - 10);
	label->set_padding(5);
	label->set_center_x(true);
	label->set_parent(window);

	ok_button = new Widget_Text_Button(noo.t->translate(7));
	ok_button->set_center_x(true);
	ok_button->set_float_bottom(true);
	ok_button->set_padding_bottom(5);
	ok_button->set_parent(window);

	gui = new TGUI(modal_main_widget, noo.screen_size.w, noo.screen_size.h);
}

void Notification_GUI::update()
{
	if (ok_button->pressed()) {
		exit();
	}
}

//--

Yes_No_GUI::Yes_No_GUI(std::string text, Callback callback) :
	callback(callback)
{
	Widget *modal_main_widget = new Widget(1.0f, 1.0f);
	SDL_Colour background_colour = { 0, 0, 0, 192 };
	modal_main_widget->set_background_colour(background_colour);

	Widget_Window *window = new Widget_Window(100, 50);
	window->set_center_x(true);
	window->set_center_y(true);
	window->set_parent(modal_main_widget);

	Widget_Label *label = new Widget_Label(text, window->get_width() - 10);
	label->set_padding(5);
	label->set_center_x(true);
	label->set_parent(window);

	yes_button = new Widget_Text_Button(noo.t->translate(12));
	yes_button->set_center_x(true);
	yes_button->set_padding_right(2);

	no_button = new Widget_Text_Button(noo.t->translate(6));
	no_button->set_center_x(true);
	no_button->set_padding_left(2);

	Widget *button_container = new Widget(1.0f, yes_button->get_height());
	button_container->set_float_bottom(true);
	button_container->set_padding_bottom(5);
	button_container->set_parent(window);

	yes_button->set_parent(button_container);
	no_button->set_parent(button_container);

	gui = new TGUI(modal_main_widget, noo.screen_size.w, noo.screen_size.h);
}

void Yes_No_GUI::update()
{
	if (yes_button->pressed()) {
		callback((void *)1);
		exit();
	}
	else if (no_button->pressed()) {
		callback(0);
		exit();
	}
}

//--

Yes_No_Always_GUI::Yes_No_Always_GUI(std::string text, int milestone, Callback callback) :
	milestone(milestone),
	callback(callback)
{
	Widget *modal_main_widget = new Widget(1.0f, 1.0f);
	SDL_Colour background_colour = { 0, 0, 0, 192 };
	modal_main_widget->set_background_colour(background_colour);

	Widget_Window *window = new Widget_Window(100, 50);
	window->set_center_x(true);
	window->set_center_y(true);
	window->set_parent(modal_main_widget);

	Widget_Label *label = new Widget_Label(text, window->get_width() - 10);
	label->set_padding(5);
	label->set_center_x(true);
	label->set_parent(window);

	yes_button = new Widget_Text_Button(noo.t->translate(12));
	yes_button->set_center_x(true);
	yes_button->set_padding_right(2);

	no_button = new Widget_Text_Button(noo.t->translate(6));
	no_button->set_center_x(true);
	no_button->set_padding_left(2);
	no_button->set_padding_right(2);

	always_button = new Widget_Text_Button(TRANSLATE("Always")END);
	always_button->set_center_x(true);
	always_button->set_padding_left(2);

	Widget *button_container = new Widget(1.0f, yes_button->get_height());
	button_container->set_float_bottom(true);
	button_container->set_padding_bottom(5);
	button_container->set_parent(window);

	yes_button->set_parent(button_container);
	no_button->set_parent(button_container);
	always_button->set_parent(button_container);

	gui = new TGUI(modal_main_widget, noo.screen_size.w, noo.screen_size.h);
}

void Yes_No_Always_GUI::update()
{
	if (yes_button->pressed()) {
		callback((void *)1);
		exit();
	}
	else if (no_button->pressed()) {
		callback(0);
		exit();
	}
	else if (always_button->pressed()) {
		noo.set_milestone(milestone, true);
		callback((void *)1);
		exit();
	}
}

//--

Get_Number_GUI::Get_Number_GUI(std::string text, int stops, int initial_value, Callback callback) :
	callback(callback)
{
	Widget *modal_main_widget = new Widget(1.0f, 1.0f);
	SDL_Colour background_colour = { 0, 0, 0, 192 };
	modal_main_widget->set_background_colour(background_colour);

	Widget_Window *window = new Widget_Window(110, 60);
	window->set_center_x(true);
	window->set_center_y(true);
	window->set_parent(modal_main_widget);

	Widget_Label *label = new Widget_Label(text, window->get_width() - 10);
	label->set_padding(5);
	label->set_center_x(true);
	label->set_parent(window);

	slider = new Widget_Slider(100, stops, initial_value);
	slider->set_center_x(true);
	slider->set_clear_float_x(true);
	slider->set_break_line(true);
	slider->set_parent(window);

	value_label = new Widget_Label("", 50);
	value_label->set_padding_top(5);
	value_label->set_center_x(true);
	value_label->set_clear_float_x(true);
	value_label->set_break_line(true);
	value_label->set_parent(window);

	ok_button = new Widget_Text_Button(TRANSLATE("OK")END);
	ok_button->set_center_x(true);
	ok_button->set_padding_right(2);

	slider->set_down_widget(ok_button);

	cancel_button = new Widget_Text_Button(TRANSLATE("Cancel")END);
	cancel_button->set_center_x(true);
	cancel_button->set_padding_left(2);

	Widget *button_container = new Widget(1.0f, ok_button->get_height());
	button_container->set_float_bottom(true);
	button_container->set_padding_bottom(5);
	button_container->set_parent(window);

	ok_button->set_parent(button_container);
	cancel_button->set_parent(button_container);

	gui = new TGUI(modal_main_widget, noo.screen_size.w, noo.screen_size.h);

	gui->set_focus(slider);
}

void Get_Number_GUI::update()
{
	value_label->set_text(itos(slider->get_value()));

	gui->layout();

	if (ok_button->pressed()) {
		callback((void *)(int64_t)slider->get_value());
		exit();
	}
	else if (cancel_button->pressed()) {
		callback((void *)-1);
		exit();
	}
}

//--

Save_Load_GUI::Save_Load_GUI(bool saving, Callback callback) :
	saving(saving),
	callback(callback)
{
	std::string caption;

	if (saving) {
		std::string filename;
#ifdef __APPLE__
		filename = SDL_GetBasePath();
		const char *p = filename.c_str();
		const char *p2 = strstr(p, ".app");
		if (p2) {
			filename = filename.substr(0, p2-p + 4) + "/../test.save";
		}
		else {
			filename = ""; // error
		}
#else
		filename = "test.save";
#endif

		SDL_RWops *file = SDL_RWFromFile(filename.c_str(), "w");

		if (file == 0 || noo.save_game(file) == false) {
			if (callback) callback((void *)ERR);
			caption = noo.t->translate(2);
		}
		else {
			if (callback) callback((void *)SAVE);
			caption = noo.t->translate(3);
		}

		if (file != 0) {
			SDL_RWclose(file);
		}
	}
	else {
		if (callback) callback((void *)LOAD);
	}

	if (caption != "") {
		Widget *modal_main_widget = new Widget(1.0f, 1.0f);
		SDL_Colour background_colour = { 0, 0, 0, 192 };
		modal_main_widget->set_background_colour(background_colour);

		Widget_Window *window = new Widget_Window(100, 50);
		window->set_center_x(true);
		window->set_center_y(true);
		window->set_parent(modal_main_widget);

		Widget_Label *label = new Widget_Label(caption, window->get_width() - 10);
		label->set_padding(5);
		label->set_center_x(true);
		label->set_parent(window);

		ok_button = new Widget_Text_Button(noo.t->translate(7));
		ok_button->set_center_x(true);
		ok_button->set_float_bottom(true);
		ok_button->set_padding_bottom(5);
		ok_button->set_parent(window);

		gui = new TGUI(modal_main_widget, noo.screen_size.w, noo.screen_size.h);
	}
 	else {
		gui = 0;
 	}
}

void Save_Load_GUI::update()
{
	if (gui == 0 || ok_button->pressed()) {
		exit();
	}
}

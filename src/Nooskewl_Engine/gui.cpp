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

bool Pause_GUI::quitting;
bool Pause_GUI::quit;

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

bool GUI::update()
{
	return true;
}

bool GUI::update_background()
{
	return true;
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

bool GUI::do_return(bool ret)
{
	if (fade == false) {
		return ret;
	}

	if (ret == false) {
		if (fading_out == false) {
			fading_out = true;
			fade_start = SDL_GetTicks();
		}
	}

	if (fading_out && (SDL_GetTicks()-fade_start) >= 200) {
		fading_out = false;
		if (fade_done(false) == false) {
			return false;
		}
	}

	return true;
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

bool Title_GUI::update()
{
	if (check_loaded() == false) {
		return do_return(false);
	}

	if (new_game_button->pressed()) {
		do_new_game = true;

		return do_return(false);
	}
	else if (load_game_button->pressed()) {
		do_new_game = false;

		loading = true;
		loaded = false;

		Save_Load_GUI *save_load_gui = new Save_Load_GUI(false, callback);
		save_load_gui->start();
		noo.guis.push_back(save_load_gui);
	}

	return do_return(true);
}

bool Title_GUI::update_background()
{
	return do_return(check_loaded());
}

void Title_GUI::draw_back()
{
	int scale1 = 4;
	int x1 = -static_logo->size.w / 2 * scale1;
	int y1 = -static_logo->size.h / 2 * scale1;
	int scale2 = 8;
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
		if (do_new_game) {
			Map::new_game_started();

			noo.player = new Map_Entity("player");
			noo.player->set_brain(new Player_Brain());
			noo.player->load_sprite("player");
			noo.player->load_stats("player");
			noo.map = new Map("start.map");
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

				bool result = noo.load_game(file);

				if (result == true) {
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

void Pause_GUI::callback(void *data)
{
	if (quitting) {
		quitting = false;
		quit = data != 0;
	}
}

Pause_GUI::Pause_GUI() :
	exit_menu(false)
{
	quitting = quit = false;

	Widget *modal_main_widget = new Widget(1.0f, 1.0f);
	SDL_Colour background_colour = { 0, 0, 0, 192 };
	modal_main_widget->set_background_colour(background_colour);

	Widget_Window *window = new Widget_Window(0.95f, 0.95f);
	window->set_center_x(true);
	window->set_center_y(true);
	window->set_padding(5);
	window->set_parent(modal_main_widget);

	TGUI_Widget *pad = new TGUI_Widget(0.95f, 0.95f);
	pad->set_center_x(true);
	pad->set_center_y(true);
	pad->set_parent(window);

	resume_button = new Widget_Text_Button(noo.t->translate(10), 0.3f, -1);
	resume_button->set_center_x(true);

	save_button = new Widget_Text_Button(noo.t->translate(11), 0.3f, -1);
	save_button->set_center_x(true);
	save_button->set_padding_left(5);
	save_button->set_padding_right(5);

	quit_button = new Widget_Text_Button(noo.t->translate(8), 0.3f, -1);
	quit_button->set_center_x(true);

	Widget *button_container = new Widget(1.0f, quit_button->get_height());
	button_container->set_float_bottom(true);
	button_container->set_parent(pad);

	resume_button->set_parent(button_container);
	save_button->set_parent(button_container);
	quit_button->set_parent(button_container);

	TGUI_Widget *column1 = new TGUI_Widget(TGUI_Widget::FIT_Y, 0.3f);
	column1->set_center_x(true);
	column1->set_center_y(true);
	column1->set_padding_bottom(resume_button->get_height() + 5);
	column1->set_parent(pad);

	TGUI_Widget *column2 = new TGUI_Widget(TGUI_Widget::FIT_Y, 0.3f);
	column2->set_center_x(true);
	column2->set_center_y(true);
	column2->set_padding_left(5);
	column2->set_padding_right(5);
	column2->set_padding_bottom(resume_button->get_height() + 5);
	column2->set_parent(pad);

	stats = noo.map->get_entity(0)->get_stats();

	if (stats->profile_pic != 0) {
		profile_image = new Widget_Image(stats->profile_pic, false);
		profile_image->set_padding_bottom(2);
		profile_image->set_parent(column1);
	}

	name = new Widget_Label("", -1);
	name->set_break_line(true);
	name->set_parent(column1);

	alignment_label = new Widget_Label(TRANSLATE("Align: ")END, -1);
	alignment_label->set_break_line(true);
	alignment_label->set_padding_top((int)noo.font->get_height()+2);
	alignment_label->set_parent(column1);

	alignment = new Widget_Label("", -1);
	alignment->set_padding_top((int)noo.font->get_height()+2);
	alignment->set_parent(column1);

	sex_label = new Widget_Label(TRANSLATE("Sex: ")END, -1);
	sex_label->set_break_line(true);
	sex_label->set_parent(column1);

	sex = new Widget_Label("", -1);
	sex->set_parent(column1);

	hp_label = new Widget_Label(TRANSLATE("HP: ")END, -1);
	hp_label->set_break_line(true);
	hp_label->set_padding_top((int)noo.font->get_height()+2);
	hp_label->set_parent(column1);

	hp = new Widget_Label("", -1);
	hp->set_padding_top((int)noo.font->get_height()+2);
	hp->set_parent(column1);

	mp_label = new Widget_Label(TRANSLATE("MP: ")END, -1);
	mp_label->set_break_line(true);
	mp_label->set_parent(column1);

	mp = new Widget_Label("", -1);
	mp->set_parent(column1);

	experience_label = new Widget_Label(TRANSLATE("Exp: ")END, -1);
	experience_label->set_break_line(true);
	experience_label->set_parent(column1);

	experience = new Widget_Label("", -1);
	experience->set_parent(column1);

	weapon_label = new Widget_Label(TRANSLATE("Weapon: ")END, -1);
	weapon_label->set_break_line(true);
	weapon_label->set_padding_top((int)noo.font->get_height()+2);
	weapon_label->set_parent(column1);

	weapon = new Widget_Label("", -1);
	weapon->set_padding_top((int)noo.font->get_height()+2);
	weapon->set_parent(column1);

	armour_label = new Widget_Label(TRANSLATE("Armour: ")END, -1);
	armour_label->set_break_line(true);
	armour_label->set_parent(column1);

	armour = new Widget_Label("", -1);
	armour->set_parent(column1);

	attack_label = new Widget_Label(TRANSLATE("Attack: ")END, -1);
	attack_label->set_break_line(true);
	attack_label->set_padding_top(18);
	attack_label->set_parent(column2);

	attack = new Widget_Label("", -1);
	attack->set_padding_top(18);
	attack->set_parent(column2);

	defense_label = new Widget_Label(TRANSLATE("Defense: ")END, -1);
	defense_label->set_break_line(true);
	defense_label->set_parent(column2);

	defense = new Widget_Label("", -1);
	defense->set_parent(column2);

	agility_label = new Widget_Label(TRANSLATE("Agility: ")END, -1);
	agility_label->set_break_line(true);
	agility_label->set_parent(column2);

	agility = new Widget_Label("", -1);
	agility->set_parent(column2);

	luck_label = new Widget_Label(TRANSLATE("Luck: ")END, -1);
	luck_label->set_break_line(true);
	luck_label->set_parent(column2);

	luck = new Widget_Label("", -1);
	luck->set_parent(column2);

	speed_label = new Widget_Label(TRANSLATE("Speed: ")END, -1);
	speed_label->set_break_line(true);
	speed_label->set_parent(column2);

	speed = new Widget_Label("", -1);
	speed->set_parent(column2);

	strength_label = new Widget_Label(TRANSLATE("Strength: ")END, -1);
	strength_label->set_break_line(true);
	strength_label->set_parent(column2);

	strength = new Widget_Label("", -1);
	strength->set_parent(column2);

	karma_label = new Widget_Label(TRANSLATE("Karma: ")END, -1);
	karma_label->set_break_line(true);
	karma_label->set_parent(column2);

	karma = new Widget_Label("", -1);
	karma->set_parent(column2);

	hunger_label = new Widget_Label(TRANSLATE("Hunger: ")END, -1);
	hunger_label->set_break_line(true);
	hunger_label->set_parent(column2);

	hunger = new Widget_Label("", -1);
	hunger->set_parent(column2);

	thirst_label = new Widget_Label(TRANSLATE("Thirst: ")END, -1);
	thirst_label->set_break_line(true);
	thirst_label->set_parent(column2);

	thirst = new Widget_Label("", -1);
	thirst->set_parent(column2);

	rest_label = new Widget_Label(TRANSLATE("Rest: ")END, -1);
	rest_label->set_break_line(true);
	rest_label->set_parent(column2);

	rest = new Widget_Label("", -1);
	rest->set_parent(column2);

	sobriety_label = new Widget_Label(TRANSLATE("Sobriety: ")END, -1);
	sobriety_label->set_break_line(true);
	sobriety_label->set_parent(column2);

	sobriety = new Widget_Label("", -1);
	sobriety->set_parent(column2);

	int max_w = 0;
	
	max_w = MAX(max_w, alignment_label->get_width());
	max_w = MAX(max_w, sex_label->get_width());
	max_w = MAX(max_w, hp_label->get_width());
	max_w = MAX(max_w, mp_label->get_width());
	max_w = MAX(max_w, experience_label->get_width());
	max_w = MAX(max_w, weapon_label->get_width());
	max_w = MAX(max_w, armour_label->get_width());

	alignment_label->set_width(max_w);
	sex_label->set_width(max_w);
	hp_label->set_width(max_w);
	mp_label->set_width(max_w);
	experience_label->set_width(max_w);
	weapon_label->set_width(max_w);
	armour_label->set_width(max_w);

	max_w = 0;
	max_w = MAX(max_w, attack_label->get_width());
	max_w = MAX(max_w, defense_label->get_width());
	max_w = MAX(max_w, agility_label->get_width());
	max_w = MAX(max_w, luck_label->get_width());
	max_w = MAX(max_w, speed_label->get_width());
	max_w = MAX(max_w, strength_label->get_width());
	max_w = MAX(max_w, karma_label->get_width());
	max_w = MAX(max_w, hunger_label->get_width());
	max_w = MAX(max_w, thirst_label->get_width());
	max_w = MAX(max_w, rest_label->get_width());
	max_w = MAX(max_w, sobriety_label->get_width());

	attack_label->set_width(max_w);
	defense_label->set_width(max_w);
	agility_label->set_width(max_w);
	luck_label->set_width(max_w);
	speed_label->set_width(max_w);
	strength_label->set_width(max_w);
	karma_label->set_width(max_w);
	hunger_label->set_width(max_w);
	thirst_label->set_width(max_w);
	rest_label->set_width(max_w);
	sobriety_label->set_width(max_w);

	set_labels();

	std::vector<TGUI_Widget *> &v = column2->get_children();
	int height = 0;

	for (size_t i = 0; i < v.size(); i++) {
		TGUI_Widget *w = v[i];
		if (w->get_break_line()) {
			height += w->get_height() + w->get_padding_top() + w->get_padding_bottom();
		}
	}

	TGUI_Widget *column3 = new TGUI_Widget(0.3f, height);
	column3->set_center_x(true);
	column3->set_center_y(true);
	column3->set_padding_bottom(resume_button->get_height() + 5);
	column3->set_parent(pad);

	items_button = new Widget_Text_Button(TRANSLATE("Items")END, 1.0f, -1);
	items_button->set_parent(column3);

	weapons_button = new Widget_Text_Button(TRANSLATE("Weapons")END, 1.0f, -1);
	weapons_button->set_padding_top(2);
	weapons_button->set_padding_bottom(2);
	weapons_button->set_parent(column3);

	armour_button = new Widget_Text_Button(TRANSLATE("Armour")END, 1.0f, -1);
	armour_button->set_parent(column3);

	gui = new TGUI(modal_main_widget, noo.screen_size.w, noo.screen_size.h);

	gui->set_focus(resume_button);
}

void Pause_GUI::handle_event(TGUI_Event *event)
{
	if (
		(event->type == TGUI_KEY_DOWN && event->keyboard.code == TGUIK_ESCAPE) ||
		(event->type== TGUI_JOY_DOWN && event->joystick.button == noo.joy_b1)) {
		noo.button_mml->play(false);
		exit_menu = true;
	}
	else {
		GUI::handle_event(event);
	}
}

bool Pause_GUI::update()
{
	if (exit_menu) {
		return do_return(false);
	}
	else if (check_quit() == false) {
		return do_return(false);
	}

	if (resume_button->pressed()) {
		return do_return(false);
	}
	else if (save_button->pressed()) {
		Save_Load_GUI *save_load_gui = new Save_Load_GUI(true);
		save_load_gui->start();
		noo.guis.push_back(save_load_gui);
	}
	else if (quit_button->pressed()) {
		quitting = true;
		quit = false;

		Yes_No_GUI *yes_no_gui = new Yes_No_GUI(noo.t->translate(9), callback);
		yes_no_gui->start();
		noo.guis.push_back(yes_no_gui);
	}

	if (items_button->pressed()) {
		Items_GUI *items_gui = new Items_GUI();
		items_gui->start();
		noo.guis.push_back(items_gui);
	}

	return do_return(true);
}

bool Pause_GUI::update_background()
{
	return do_return(check_quit());
}

bool Pause_GUI::check_quit()
{
	if (quit) {
		return false;
	}

	return true;
}

void Pause_GUI::set_labels()
{
	if (stats == 0) {
		return;
	}

	name->set_text(TRANSLATE("Eny")END); // FIXME

	if (stats->alignment == Stats::GOOD) {
		alignment->set_text(TRANSLATE("Good")END);
	}
	else if (stats->alignment == Stats::EVIL) {
		alignment->set_text(TRANSLATE("Evil")END);
	}
	else {
		alignment->set_text(TRANSLATE("Neutral")END);
	}

	if (stats->sex == Stats::MALE) {
		sex->set_text(TRANSLATE("Male")END);
	}
	else if (stats->sex == Stats::FEMALE) {
		sex->set_text(TRANSLATE("Female")END);
	}
	else {
		sex->set_text(TRANSLATE("Unknown")END);
	}

	hp->set_text(string_printf("%d/%d", stats->hp, stats->max_hp));
	mp->set_text(string_printf("%d/%d", stats->mp, stats->max_mp));
	experience->set_text(string_printf("%d", stats->experience));

	if (stats->weapon != 0) {
		weapon->set_text(stats->weapon->name);
	}
	if (stats->armour != 0) {
		armour->set_text(stats->armour->name);
	}

	attack->set_text(string_printf("%d", stats->attack));
	defense->set_text(string_printf("%d", stats->defense));
	agility->set_text(string_printf("%d", stats->agility));
	luck->set_text(string_printf("%d", stats->luck));
	speed->set_text(string_printf("%d", stats->speed));
	strength->set_text(string_printf("%d", stats->strength));
	
	karma->set_text(string_printf("%d%%", int((((float)stats->karma / 0xffff) * 2.0f - 1.0f) * 100)));
	hunger->set_text(string_printf("%d%%", int((((float)stats->hunger / 0xffff) * 2.0f - 1.0f) * 100)));
	thirst->set_text(string_printf("%d%%", int((((float)stats->thirst / 0xffff) * 2.0f - 1.0f) * 100)));
	rest->set_text(string_printf("%d%%", int((((float)stats->rest / 0xffff) * 2.0f - 1.0f) * 100)));
	sobriety->set_text(string_printf("%d%%", int((((float)stats->sobriety / 0xffff) * 2.0f - 1.0f) * 100)));
}

bool Pause_GUI::fade_done(bool fading_in) {
	if (fading_in == false) {
		if (quit) {
			delete noo.map;
			noo.map = 0;
			delete noo.player;
			noo.player = 0;
			noo.last_map_name = "";

			Title_GUI *title_gui = new Title_GUI();
			title_gui->start();
			noo.guis.push_back(title_gui);
		}
	}

	return false;
}

//--

Items_GUI::Items_GUI() :
	exit_menu(false)
{
	Widget *modal_main_widget = new Widget(1.0f, 1.0f);
	SDL_Colour background_colour = { 0, 0, 0, 192 };
	modal_main_widget->set_background_colour(background_colour);

	Widget_Window *window = new Widget_Window(0.95f, 0.95f);
	window->set_center_x(true);
	window->set_center_y(true);
	window->set_padding(5);
	window->set_parent(modal_main_widget);

	TGUI_Widget *pad = new TGUI_Widget(0.95f, 0.95f);
	pad->set_center_x(true);
	pad->set_center_y(true);
	pad->set_parent(window);

	list = new Widget_List(0.4f, 1.0f);
	std::vector<std::string> &v = list->get_items();
	v.push_back("A thing");
	v.push_back("Another thing");
	v.push_back("Another thing");
	v.push_back("A thing");
	v.push_back("Crap");
	v.push_back("Junk");
	v.push_back("Hogwash");
	v.push_back("Boloney");
	v.push_back("Sandwhich");
	v.push_back("Turkey");
	v.push_back("Foo");
	v.push_back("Bologna");
	v.push_back("Ham");
	v.push_back("Crapola");
	v.push_back("Turks");
	v.push_back("Big Turks");
	v.push_back("Small Turks");
	v.push_back("Big Macs");
	v.push_back("Small Macs");
	v.push_back("Crap");
	v.push_back("More crap");
	v.push_back("Even more crap");
	v.push_back("Turk");
	v.push_back("Hamwhich");
	v.push_back("Bologna samwhich");
	v.push_back("Beans");
	v.push_back("Onion sandwhich");
	list->set_parent(pad);

	TGUI_Widget *info = new TGUI_Widget(0.4f, 1.0f);
	info->set_parent(pad);

	done_button = new Widget_Text_Button(TRANSLATE("Done")END, -1, -1);
	done_button->set_parent(pad);

	gui = new TGUI(modal_main_widget, noo.screen_size.w, noo.screen_size.h);

	gui->set_focus(list);
}

void Items_GUI::handle_event(TGUI_Event *event)
{
	if ((event->type == TGUI_KEY_DOWN && event->keyboard.code == TGUIK_ESCAPE) ||
		(event->type== TGUI_JOY_DOWN && event->joystick.button == noo.joy_b1)) {

		noo.button_mml->play(false);
		exit_menu = true;
	}
	else {
		GUI::handle_event(event);
	}
}

bool Items_GUI::update()
{
	if (done_button->pressed() || exit_menu) {
		return do_return(false);
	}

	return do_return(true);
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

bool Notification_GUI::update()
{
	if (ok_button->pressed()) {
		return do_return(false);
	}
	return do_return(true);
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

bool Yes_No_GUI::update()
{
	if (yes_button->pressed()) {
		callback((void *)1);
		return do_return(false);
	}
	else if (no_button->pressed()) {
		callback(0);
		return do_return(false);
	}
	return do_return(true);
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

bool Save_Load_GUI::update()
{
	if (gui == 0) {
		return do_return(false);
	}
	else {
		return do_return(!ok_button->pressed());
	}
}

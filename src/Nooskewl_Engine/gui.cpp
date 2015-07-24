#include "Nooskewl_Engine/brain.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/gui.h"
#include "Nooskewl_Engine/image.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/player_brain.h"
#include "Nooskewl_Engine/shader.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/tokenizer.h"
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
}

GUI::~GUI()
{
	delete gui;
}

bool GUI::update()
{
	return true;
}

bool GUI::update_background()
{
	return true;
}

//--

void Title_GUI::callback(void *data)
{
	if (loading) {
		loading = false;
		loaded = data != 0;
	}
}

Title_GUI::Title_GUI() :
	did_intro(false),
	intro_start(SDL_GetTicks())
{
	loading = loaded = false;

	noo.play_music("title.mml");

	logo = new Image("logo.tga");

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
	delete logo;
}

bool Title_GUI::update()
{
	if (check_loaded() == false) {
		return false;
	}

	if (new_game_button->pressed()) {
		Map::new_game_started();

		noo.player = new Map_Entity("player");
		noo.player->set_brain(new Player_Brain());
		noo.player->load_sprite("player");
		noo.map = new Map("start.map");
		noo.map->add_entity(noo.player);
		noo.map->start();
		noo.map->update_camera();

		return false;
	}
	else if (load_game_button->pressed()) {
		loading = true;
		loaded = false;
		noo.guis.push_back(new Save_Load_GUI(false, callback));
	}

	return true;
}

bool Title_GUI::update_background()
{
	return check_loaded();
}

void Title_GUI::draw_back()
{
	int scale1 = 4;
	int x1 = -logo->size.w / 2 * scale1;
	int y1 = -logo->size.h / 2 * scale1;
	int scale2 = 8;
	int x2 = -logo->size.w / 4 * scale2;
	int y2 = -logo->size.h / 4 * scale2;
	int dx1 = x1 + logo->size.w / 2 * scale1;
	int dy2 = y2 + logo->size.h / 4 * scale2;
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
	logo->stretch_region_tinted_single(tint1, Point<float>(0, 0), logo->size, Point<float>(x1 + (dx1 - x1) * p1, (float)y1), logo->size * scale1);
	SDL_Colour tint2 = noo.white;
	tint2.a = 64;
	logo->stretch_region_tinted_single(tint2, Point<float>(0, 0), logo->size, Point<float>((float)x2, y2 + (dy2 - y2) * p2), logo->size * scale2);
}

void Title_GUI::draw_fore()
{
	Point<float> pos;
	Size<int> size;

	if (did_intro == false) {
		int max_w = logo->size.w * 16 - logo->size.w;
		int max_h = logo->size.h * 16 - logo->size.h;
		float p = (SDL_GetTicks() - intro_start) / 2000.0f;
		if (p > 1.0f) {
			p = 1.0f;
			did_intro = true;
		}
		float w = (1.0f - p) * max_w + logo->size.w;
		float h = (1.0f - p) * max_h + logo->size.h;
		size = Size<int>((int)w, (int)h);
		pos.x = noo.screen_size.w / 2 - w / 2;
		pos.y = noo.screen_size.h / 3 - h / 2;
	}
	else {
		pos.x = float(noo.screen_size.w / 2 - logo->size.w / 2);
		pos.y = float(noo.screen_size.h / 3 - logo->size.h / 2);
		size = logo->size;
	}

	float percent = (SDL_GetTicks() % 500) / 500.0f * 2.0f;
	if (percent >= 1.0f) {
		percent = 1.0f - (percent - 1.0f);
	}
	float amount_x = (1.0f / noo.scale) * cos((SDL_GetTicks() % 5000) / 5000.0f * (float)M_PI * 2);
	float amount_y = (1.0f / noo.scale) * sin((SDL_GetTicks() % 3000) / 5000.0f * (float)M_PI * 2);
	Shader *bak = noo.current_shader;
	noo.current_shader = noo.glitch_shader;
	noo.current_shader->use();
	noo.current_shader->set_float("percent", percent);
	noo.current_shader->set_float("width", (float)logo->size.w);
	noo.current_shader->set_float("height", (float)logo->size.h);
	noo.current_shader->set_float("amount_x", amount_x);
	noo.current_shader->set_float("amount_y", amount_y);
	logo->stretch_region_single(Point<float>(0.0f, 0.0f), logo->size, pos, size);
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

//--

void Pause_GUI::callback(void *data)
{
	if (quitting) {
		quitting = false;
		quit = data != 0;
	}
}

Pause_GUI::Pause_GUI()
{
	quitting = quit = false;

	Widget *modal_main_widget = new Widget(1.0f, 1.0f);
	SDL_Colour background_colour = { 0, 0, 0, 192 };
	modal_main_widget->set_background_colour(background_colour);

	Widget_Window *window = new Widget_Window(0.95f, 0.95f);
	window->set_center_x(true);
	window->set_center_y(true);
	window->set_parent(modal_main_widget);

	Widget_Label *label = new Widget_Label(noo.t->translate(0), modal_main_widget->get_width()-10);
	label->set_padding(5);
	label->set_center_x(true);
	label->set_parent(window);

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
	button_container->set_padding_bottom(5);
	button_container->set_parent(window);

	resume_button->set_parent(button_container);
	save_button->set_parent(button_container);
	quit_button->set_parent(button_container);

	gui = new TGUI(modal_main_widget, noo.screen_size.w, noo.screen_size.h);

	gui->set_focus(resume_button);
}

bool Pause_GUI::update()
{
	if (check_quit() == false) {
		return false;
	}

	if (resume_button->pressed()) {
		return false;
	}
	else if (save_button->pressed()) {
		noo.guis.push_back(new Save_Load_GUI(true));
	}
	else if (quit_button->pressed()) {
		quitting = true;
		quit = false;

		noo.guis.push_back(new Yes_No_GUI(noo.t->translate(9), callback));
	}

	return true;
}

bool Pause_GUI::update_background()
{
	return check_quit();
}

bool Pause_GUI::check_quit()
{
	if (quit) {
		quit = false;

		delete noo.map;
		noo.map = 0;
		delete noo.player;
		noo.player = 0;
		noo.last_map_name = "";

		noo.guis.push_back(new Title_GUI());

		return false;
	}

	return true;
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
		return false;
	}
	return true;
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
		return false;
	}
	else if (no_button->pressed()) {
		callback(0);
		return false;
	}
	return true;
}

//--

Save_Load_GUI::Save_Load_GUI(bool saving, Callback callback) :
	saving(saving),
	callback(callback)
{
	SDL_RWops *file;
	std::string caption = "";

	if (saving) {
		file = SDL_RWFromFile("test.save", "w");
		if (file == 0 || save_game(file) == false) {
			if (callback) callback(0);
			caption = noo.t->translate(2);
		}
		else {
			if (callback) callback((void *)1);
			caption = noo.t->translate(3);
		}
	}
	else {
		bool result;
		file = SDL_RWFromFile("test.save", "r");
		if (file == NULL) {
			result = false;
		}
		else {
			Map::new_game_started();

			result = load_game(file);
		}

		if (result == true) {
			if (callback) callback((void *)1);

			noo.last_map_name = "--LOADED--";

			noo.map->start();
			noo.map->update_camera();
		}
		else {
			if (noo.map) {
				delete noo.map;
				noo.map = 0;
			}
			if (callback) callback(0);
			caption = noo.t->translate(1);
		}
	}

	if (file) {
		SDL_RWclose(file);
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
		return false;
	}

	if (ok_button->pressed()) {
		return false;
	}

	return true;
}

bool Save_Load_GUI::save_game(SDL_RWops *file)
{
	SDL_fprintf(file, "version=100\n");
	if (save_milestones(file) == false) {
		return false;
	}
	return noo.map->save(file);
}

bool Save_Load_GUI::save_milestones(SDL_RWops *file)
{
	int num_milestones = noo.get_num_milestones();
	SDL_fprintf(file, "num_milestones=%d\n", num_milestones);
	for (int i = 0; i < num_milestones; i++) {
		SDL_fprintf(file, "%d=%d\n", i, noo.check_milestone(i) == true ? 1 : 0);
	}
	return true;
}

bool Save_Load_GUI::load_game(SDL_RWops *file)
{
	char line[1000];
	SDL_fgets(file, line, 1000);
	std::string s = line;
	trim(s);
	Tokenizer t(s, '=');
	std::string tag = t.next();
	if (tag != "version") {
		errormsg("Missing version in save state\n");
		return false;
	}
	std::string version = t.next();
	// Do something with version
	if (load_milestones(file) == false) {
		return false;
	}
	return load_map(file);
}

bool Save_Load_GUI::load_milestones(SDL_RWops *file)
{
	char line[1000];
	SDL_fgets(file, line, 1000);
	std::string s = line;
	trim(s);
	Tokenizer t(s, '=');
	std::string tag = t.next();
	std::string value = t.next();
	if (tag != "num_milestones") {
		errormsg("Expected num_milestones in save state\n");
		return false;
	}

	int num_milestones = atoi(value.c_str());

	for (int i = 0; i < num_milestones; i++) {
		SDL_fgets(file, line, 1000);
		s = line;
		trim(s);
		Tokenizer t(s, '=');
		std::string tag = t.next();
		std::string value = t.next();
		if (atoi(tag.c_str()) != i) {
			errormsg("Expected milestone %d, got '%s'\n", i, tag.c_str());
			return false;
		}
		bool onoff = atoi(value.c_str()) != 0;
		noo.set_milestone(i, onoff);
	}

	return true;
}

bool Save_Load_GUI::load_map(SDL_RWops *file)
{
	char line[1000];
	SDL_fgets(file, line, 1000);
	std::string s = line;
	trim(s);
	Tokenizer t(s, '=');
	std::string tag = t.next();
	std::string value = t.next();
	trim(value);

	if (tag != "map_name") {
		errormsg("Expected map_name in save state\n");
		return false;
	}

	noo.map = new Map(value);

	SDL_fgets(file, line, 1000);
	s = line;
	trim(s);
	t = Tokenizer(s, '=');
	tag = t.next();
	value = t.next();

	if (tag != "num_entities") {
		errormsg("Expected num_entities in save state\n");
		return false;
	}
	int num_entities = atoi(value.c_str());
	if (num_entities < 1) {
		errormsg("Expected at least 1 entity in save state\n");
		return false;
	}

	noo.player = load_entity(file);
	if (noo.player == 0) {
		return false;
	}
	else if (noo.player->get_name() != "player") {
		errormsg("Expected player first in save state\n");
		return false;
	}

	noo.map->add_entity(noo.player);

	for (int i = 1; i < num_entities; i++) {
		Map_Entity *entity = load_entity(file);
		if (entity == 0) {
			return false;
		}
		noo.map->add_entity(entity);
	}

	return true;
}

Map_Entity *Save_Load_GUI::load_entity(SDL_RWops *file)
{
	Brain *brain = load_brain(file);

	char line[1000];
	SDL_fgets(file, line, 1000);
	std::string s = line;
	trim(s);
	Tokenizer t(s, '=');
	std::string name = t.next();
	std::string options = s.substr(name.length()+1);

	t = Tokenizer(options, ',');
	std::string option;

	Map_Entity *entity = new Map_Entity(name);
	entity->set_brain(brain);

	while ((option = t.next()) != "") {
		Tokenizer t2(option, '=');
		std::string key = t2.next();
		std::string value = t2.next();
		if (key == "position") {
			Tokenizer t3(value, ':');
			std::string x_s = t3.next();
			std::string y_s = t3.next();
			entity->set_position(Point<int>(atoi(x_s.c_str()), atoi(y_s.c_str())));
		}
		else if (key == "direction") {
			entity->set_direction((Direction)atoi(value.c_str()));
		}
		else if (key == "sitting") {
			entity->set_sitting(atoi(value.c_str()) != 0);
		}
		else if (key == "sprite") {
			Tokenizer t3(value, ':');
			std::string xml_filename = t3.next();
			std::string image_directory = t3.next();
			std::string animation = t3.next();
			bool started = atoi(t3.next().c_str()) != 0;
			Sprite *sprite = new Sprite(xml_filename, image_directory, true);
			sprite->set_animation(animation);
			if (started) {
				sprite->start();
			}
			else {
				sprite->stop();
			}
			entity->set_sprite(sprite);
		}
		else if (key == "z_add") {
			entity->set_z_add(atoi(value.c_str()));
		}
		else {
			infomsg("Unknown token in entity in save state '%s'\n", key.c_str());
		}
	}

	return entity;
}

Brain *Save_Load_GUI::load_brain(SDL_RWops *file)
{
	char line[1000];
	SDL_fgets(file, line, 1000);
	std::string s = line;
	trim(s);
	Tokenizer t(s, '=');
	std::string tag = t.next();
	std::string value = t.next();
	if (tag != "brain") {
		errormsg("Expected brain in save state\n");
		return 0;
	}

	t = Tokenizer(value, ',');

	std::string type = t.next();

	Brain *brain;

	if (type == "0") {
		return 0;
	}
	else if (type == "player_brain") {
		brain = new Player_Brain();
	}
	else {
		return m.dll_get_brain(value);
	}

	return brain;
}

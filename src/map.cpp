#include "log.h"
#include "map.h"
#include "script_functions.h"
#include "video.h"

Map::Map(std::string map_name) :
	offset(0, 0),
	speech(NULL),
	script_func(NULL),
	map_name(map_name),
	new_map_name("")
{
	tilemap = new Tilemap(8, map_name);
}

Map::~Map()
{
	delete tilemap;

	for (size_t i = 0; i < entities.size(); i++) {
		if (entities[i]->get_id() != 0) {
			delete entities[i];
		}
	}
}

void Map::start()
{
	if (map_name == "test.map") {
		script_func = sf_test;
	}
	else if (map_name == "test2.map") {
		script_func = sf_test2;
	}

	if (script_func != NULL) {
		script_func(SCRIPT_BEGIN, this, &script_data, NULL);
	}
}

void Map::end()
{
	if (script_func) {
		script_func(SCRIPT_END, this, &script_data, NULL);
	}
}

void Map::add_entity(Map_Entity *entity)
{
	entities.push_back(entity);
}

void Map::add_speeches(std::vector<std::string> &speeches)
{
	this->speeches.insert(this->speeches.begin(), speeches.begin(), speeches.end());
	if (speech == NULL) {
		speech = new Speech(speeches[0]);
		speech->start();
	}
}

void Map::change_map(std::string map_name, Point<int> position, Direction direction)
{
	new_map_name = map_name;
	new_map_position = position;
	new_map_direction = direction;
}

bool Map::is_solid(int layer, Point<int> position, Size<int> size)
{
	for (size_t i = 0; i < entities.size(); i++) {
		Point<int> p = entities[i]->get_position();
		if (p.x >= position.x && p.x < position.x+size.w && p.y >= position.y && p.y < position.y+size.h) {
			return true;
		}
	}
	for (int y = 0; y < size.h; y++) {
		for (int x = 0; x < size.w; x++) {
			Point<int> p(position.x+x, position.y+y);
			if (tilemap->is_solid(layer, p)) {
				return true;
			}
		}
	}

	return false;
}

void Map::check_triggers(Map_Entity *entity)
{
	if (script_func) {
		script_func(SCRIPT_TRIGGERS, this, &script_data, entity);
	}
}

void Map::get_new_map_details(std::string &map_name, Point<int> &position, Direction &direction)
{
	map_name = new_map_name;
	position = new_map_position;
	direction = new_map_direction;
}

Map_Entity *Map::get_entity(int id)
{
	for (size_t i = 0; i < entities.size(); i++) {
		if (entities[i]->get_id() == id) {
			return entities[i];
		}
	}
	return NULL;
}

void Map::handle_event(TGUI_Event *event)
{
	if (speech) {
		if (speech->handle_event(event) == false) {
			delete speech;
			speeches.erase(speeches.begin());
			if (speeches.size() > 0) {
				speech = new Speech(speeches[0]);
				speech->start();
			}
			else {
				speech = NULL;
			}
		}
	}
	else {
		for (size_t i = 0; i < entities.size(); i++) {
			entities[i]->handle_event(event);
		}
	}
}

void Map::update_camera()
{
	Map_Entity *player = get_entity(0);
	if (player) {
		Point<int> p = player->get_draw_position();
		Size<int> sz = player->get_size();
		offset = p - Point<int>(screen_w, screen_h) / 2 + sz / 2;
		int max_x = (tilemap->get_width()*tilemap->get_tile_size()-screen_w);
		int max_y = (tilemap->get_height()*tilemap->get_tile_size()-screen_h);
		if (offset.x < 0) {
			offset.x = 0;
		}
		else if (offset.x > max_x) {
			offset.x = max_x;
		}
		if (offset.y < 0) {
			offset.y = 0;
		}
		else if (offset.y > max_y) {
			offset.y = max_y;
		}
		offset = -offset;
		// Correct for small levels
		if (tilemap->get_width()*tilemap->get_tile_size() < screen_w) {
			offset.x = (screen_w - (tilemap->get_width() * tilemap->get_tile_size())) / 2;
		}
		if (tilemap->get_height()*tilemap->get_tile_size() < screen_h) {
			offset.y = (screen_h - (tilemap->get_height() * tilemap->get_tile_size())) / 2;
		}
	}
}

bool Map::update()
{
	if (script_func) {
		script_func(SCRIPT_UPDATE, this, &script_data, NULL);
	}

	if (speech == NULL) {
		for (size_t i = 0; i < entities.size(); i++) {
			entities[i]->update(this);
		}
	}

	update_camera();

	if (new_map_name != "") {
		return false;
	}

	return true;
}

void Map::draw()
{
	// FIXME: fix layers
	tilemap->draw(0, offset); // FIXME: pos

	for (size_t i = 0; i < entities.size(); i++) {
		Map_Entity *e = entities[i];
		e->draw(e->get_draw_position() + offset);
	}
	tilemap->draw(1, offset); // FIXME: pos

	if (speech) {
		speech->draw();
	}
}
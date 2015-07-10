#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/log.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/module.h"

using namespace Nooskewl_Engine;

Map::Map(std::string map_name) :
	offset(0, 0),
	speech(NULL),
	map_name(map_name),
	new_map_name(""),
	ml(NULL)
{
	tilemap = new Tilemap(8, map_name);

	ml = m.get_map_logic(this);
}

Map::~Map()
{
	if (ml) {
		ml->end(this);
	}

	delete tilemap;

	for (size_t i = 0; i < entities.size(); i++) {
		if (entities[i]->get_id() != 0) {
			delete entities[i];
		}
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
	if (ml) {
		ml->trigger(this, entity);
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

std::string Map::get_map_name()
{
	return map_name;
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
		offset = p - Point<int>(noo.screen_w, noo.screen_h) / 2 + sz / 2;
		int max_x = (tilemap->get_width()*tilemap->get_tile_size()-noo.screen_w);
		int max_y = (tilemap->get_height()*tilemap->get_tile_size()-noo.screen_h);
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
		if (tilemap->get_width()*tilemap->get_tile_size() < noo.screen_w) {
			offset.x = (noo.screen_w - (tilemap->get_width() * tilemap->get_tile_size())) / 2;
		}
		if (tilemap->get_height()*tilemap->get_tile_size() < noo.screen_h) {
			offset.y = (noo.screen_h - (tilemap->get_height() * tilemap->get_tile_size())) / 2;
		}
	}
}

bool Map::update()
{
	if (ml) {
		ml->update(this);
	}

	if (speech == NULL) {
		std::vector<Map_Entity *>::iterator it;
		for (it = entities.begin(); it != entities.end();) {
			Map_Entity *e = *it;
			if (e->update(this) == false) {
				delete e;
				it = entities.erase(it);
			}
			else {
				it++;
			}
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
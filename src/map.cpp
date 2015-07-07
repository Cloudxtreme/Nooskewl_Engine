#include "log.h"
#include "map.h"
#include "video.h"

Map::Map(std::string map_name) :
	offset(0, 0),
	speech(NULL),
	new_map_name("")
{
	tilemap = new Tilemap(8, map_name);
	init_entities(map_name);
}

Map::~Map()
{
	delete tilemap;

	for (size_t i = 0; i < entities.size(); i++) {
		if (entities[i]->get_id() != 0) {
			delete entities[i];
		}
	}

	for (size_t i = 0; i < floor_triggers.size(); i++) {
		delete floor_triggers[i];
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

bool Map::is_solid(int layer, Point<int> position)
{
	for (size_t i = 0; i < entities.size(); i++) {
		if (position == entities[i]->get_position()) {
			return true;
		}
	}
	return tilemap->is_solid(layer, position);
}

void Map::check_triggers(Map_Entity *entity)
{
	Point<int> pos = entity->get_position();
	for (size_t i = 0; i < floor_triggers.size(); i++) {
		Floor_Trigger *t = floor_triggers[i];
		if (pos.x >= t->topleft.x && pos.x < t->topleft.x+t->size.w && pos.y >= t->topleft.y && pos.y < t->topleft.y+t->size.h) {
			t->function(this, entity, pos-t->topleft);
		}
	}
}

void Map::get_new_map_details(std::string &map_name, Point<int> &position, Direction &direction)
{
	map_name = new_map_name;
	position = new_map_position;
	direction = new_map_direction;
}

void Map::handle_event(SDL_Event *event)
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

bool Map::update()
{
	if (speech == NULL) {
		for (size_t i = 0; i < entities.size(); i++) {
			entities[i]->update(this);
			if (entities[i]->get_id() == 0) {
				Point<int> p = entities[i]->get_draw_position();
				Size<int> sz = entities[i]->get_size();
				offset = p - Point<int>(screen_w/2, screen_h/2) + sz / 2;
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
	}

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

void Map::init_entities(std::string map_name)
{
	if (map_name == "test.map") {
		floor_triggers.push_back(new Floor_Trigger(Point<int>(7, 1), Size<int>(2, 1), ft_test));
	}
	else if (map_name == "test2.map") {
		floor_triggers.push_back(new Floor_Trigger(Point<int>(7, 18), Size<int>(2, 1), ft_test2));
	}
}
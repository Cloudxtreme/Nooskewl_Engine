#include "log.h"
#include "map.h"
#include "video.h"

Map::Map() :
	tilemap(NULL),
	offset(0.0f, 0.0f)
{
}

Map::~Map()
{
	delete tilemap;

	for (size_t i = 0; i < entities.size(); i++) {
		delete entities[i];
	}

	for (size_t i = 0; i < floor_triggers.size(); i++) {
		delete floor_triggers[i];
	}
}

bool Map::load(std::string map_name)
{
	tilemap = new Tilemap(8);
	if (tilemap->load("sheets", map_name) == false) {
		delete tilemap;
		errormsg("Error loading tilemap\n");
		return false;
	}

	init_entities(map_name);

	return true;
}

void Map::add_entity(Map_Entity *entity)
{
	entities.push_back(entity);
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
			t->function(this, entity);
		}
	}
}

void Map::handle_event(SDL_Event *event)
{
	for (size_t i = 0; i < entities.size(); i++) {
		entities[i]->handle_event(event);
	}
}

bool Map::update()
{
	for (size_t i = 0; i < entities.size(); i++) {
		entities[i]->update();
		if (entities[i]->get_id() == 0) {
			Point<int> p = entities[i]->get_draw_position();
			Size<int> sz = entities[i]->get_size();
			offset = p - Point<float>(screen_w/2, screen_h/2) + sz / 2;
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
	return true;
}

void Map::draw()
{
	// FIXME: fix layers
	tilemap->draw(0, offset.x, offset.y); // FIXME: pos

	for (size_t i = 0; i < entities.size(); i++) {
		Map_Entity *e = entities[i];
		e->draw(e->get_draw_position() + offset);
	}
	tilemap->draw(1, offset.x, offset.y); // FIXME: pos
}

void Map::init_entities(std::string map_name)
{
	if (map_name == "test.map") {
		floor_triggers.push_back(new Floor_Trigger(Point<int>(1, 4), Size<int>(1, 1), ft_test));
	}
}
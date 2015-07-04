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
}

bool Map::load(std::string name)
{
	tilemap = new Tilemap(8);
	if (tilemap->load("sheets", name) == false) {
		delete tilemap;
		errormsg("Error loading tilemap\n");
		return false;
	}
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
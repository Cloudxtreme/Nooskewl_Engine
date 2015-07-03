#include "log.h"
#include "map.h"

Map::Map() :
	tilemap(NULL)
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
	}
	return true;
}

void Map::draw()
{
	// FIXME: fix layers
	tilemap->draw(0, 0, 0); // FIXME: pos

	for (size_t i = 0; i < entities.size(); i++) {
		Map_Entity *e = entities[i];
		e->draw();
	}
	tilemap->draw(1, 0, 0); // FIXME: pos
}
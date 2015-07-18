#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map.h"

using namespace Nooskewl_Engine;

const float Map::PAN_BACK_SPEED = 2.0f;

void Map::new_game_started()
{
	Map_Entity::new_game_started();
}

Map::Map(std::string map_name) :
	offset(0.0f, 0.0f),
	panning(false),
	speech(0),
	map_name(map_name),
	new_map_name("")
{
	tilemap = new Tilemap(map_name);

	ml = m.get_map_logic(map_name);
}

Map::~Map()
{
	delete ml;

	delete tilemap;

	delete a_star;

	for (size_t i = 0; i < entities.size(); i++) {
		if (entities[i]->get_id() != 0) {
			delete entities[i];
		}
	}
}

void Map::start()
{
	a_star = new A_Star(this);

	if (ml) {
		ml->start();
	}
}

void Map::end()
{
	if (ml) {
		ml->end();
	}
}

void Map::add_entity(Map_Entity *entity)
{
	entities.push_back(entity);
}

void Map::add_speech(std::string text)
{
	speeches.push_back(text);
	if (speech == 0) {
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

void Map::set_panning(bool panning)
{
	this->panning = panning;
	if (panning == false) {
		if (pan.length() < 5) {
			pan = Point<int>(0, 0);
		}
		else {
			pan_angle = -pan.angle();
		}
	}
}

void Map::set_pan(Point<float> pan)
{
	this->pan = pan;
}

bool Map::is_solid(int layer, Point<int> position, Size<int> size, bool check_entities, bool check_tiles)
{
	if (check_entities) {
		for (size_t i = 0; i < entities.size(); i++) {
			Map_Entity *e = entities[i];
			if (e->is_solid()) {
				Point<int> p = e->get_position();
				if (p.x >= position.x && p.x < position.x+size.w && p.y >= position.y && p.y < position.y+size.h) {
					return true;
				}
			}
		}
	}

	if (check_tiles) {
		for (int y = 0; y < size.h; y++) {
			for (int x = 0; x < size.w; x++) {
				Point<int> p(position.x+x, position.y+y);
				if (tilemap->is_solid(layer, p)) {
					return true;
				}
			}
		}
	}

	return false;
}

void Map::check_triggers(Map_Entity *entity)
{
	if (ml) {
		ml->trigger(entity);
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
	return 0;
}

std::string Map::get_map_name()
{
	return map_name;
}

Tilemap *Map::get_tilemap()
{
	return tilemap;
}

Point<float> Map::get_offset()
{
	return offset;
}

std::list<A_Star::Node *> Map::find_path(Point<int> start, Point<int> goal)
{
	return a_star->find_path(start, goal);
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
				speech = 0;
			}
		}
	}
	else {
		for (size_t i = 0; i < entities.size(); i++) {
			Map_Entity *e = entities[i];
			bool is_player = e->get_id() == 0;
			bool b1_down = is_player && e->get_brain()->b1;
			e->handle_event(event);
			if (ml && is_player && !e->is_sitting() && b1_down == false && e->get_brain()->b1) {
				// activate pressed
				activate(e);
			}
		}
	}
}

void Map::update_camera()
{
	Map_Entity *player = get_entity(0);
	if (player) {
		Point<float> p = player->get_draw_position();
		Size<int> sz = player->get_size();
		offset = p - noo.screen_size / 2 + sz / 2;
		offset += pan;
		Size<int> tilemap_size = tilemap->get_size();
		int max_x = (tilemap_size.w * noo.tile_size)-noo.screen_size.w;
		int max_y = (tilemap_size.h * noo.tile_size)-noo.screen_size.h;
		if (offset.x < 0.0f) {
			offset.x = 0.0f;
		}
		else if (offset.x >= max_x) {
			offset.x = (float)max_x;
		}
		if (offset.y < 0.0f) {
			offset.y = 0.0f;
		}
		else if (offset.y >= max_y) {
			offset.y = (float)max_y;
		}
		offset = -offset;
		// Correct for small levels
		if (tilemap_size.w*noo.tile_size < noo.screen_size.w) {
			offset.x = float(noo.screen_size.w - (tilemap_size.w * noo.tile_size)) / 2;
		}
		if (tilemap_size.h*noo.tile_size < noo.screen_size.h) {
			offset.y = float(noo.screen_size.h - (tilemap_size.h * noo.tile_size)) / 2;
		}
	}
}

bool Map::update()
{
	if (ml) {
		ml->update();
	}

	std::vector<Map_Entity *>::iterator it;
	for (it = entities.begin(); it != entities.end();) {
		Map_Entity *e = *it;
		if (e->update(speech != 0) == false) {
			delete e;
			it = entities.erase(it);
		}
		else {
			it++;
		}
	}

	update_camera();

	// Reset pan gradually when the user lets go of mouse
	if (panning == false && (pan.x != 0.0f || pan.y != 0.0f)) {
		if (pan.x < 0.0f) {
			pan.x += cos(pan_angle) * PAN_BACK_SPEED;
			if (pan.x > 0.0f) {
				pan.x = 0.0f;
			}
		}
		else if (pan.x > 0.0f) {
			pan.x += cos(pan_angle) * PAN_BACK_SPEED;
			if (pan.x < 0.0f) {
				pan.x = 0.0f;
			}
		}
		if (pan.y < 0.0f) {
			pan.y += sin(pan_angle) * PAN_BACK_SPEED;
			if (pan.y > 0.0f) {
				pan.y = 0.0f;
			}
		}
		else if (pan.y > 0.0f) {
			pan.y += sin(pan_angle) * PAN_BACK_SPEED;
			if (pan.y < 0.0f) {
				pan.y = 0.0f;
			}
		}
	}

	if (new_map_name != "") {
		return false;
	}

	return true;
}

void Map::draw(bool use_depth_buffer)
{
	int nlayers = tilemap->get_num_layers();
	int layer;

	for (layer = 0; layer < nlayers/2; layer++) {
		tilemap->draw(layer, offset);
	}

	tilemap->draw_shadows(layer, offset);

	if (use_depth_buffer) {
		noo.enable_depth_buffer(true);
		noo.clear_depth_buffer(1.0f);
	}

	for (size_t i = 0; i < entities.size(); i++) {
		Map_Entity *e = entities[i];
		e->draw(e->get_draw_position() + offset, use_depth_buffer);
	}

	tilemap->draw(layer, offset, use_depth_buffer);

	layer++;

	if (use_depth_buffer) {
		noo.enable_depth_buffer(false);
	}

	for (; layer < nlayers; layer++) {
		tilemap->draw(layer, offset);
	}

	if (speech) {
		speech->draw();
	}
}

bool Map::activate(Map_Entity *entity)
{
	entity->stop();
	Direction dir = entity->get_direction();
	Point<int> pos = entity->get_position() * noo.tile_size + entity->get_offset() * (float)noo.tile_size;
	Size<int> size(noo.tile_size, noo.tile_size);
	switch (dir) {
		case N:
			pos.y -= noo.tile_size*2;
			size.h += noo.tile_size;
			break;
		case E:
			pos.x += noo.tile_size;
			size.w += noo.tile_size;
			break;
		case S:
			pos.y += noo.tile_size;
			size.h += noo.tile_size;
			break;
		case W:
			pos.x -= noo.tile_size*2;
			size.w += noo.tile_size;
			break;
	}
	for (size_t j = 0; j < entities.size(); j++) {
		Map_Entity *e = entities[j];
		if (e->get_id() == 0) {
			continue;
		}
		if (e->pixels_collide(pos, size)) {
			ml->activate(e, e);
			return true;
		}
	}

	return false;
}
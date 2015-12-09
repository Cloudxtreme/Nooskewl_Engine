#include "Nooskewl_Engine/a_star.h"
#include "Nooskewl_Engine/brain.h"
#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"
#include "Nooskewl_Engine/map_logic.h"
#include "Nooskewl_Engine/speech.h"
#include "Nooskewl_Engine/sprite.h"
#include "Nooskewl_Engine/tilemap.h"

using namespace Nooskewl_Engine;

struct Sit_Sleep_Data {
	Map_Entity *entity;
	int sit_sleep_directions;
	Direction direction;
	bool is_chair;
};

struct Map_Entity_Distance {
	Map_Entity *entity;
	float distance;
};

static bool sort_by_distance(const Map_Entity_Distance &a, const Map_Entity_Distance &b)
{
	return a.distance < b.distance;
}

const float Map::PAN_BACK_SPEED = 2.0f;

void Map::new_game_started()
{
	Map_Entity::new_game_started();

	noo.clear_milestones();
}

void Map::sit_sleep_callback(void *data)
{
	Sit_Sleep_Data *sit_sleep_data = (Sit_Sleep_Data *)data;

	sit_sleep_data->entity->set_direction(sit_sleep_data->direction);

	if (sit_sleep_data->is_chair) {
		sit_sleep_data->entity->set_sitting(true);
	}
	else {
		sit_sleep_data->entity->set_sleeping(true);
	}

	sit_sleep_data->entity->set_sit_sleep_directions(sit_sleep_data->sit_sleep_directions);

	delete sit_sleep_data;
}

Map::Map(std::string map_name, bool been_here_before, int last_visited_time) :
	offset(0.0f, 0.0f),
	panning(false),
	speech(0),
	map_name(map_name),
	new_map_name(""),
	a_star(0),
	been_here_before(been_here_before)
{
	tilemap = new Tilemap(map_name);

	ml = m.dll_get_map_logic(map_name, last_visited_time);
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

void Map::start_audio()
{
	if (ml) {
		ml->start_audio();
	}
}

void Map::start()
{
	start_audio();

	a_star = new A_Star(this);

	if (ml) {
		ml->start(been_here_before);
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

void Map::add_speech(std::string text, Callback callback, void *callback_data)
{
	speeches.push_back(new Map_Speech(text, callback, callback_data));
	if (speech == 0) {
		speech = new Speech(speeches[0]->text, speeches[0]->callback, callback_data);
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
		if (pan.length() <= PAN_BACK_SPEED) {
			pan = Point<int>(0, 0);
		}
		else {
			pan_angle = pan.angle();
		}
	}
}

void Map::set_pan(Point<float> pan)
{
	this->pan = pan;
}

std::vector<Map_Entity *> Map::get_colliding_entities(int layer, Point<int> position, Size<int> size)
{
	std::vector<Map_Entity *> result;

	for (size_t i = 0; i < entities.size(); i++) {
		Map_Entity *e = entities[i];
		Point<int> pos1 = e->get_position() * noo.tile_size;
		Size<int> size1 = e->get_size();
		pos1.x += noo.tile_size / 2 - size1.w / 2;
		pos1.y -= (size1.h - noo.tile_size);
		Point<int> pos2 = position * noo.tile_size;
		Size<int> size2 = size * noo.tile_size;
		if (!(pos1.x >= pos2.x+size2.w || pos1.x+size1.w <= pos2.x || pos1.y >= pos2.y+size2.h || pos1.y+size1.h <= pos2.y)) {
			result.push_back(e);
		}
	}

	return result;
}

bool Map::is_solid(int layer, Map_Entity *collide_with, Point<int> position, Size<int> size, bool check_entities, bool check_tiles)
{
	if (check_entities) {
		std::vector<Map_Entity *> colliding_entities = get_colliding_entities(layer, position, size);
		for (size_t i = 0; i < colliding_entities.size(); i++) {
			Map_Entity *e = colliding_entities[i];
			if (collide_with) {
				collisions.push_back(std::pair<Map_Entity *, Map_Entity *>(e, collide_with));
			}
			if (e->is_solid()) {
				return true;
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

Map_Entity *Map::find_entity(std::string name)
{
	for (size_t i = 0; i < entities.size(); i++) {
		if (entities[i]->get_name() == name) {
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

Point<float> Map::get_pan()
{
	return pan;
}

std::list<A_Star::Node *> Map::find_path(Point<int> start, Point<int> goal, bool check_solids)
{
	return a_star->find_path(start, goal, check_solids);
}

bool Map::is_speech_active()
{
	return speech != 0;
}

Map_Logic *Map::get_map_logic()
{
	return ml;
}

void Map::handle_event(TGUI_Event *event)
{
	if (speech) {
		if (speech->handle_event(event) == false) {
			delete speech;
			Map_Speech *ms = speeches[0];
			delete ms;
			speeches.erase(speeches.begin());
			if (speeches.size() > 0) {
				speech = new Speech(speeches[0]->text, speeches[0]->callback, speeches[0]->callback_data);
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
			if (ml && is_player && !e->is_sitting() && !e->is_sleeping() && !e->is_following_path() && b1_down == false && e->get_brain()->b1) {
				// activate pressed
				activate(e);
			}
		}

		if (
			(event->type == TGUI_KEY_DOWN && event->keyboard.code == noo.key_b3) ||
			(event->type == TGUI_JOY_DOWN && event->joystick.button == noo.joy_b3)
		) {
			choose_action();
		}
	}
}

void Map::update_camera()
{
	for (size_t i = 0; i < collisions.size(); i++) {
		std::pair<Map_Entity *, Map_Entity *> &p = collisions[i];
		Brain *brain = p.first->get_brain();
		if (brain) {
			brain->collide(p.second);
		}
	}
	collisions.clear();

	for (size_t i = 0; i < entities_to_destroy.size(); i++) {
		Map_Entity *entity_to_destroy = entities_to_destroy[i];
		std::vector<Map_Entity *>::iterator it = std::find(entities.begin(), entities.end(), entity_to_destroy);
		if (it != entities.end()) {
			Map_Entity *entity = *it;
			entities.erase(it);
			delete entity;
		}
	}
	entities_to_destroy.clear();

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
		Brain *b = e->get_brain();
		if (b) {
			b->update();
		}
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
		if (pan.x != 0.0f) {
			pan.x -= cos(pan_angle) * PAN_BACK_SPEED;
			if (fabs(pan.x) <= PAN_BACK_SPEED) {
				pan.x = 0.0f;
			}
		}
		if (pan.y != 0.0f) {
			pan.y -= sin(pan_angle) * PAN_BACK_SPEED;
			if (fabs(pan.y) <= PAN_BACK_SPEED) {
				pan.y = 0.0f;
			}
		}
		pan_angle = pan.angle();
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

	for (size_t i = 0; i < entities.size(); i++) {
		Map_Entity *e = entities[i];
		if (e->is_low()) {
			e->draw(e->get_draw_position() + offset, use_depth_buffer);
		}
	}

	noo.enable_depth_buffer(use_depth_buffer);

	for (size_t i = 0; i < entities.size(); i++) {
		Map_Entity *e = entities[i];
		if (!e->is_high() && !e->is_low()) {
			e->draw(e->get_draw_position() + offset, use_depth_buffer);
		}
	}

	tilemap->draw(layer, offset, use_depth_buffer);

	layer++;

	noo.enable_depth_buffer(false);

	for (; layer < nlayers; layer++) {
		tilemap->draw(layer, offset);
	}

	for (size_t i = 0; i < entities.size(); i++) {
		Map_Entity *e = entities[i];
		if (e->is_high()) {
			e->draw(e->get_draw_position() + offset, use_depth_buffer);
		}
		else if (e->is_sitting() && e->get_direction() == N) {
			e->draw(e->get_draw_position() + offset, use_depth_buffer, true);
		}
	}

	if (speech) {
		speech->draw();
	}
}

bool Map::activate(Map_Entity *entity)
{
	bool moving = entity->is_moving();
	bool following_path = entity->is_following_path();
	if (!following_path) {
		entity->stop();
	}
	if (moving) {
		entity->set_activate_next_tile(true);
		return false;
	}

	Direction dir = entity->get_direction();
	Point<int> pos = entity->get_position() * noo.tile_size + entity->get_offset() * (float)noo.tile_size;
	Size<int> size = entity->get_size();
	pos.x += noo.tile_size / 2 - size.w / 2;
	pos.y -= (size.h - noo.tile_size);
	switch (dir) {
		case N:
			pos.y -= noo.tile_size;
			size.h += noo.tile_size;
			break;
		case E:
			size.w += noo.tile_size;
			break;
		case S:
			size.h += noo.tile_size;
			break;
		case W:
			pos.x -= noo.tile_size;
			size.w += noo.tile_size;
			break;
	}

	std::vector<Tilemap::Group *> groups = tilemap->get_groups(-1);

	for (size_t i = 0; i < groups.size(); i++) {
		Tilemap::Group *g = groups[i];

		int x1_1 = pos.x;
		int y1_1 = pos.y;
		int x2_1 = pos.x + size.w;
		int y2_1 = pos.y + size.h;

		int x1_2 = g->position.x;
		int y1_2 = g->position.y;
		int x2_2 = g->position.x + g->size.w;
		int y2_2 = g->position.y + g->size.h;
		x1_2 *= noo.tile_size;
		y1_2 *= noo.tile_size;
		x2_2 *= noo.tile_size;
		y2_2 *= noo.tile_size;

		if (x1_1 >= x2_2 || x2_1 <= x1_2 || y1_1 >= y2_2 || y2_1 <= y1_2) {
			continue;
		}

		Point<int> collide_pos = Point<int>(MAX(x1_1, x1_2), MAX(y1_1, y1_2)) / noo.tile_size;

		Point<int> entity_pos = entity->get_position();

		if (g->position == entity_pos) {
			if (
				(g->type & Tilemap::Group::GROUP_CHAIR_NORTH) ||
				(g->type & Tilemap::Group::GROUP_CHAIR_EAST) ||
				(g->type & Tilemap::Group::GROUP_CHAIR_SOUTH) ||
				(g->type & Tilemap::Group::GROUP_CHAIR_WEST)
			) {
				entity->set_sitting(true);
				entity->set_sit_sleep_directions(g->type);

				return true;
			}
		}
		else {
			if (is_solid(-1, 0, g->position, Size<int>(1, 1), true, false)) {
				continue;
			}

			Direction direction = entity->get_direction();

			std::list<A_Star::Node *> path;

			bool is_chair = true;

			if (g->type == Tilemap::Group::GROUP_BED_NORTH && direction == E) {
				is_chair = false;
				collide_pos = g->position + Point<int>(-1, 1);
				direction = N;
			}
			else if (g->type == Tilemap::Group::GROUP_BED_NORTH && direction == W) {
				is_chair = false;
				collide_pos = g->position + Point<int>(2, 1);
				direction = N;
			}
			else if (g->type == Tilemap::Group::GROUP_BED_EAST && direction == N) {
				is_chair = false;
				collide_pos = g->position + Point<int>(0, 2);
				direction = E;
			}
			else if (g->type == Tilemap::Group::GROUP_BED_EAST && direction == S) {
				is_chair = false;
				collide_pos = g->position + Point<int>(0, -1);
				direction = E;
			}
			else if (g->type == Tilemap::Group::GROUP_BED_SOUTH && direction == E) {
				is_chair = false;
				collide_pos = g->position + Point<int>(-1, 0);
				direction = S;
			}
			else if (g->type == Tilemap::Group::GROUP_BED_SOUTH && direction == W) {
				is_chair = false;
				collide_pos = g->position + Point<int>(2, 0);
				direction = S;
			}
			else if (g->type == Tilemap::Group::GROUP_BED_WEST && direction == N) {
				is_chair = false;
				collide_pos = g->position + Point<int>(1, 2);
				direction = W;
			}
			else if (g->type == Tilemap::Group::GROUP_BED_WEST && direction == S) {
				is_chair = false;
				collide_pos = g->position + Point<int>(1, -1);
				direction = W;
			}
			else if (direction == N) {
				if (g->type & Tilemap::Group::GROUP_CHAIR_SOUTH) {
					path = find_path(entity_pos, collide_pos, false);
					direction = S;
				}
			}
			else if (direction == E) {
				if (g->type & Tilemap::Group::GROUP_CHAIR_WEST) {
					path = find_path(entity_pos, collide_pos, false);
					direction = W;
				}
			}
			else if (direction == S) {
				if (g->type & Tilemap::Group::GROUP_CHAIR_NORTH) {
					path = find_path(entity_pos, collide_pos, false);
					direction = N;
				}
			}
			else {
				if (g->type & Tilemap::Group::GROUP_CHAIR_EAST) {
					path = find_path(entity_pos, collide_pos, false);
					direction = E;
				}
			}

			if (is_chair == false && entity_pos != collide_pos) {
				path = find_path(entity_pos, collide_pos, false);
			}

			if (is_chair == false && entity_pos == collide_pos) {
				entity->set_direction(direction);
				entity->set_sit_sleep_directions(g->type);
				entity->set_sleeping(true);
			}
			else if (path.size() > 0) {
				entity->set_pre_sit_sleep_direction(entity->get_direction());
				entity->set_solid(false);

				Sit_Sleep_Data *sit_sleep_data = new Sit_Sleep_Data;
				sit_sleep_data->entity = entity;
				sit_sleep_data->sit_sleep_directions = g->type;
				sit_sleep_data->direction = direction;
				sit_sleep_data->is_chair = is_chair;
				entity->set_path(path, sit_sleep_callback, sit_sleep_data);

				return true;
			}
		}
	}

	switch (dir) {
		case N:
			pos.y -= noo.tile_size;
			size.h += noo.tile_size;
			break;
		case E:
			size.w += noo.tile_size;
			break;
		case S:
			size.h += noo.tile_size;
			break;
		case W:
			pos.x -= noo.tile_size;
			size.w += noo.tile_size;
			break;
	}

	std::vector<Map_Entity_Distance> v;

	for (size_t j = 0; j < entities.size(); j++) {
		Map_Entity_Distance m;
		m.entity = entities[j];
		Point<int> p = entity->get_position()-m.entity->get_position();
		Size<int> s(p.x, p.y);
		m.distance = s.length();
		v.push_back(m);
	}

	std::sort(v.begin(), v.end(), sort_by_distance);

	for (size_t j = 0; j < v.size(); j++) {
		Map_Entity *e = v[j].entity;
		if (e->get_id() == 0) {
			continue;
		}
		if (e->pixels_collide(pos, size)) {
			ml->activate(entity, e);
			Brain *b = e->get_brain();
			if (b) {
				if (b->activate(entity)) {
					return true;
				}
			}
		}
	}

	return false;
}

void Map::choose_action()
{
	Map_Entity *entity = noo.player;

	Direction dir = entity->get_direction();
	Point<int> pos = entity->get_position() * noo.tile_size + entity->get_offset() * (float)noo.tile_size;
	Size<int> size = entity->get_size();
	pos.x += noo.tile_size / 2 - size.w / 2;
	pos.y -= (size.h - noo.tile_size);
	switch (dir) {
		case N:
			pos.y -= noo.tile_size;
			size.h += noo.tile_size;
			break;
		case E:
			size.w += noo.tile_size;
			break;
		case S:
			size.h += noo.tile_size;
			break;
		case W:
			pos.x -= noo.tile_size;
			size.w += noo.tile_size;
			break;
	}

	switch (dir) {
		case N:
			pos.y -= noo.tile_size;
			size.h += noo.tile_size;
			break;
		case E:
			size.w += noo.tile_size;
			break;
		case S:
			size.h += noo.tile_size;
			break;
		case W:
			pos.x -= noo.tile_size;
			size.w += noo.tile_size;
			break;
	}

	std::vector<Map_Entity_Distance> v;

	for (size_t j = 0; j < entities.size(); j++) {
		Map_Entity_Distance m;
		m.entity = entities[j];
		Point<int> p = entity->get_position()-m.entity->get_position();
		Size<int> s(p.x, p.y);
		m.distance = s.length();
		v.push_back(m);
	}

	std::sort(v.begin(), v.end(), sort_by_distance);

	for (size_t j = 0; j < v.size(); j++) {
		Map_Entity *e = v[j].entity;
		if (e->get_id() == 0) {
			continue;
		}
		if (e->pixels_collide(pos, size)) {
			if (m.dll_choose_action(e)) {
				break;
			}
		}
	}
}

void Map::schedule_destroy(Map_Entity *entity)
{
	if (std::find(entities_to_destroy.begin(), entities_to_destroy.end(), entity) == entities_to_destroy.end()) {
		entities_to_destroy.push_back(entity);
	}
}

bool Map::save(std::string &out, bool save_player)
{
	out += string_printf("map_name=%s\n", map_name.c_str());

	out += string_printf("time=%d\n", noo.get_play_time());

	out += string_printf("num_entities=%d\n", entities.size());

	std::string entity_save;

	if (save_player) {
		if (noo.player->save(entity_save) == false) {
			return false;
		}

		out += entity_save;
	}

	for (size_t i = 0; i < entities.size(); i++) {
		Map_Entity *entity = entities[i];
		if (entity != noo.player) {
			entity_save = "";
			if (entity->save(entity_save) == false) {
				return false;
			}
			out += entity_save;
		}
	}

	return true;
}

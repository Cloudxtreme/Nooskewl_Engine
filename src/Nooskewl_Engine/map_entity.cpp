#include "Nooskewl_Engine/engine.h"
#include "Nooskewl_Engine/map.h"
#include "Nooskewl_Engine/map_entity.h"

using namespace Nooskewl_Engine;

static int current_id;

Map_Entity::Map_Entity(Brain *brain) :
	direction(S),
	sprite(NULL),
	brain(brain),
	position(0, 0),
	moving(false),
	speed(0.1f),
	offset(0.0f, 0.0f),
	bounce(1),
	solid(true),
	size(noo.tile_size, noo.tile_size),
	stop_next_tile(false)
{
	id = current_id++;
}

Map_Entity::~Map_Entity()
{
	delete brain;
	delete sprite;
}

void Map_Entity::load_sprite(std::string name)
{
	sprite = new Sprite(name + "/animations.xml", name);
	sprite->set_animation("stand_s");
}

void Map_Entity::set_position(Point<int> position)
{
	this->position = position;
}

void Map_Entity::set_offset(Point<float> offset)
{
	this->offset = offset;
}

void Map_Entity::set_bounce(int bounce)
{
	this->bounce = bounce;
}

void Map_Entity::set_direction(Direction direction)
{
	this->direction = direction;
	std::string animation_name;
	if (moving) {
		switch (direction) {
			case N:
				animation_name = "walk_n";
				break;
			case E:
				animation_name = "walk_e";
				break;
			case S:
				animation_name = "walk_s";
				break;
			default:
				animation_name = "walk_w";
				break;
		}
	}
	else {
		switch (direction) {
			case N:
				animation_name = "stand_n";
				break;
			case E:
				animation_name = "stand_e";
				break;
			case S:
				animation_name = "stand_s";
				break;
			default:
				animation_name = "stand_w";
				break;
		}
	}

	sprite->set_animation(animation_name);
}

void Map_Entity::set_solid(bool solid)
{
	this->solid = solid;
}

int Map_Entity::get_id()
{
	return id;
}

Brain *Map_Entity::get_brain()
{
	return brain;
}

Direction Map_Entity::get_direction()
{
	return direction;
}

Point<int> Map_Entity::get_position()
{
	return position;
}

Point<float> Map_Entity::get_offset()
{
	return offset;
}

Size<int> Map_Entity::get_size()
{
	return size;
}

Point<int> Map_Entity::get_draw_position()
{
		int h = sprite->get_current_image()->h;
		return Point<int>(position.x*noo.tile_size+int(offset.x*(float)noo.tile_size), (position.y+1)*noo.tile_size+int(offset.y*(float)noo.tile_size)-h);
}

bool Map_Entity::is_solid()
{
	return solid;
}

bool Map_Entity::pixels_collide(Point<int> position, Size<int> size)
{
	Point<int> pos = this->position * noo.tile_size + this->offset * (float)noo.tile_size;
	if (pos.x > position.x+size.w || pos.x+this->size.w < position.x || pos.y > position.y+size.h || pos.y+this->size.h < position.y) {
		return false;
	}
	return true;
}

bool Map_Entity::tiles_collide(Point<int> position, Size<int> size, Point<int> &collide_pos)
{
	if (this->position.x >= position.x && this->position.x < position.x+size.w && this->position.y >= position.y && this->position.y < position.y+size.h) {
		collide_pos = Point<int>(this->position.x-position.x, this->position.y-position.y);
		return true;
	}
	return false;
}

void Map_Entity::stop()
{
	if (moving) {
		stop_next_tile = true;
	}
	else {
		stop_now();
	}
}

bool Map_Entity::maybe_move(Map *map)
{
	if (brain) {
		if (brain->l) {
			if (map->is_solid(-1, position + Point<int>(-1, 0), Size<int>(1, 1)) == false) {
				moving = true;
				direction = W;
				offset = Point<float>(1, 0);
				position += Point<int>(-1, 0);
				sprite->set_animation("walk_w");
				sprite->reset();
				sprite->start();
				return true;
			}
			else {
				sprite->set_animation("stand_w");
			}
			direction = W;
		}
		else if (brain->r) {
			if (map->is_solid(-1, position + Point<int>(1, 0), Size<int>(1, 1)) == false) {
				moving = true;
				direction = E;
				offset = Point<float>(-1, 0);
				position += Point<int>(1, 0);
				sprite->set_animation("walk_e");
				sprite->reset();
				sprite->start();
				return true;
			}
			else {
				sprite->set_animation("stand_e");
			}
			direction = E;
		}
		else if (brain->u) {
			if (map->is_solid(-1, position + Point<int>(0, -1), Size<int>(1, 1)) == false) {
				moving = true;
				direction = N;
				offset = Point<float>(0, 1);
				position += Point<int>(0, -1);
				sprite->set_animation("walk_n");
				sprite->reset();
				sprite->start();
				return true;
			}
			else {
				sprite->set_animation("stand_n");
			}
			direction = N;
		}
		else if (brain->d) {
			if (map->is_solid(-1, position + Point<int>(0, 1), Size<int>(1, 1)) == false) {
				moving = true;
				direction = S;
				offset = Point<float>(0, -1);
				position += Point<int>(0, 1);
				sprite->set_animation("walk_s");
				sprite->reset();
				sprite->start();
				return true;
			}
			else {
				sprite->set_animation("stand_s");
			}
			direction = S;
		}
	}
	return false;
}

void Map_Entity::handle_event(TGUI_Event *event)
{
	if (brain) {
		brain->handle_event(event);
	}
}

bool Map_Entity::update(Map *map, bool can_move)
{
	if (moving == false) {
		maybe_move(map);
	}

	if (moving) {
		if (offset.x < 0) {
			offset.x += speed;
			if (offset.x >= 0) {
				offset.x = 0;
				map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
				}
				else if (maybe_move(map) == false) {
					moving = false;
					sprite->stop();
					sprite->set_animation("stand_e");
				}
			}
		}
		else if (offset.x > 0) {
			offset.x -= speed;
			if (offset.x <= 0) {
				offset.x = 0;
				map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
				}
				else if (maybe_move(map) == false) {
					moving = false;
					sprite->stop();
					sprite->set_animation("stand_w");
				}
			}
		}
		if (offset.y < 0) {
			offset.y += speed;
			if (offset.y >= 0) {
				offset.y = 0;
				map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
				}
				else if (maybe_move(map) == false) {
					moving = false;
					sprite->stop();
					sprite->set_animation("stand_s");
				}
			}
		}
		else if (offset.y > 0) {
			offset.y -= speed;
			if (offset.y <= 0) {
				offset.y = 0;
				map->check_triggers(this);
				if (stop_next_tile) {
					stop_now();
				}
				else if (maybe_move(map) == false) {
					moving = false;
					sprite->stop();
					sprite->set_animation("stand_n");
				}
			}
		}
	}

	sprite->update();

	return true;
}

void Map_Entity::draw(Point<int> draw_pos)
{
	int add = moving ? -((int)((SDL_GetTicks() / 100) % 2) * bounce) : 0;
	sprite->get_current_image()->draw_single(Point<int>(draw_pos.x, draw_pos.y+add));
}

void Map_Entity::stop_now()
{
	printf("stop_now\n");
	stop_next_tile = false;
	moving = false;
	// FIXME: add all buttons here
	brain->l = brain->r = brain->u = brain->d = brain->b1 = false;
	set_direction(direction);
	sprite->stop();
	sprite->reset();
	if (brain) {
		brain->reset_input();
	}
}
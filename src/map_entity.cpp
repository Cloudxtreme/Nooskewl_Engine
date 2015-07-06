#include "map.h"
#include "map_entity.h"

static int current_id;

Map_Entity::Map_Entity(Brain *brain) :
	direction(S),
	sprite(NULL),
	brain(brain),
	position(0, 0),
	moving(false),
	speed(0.1f),
	offset(0.0f, 0.0f),
	bounce(1)
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

int Map_Entity::get_id()
{
	return id;
}

Direction Map_Entity::get_direction()
{
	return direction;
}

Point<int> Map_Entity::get_position()
{
	return position;
}

Size<int> Map_Entity::get_size()
{
	// FIXME:
	return Size<int>(8, 16);
}

Point<int> Map_Entity::get_draw_position()
{
		int h = sprite->get_current_image()->h;
		return Point<int>((position.x+offset.x)*8, (position.y+offset.y+1)*8-h);
}

void Map_Entity::stop()
{
	moving = false;
	// FIXME: add all buttons here
	brain->l = brain->r = brain->u = brain->d = brain->b1 = false;
}

bool Map_Entity::maybe_move(Map *map)
{
	if (brain->l) {
		if (map->is_solid(-1, position+Point<int>(-1, 0)) == false) {
			moving = true;
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
		if (map->is_solid(-1, position+Point<int>(1, 0)) == false) {
			moving = true;
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
		if (map->is_solid(-1, position+Point<int>(0, -1)) == false) {
			moving = true;
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
		if (map->is_solid(-1, position+Point<int>(0, 1)) == false) {
			moving = true;
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
	return false;
}

void Map_Entity::handle_event(SDL_Event *event)
{
	brain->handle_event(event);
}

bool Map_Entity::update(Map *map)
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
				if (maybe_move(map) == false) {
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
				if (maybe_move(map) == false) {
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
				if (maybe_move(map) == false) {
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
				if (maybe_move(map) == false) {
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
	int add = moving ? -(((SDL_GetTicks() / 100) % 2) * bounce) : 0;
	sprite->get_current_image()->draw_single(draw_pos.x, draw_pos.y+add);
}
#include "map.h"
#include "map_entity.h"

static int current_id;

Map_Entity::Map_Entity(Brain *brain) :
	map(NULL),
	direction(S),
	anim(NULL),
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
	delete anim;
}

void Map_Entity::set_map(Map *map)
{
	this->map = map;
}

void Map_Entity::load_animation_set(std::string name)
{
	anim = new Animation_Set(name + "/animations.xml", name);
	anim->set_animation("stand_s");
}

void Map_Entity::set_position(Point<int> position)
{
	this->position = position;
}

void Map_Entity::set_bounce(int bounce)
{
	this->bounce = bounce;
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
		int h = anim->get_current_image()->h;
		return Point<int>((position.x+offset.x)*8, (position.y+offset.y+1)*8-h);
}

bool Map_Entity::maybe_move()
{
	if (brain->l) {
		if (map->is_solid(-1, position+Point<int>(-1, 0)) == false) {
			moving = true;
			offset = Point<float>(1, 0);
			position += Point<int>(-1, 0);
			anim->set_animation("walk_w");
			anim->reset();
			anim->start();
			return true;
		}
		else {
			anim->set_animation("stand_w");
		}
		direction = W;
	}
	else if (brain->r) {
		if (map->is_solid(-1, position+Point<int>(1, 0)) == false) {
			moving = true;
			offset = Point<float>(-1, 0);
			position += Point<int>(1, 0);
			anim->set_animation("walk_e");
			anim->reset();
			anim->start();
			return true;
		}
		else {
			anim->set_animation("stand_e");
		}
		direction = E;
	}
	else if (brain->u) {
		if (map->is_solid(-1, position+Point<int>(0, -1)) == false) {
			moving = true;
			offset = Point<float>(0, 1);
			position += Point<int>(0, -1);
			anim->set_animation("walk_n");
			anim->reset();
			anim->start();
			return true;
		}
		else {
			anim->set_animation("stand_n");
		}
		direction = N;
	}
	else if (brain->d) {
		if (map->is_solid(-1, position+Point<int>(0, 1)) == false) {
			moving = true;
			offset = Point<float>(0, -1);
			position += Point<int>(0, 1);
			anim->set_animation("walk_s");
			anim->reset();
			anim->start();
			return true;
		}
		else {
			anim->set_animation("stand_s");
		}
		direction = S;
	}
	return false;
}

void Map_Entity::handle_event(SDL_Event *event)
{
	brain->handle_event(event);
}

bool Map_Entity::update()
{
	if (moving == false) {
		maybe_move();
	}

	if (moving) {
		if (offset.x < 0) {
			offset.x += speed;
			if (offset.x >= 0) {
				offset.x = 0;
				map->check_triggers(this);
				if (maybe_move() == false) {
					moving = false;
					anim->stop();
					anim->set_animation("stand_e");
				}
			}
		}
		else if (offset.x > 0) {
			offset.x -= speed;
			if (offset.x <= 0) {
				offset.x = 0;
				map->check_triggers(this);
				if (maybe_move() == false) {
					moving = false;
					anim->stop();
					anim->set_animation("stand_w");
				}
			}
		}
		if (offset.y < 0) {
			offset.y += speed;
			if (offset.y >= 0) {
				offset.y = 0;
				map->check_triggers(this);
				if (maybe_move() == false) {
					moving = false;
					anim->stop();
					anim->set_animation("stand_s");
				}
			}
		}
		else if (offset.y > 0) {
			offset.y -= speed;
			if (offset.y <= 0) {
				offset.y = 0;
				map->check_triggers(this);
				if (maybe_move() == false) {
					moving = false;
					anim->stop();
					anim->set_animation("stand_n");
				}
			}
		}
	}

	anim->update();

	return true;
}

void Map_Entity::draw(Point<int> draw_pos)
{
	int add = moving ? -(((SDL_GetTicks() / 100) % 2) * bounce) : 0;
	anim->get_current_image()->draw_single(draw_pos.x, draw_pos.y+add);
}
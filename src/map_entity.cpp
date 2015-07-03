#include "starsquatters.h"
#include "map_entity.h"

Map_Entity::Map_Entity(Brain *brain) :
	brain(brain),
	anim(NULL),
	position(0, 0),
	moving(false),
	speed(0.1f),
	offset(0.0f, 0.0f)
{
}

Map_Entity::~Map_Entity()
{
	delete brain;
	delete anim;
}

bool Map_Entity::load_animation_set(std::string name)
{
	anim = new Animation_Set();
	if (anim == NULL) {
		return false;
	}
	if (anim->load(name + "/animations.xml", name) == false) {
		delete anim;
		return false;
	}
	anim->set_animation("stand_s");
	return true;
}

void Map_Entity::set_position(Point<int> position)
{
	this->position = position;
}

bool Map_Entity::maybe_move(Tilemap *map)
{
	if (brain->l) {
		if (map->is_solid(position+Point<int>(-1, 0), -1) == false) {
			moving = true;
			offset = Point<float>(1, 0);
			position += Point<int>(-1, 0);
			anim->set_animation("walk_w");
			anim->reset();
			anim->start();
			return true;
		}
	}
	else if (brain->r) {
		if (map->is_solid(position+Point<int>(1, 0), -1) == false) {
			moving = true;
			offset = Point<float>(-1, 0);
			position += Point<int>(1, 0);
			anim->set_animation("walk_e");
			anim->reset();
			anim->start();
			return true;
		}
	}
	else if (brain->u) {
		if (map->is_solid(position+Point<int>(0, -1), -1) == false) {
			moving = true;
			offset = Point<float>(0, 1);
			position += Point<int>(0, -1);
			anim->set_animation("walk_n");
			anim->reset();
			anim->start();
			return true;
		}
	}
	else if (brain->d) {
		if (map->is_solid(position+Point<int>(0, 1), -1) == false) {
			moving = true;
			offset = Point<float>(0, -1);
			position += Point<int>(0, 1);
			anim->set_animation("walk_s");
			anim->reset();
			anim->start();
			return true;
		}
	}
	return false;
}

bool Map_Entity::update(Tilemap *map)
{
	if (moving == false) {
		maybe_move(map);
	}

	if (moving) {
		if (offset.x < 0) {
			offset.x += speed;
			if (offset.x >= 0) {
				offset.x = 0;
				if (maybe_move(map) == false) {
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
				if (maybe_move(map) == false) {
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
				if (maybe_move(map) == false) {
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
				if (maybe_move(map) == false) {
					moving = false;
					anim->stop();
					anim->set_animation("stand_n");
				}
			}
		}
	}

	return true;
}

void Map_Entity::draw()
{
	Image *image = anim->get_current_image();
	if (image) {
		image->draw_single((position.x+offset.x)*8, (position.y+offset.y)*8);
	}
}
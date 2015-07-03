#include "starsquatters.h"
#include "map_entity.h"

Map_Entity::Map_Entity(Brain *brain) :
	brain(brain),
	anim(NULL),
	position(0, 0),
	moving(false),
	speed(0.1f, 0.1f),
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
	return true;
}

void Map_Entity::set_position(Point<int> position)
{
	this->position = position;
}

bool Map_Entity::update()
{
	return true;
}

void Map_Entity::draw()
{
	Image *image = anim->get_current_image();
	if (image) {
		image->draw_single(position);
	}
}
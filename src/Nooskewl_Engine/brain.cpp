#include "Nooskewl_Engine/brain.h"
#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/map_entity.h"

using namespace Nooskewl_Engine;

Brain::Brain() :
	map_entity(0)
{
	reset();
}

Brain::~Brain()
{
}

void Brain::handle_event(TGUI_Event *event)
{
}

void Brain::update()
{
}

bool Brain::activate(Map_Entity *activator)
{
	return false;
}

void Brain::collide(Map_Entity *collider)
{
}

void Brain::reset()
{
	l = r = u = d = b1 = false;
}

bool Brain::save(std::string &out)
{
	out += string_printf("brain=0\n");
	return true;
}

void Brain::set_map_entity(Map_Entity *map_entity)
{
	this->map_entity = map_entity;
}

Map_Entity *Brain::get_map_entity()
{
	return map_entity;
}

bool Brain::killme()
{
	return false;
}

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

void Brain::init()
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
	init();
}

Map_Entity *Brain::get_map_entity()
{
	return map_entity;
}

bool Brain::killme()
{
	return false;
}

//--

Light_Brain::Light_Brain(Vec3D<float> position, SDL_Colour colour, float reach, float falloff) :
	position(position),
	colour(colour),
	reach(reach),
	falloff(falloff)
{
}

Light_Brain::~Light_Brain()
{
}

Vec3D<float> Light_Brain::get_position()
{
	return position;
}

SDL_Colour Light_Brain::get_colour()
{
	return colour;
}

float Light_Brain::get_reach()
{
	return reach;
}

float Light_Brain::get_falloff()
{
	return falloff;
}

void Light_Brain::set_position(Vec3D<float> position)
{
	this->position = position;
}

void Light_Brain::set_colour(SDL_Colour colour)
{
	this->colour = colour;
}

void Light_Brain::set_reach(float reach)
{
	this->reach = reach;
}

void Light_Brain::set_falloff(float falloff)
{
	this->falloff = falloff;
}

bool Light_Brain::save(std::string &out)
{
	out += string_printf("brain=light_brain,1\n%f,%f,%f,%d,%d,%d,%f,%f\n", position.x, position.y, position.z, colour.r, colour.g, colour.b, reach, falloff);
	return true;
}

//--

Flickering_Light_Brain::Flickering_Light_Brain(Vec3D<float> position, SDL_Colour colour1, SDL_Colour colour2, int delaymin, int delaymax, float reach, float falloff) :
	Light_Brain(position, colour1, reach, falloff),
	current_colour(&colour),
	colour2(colour2),
	delaymin(delaymin),
	delaymax(delaymax),
	count(0)
{
}

Flickering_Light_Brain::~Flickering_Light_Brain()
{
}

SDL_Colour Flickering_Light_Brain::get_colour()
{
	return *current_colour;
}

SDL_Colour Flickering_Light_Brain::get_colour1()
{
	return colour;
}

SDL_Colour Flickering_Light_Brain::get_colour2()
{
	return colour2;
}

void Flickering_Light_Brain::set_colour2(SDL_Colour colour2)
{
	this->colour2 = colour2;
}

void Flickering_Light_Brain::update()
{
	count++;

	if (count >= delaymax) {
		count = 0;
		swap();
	}
	else if (count >= delaymin) {
		int r = count - delaymin;
		if ((rand() % (delaymax-delaymin)) <= r) {
			count = 0;
			swap();
		}
	}
}

bool Flickering_Light_Brain::save(std::string &out)
{
	out += string_printf("brain=flickering_light_brain,1\n%f,%f,%f,%d,%d,%d,%f,%f,%d,%d,%d,%d,%d\n", position.x, position.y, position.z, colour.r, colour.g, colour.b, reach, falloff, colour2.r, colour2.g, colour2.b, delaymin, delaymax);
	return true;
}

void Flickering_Light_Brain::swap()
{
	if (current_colour == &colour) {
		current_colour = &colour2;
		falloff++;
	}
	else {
		current_colour = &colour;
		falloff--;
	}
}

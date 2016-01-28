// This is a building block and does nothing itself
#ifndef BRAIN_H
#define BRAIN_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/basic_types.h"

namespace Nooskewl_Engine {

class Map_Entity;

class NOOSKEWL_ENGINE_EXPORT Brain {
public:
	bool l, r, u, d, b1;

	Brain();
	virtual ~Brain();

	virtual void init(); // called immediately after setting map_entity in set_map_entity
	virtual void handle_event(TGUI_Event *event);
	virtual void update();
	virtual bool activate(Map_Entity *activator);
	virtual void collide(Map_Entity *collider);
	virtual void reset();
	virtual void set_map_entity(Map_Entity *map_entity);
	virtual bool save(std::string &out);
	virtual bool killme(); // return true to die before being added to map

	Map_Entity *get_map_entity();

protected:
	Map_Entity *map_entity;
};

class NOOSKEWL_ENGINE_EXPORT Light_Brain : public Brain {
public:
	Light_Brain(Vec3D<float> position, SDL_Colour colour, float reach, float falloff);
	virtual ~Light_Brain();

	virtual Vec3D<float> get_position();
	virtual SDL_Colour get_colour();
	virtual float get_reach();
	virtual float get_falloff();

	void set_position(Vec3D<float> position);
	void set_colour(SDL_Colour colour);
	void set_reach(float reach);
	void set_falloff(float falloff);

	virtual bool save(std::string &out);

protected:
	Vec3D<float> position;
	SDL_Colour colour;
	float reach; // tiles of max power
	float falloff; // tiles till falloff completely
};

class NOOSKEWL_ENGINE_EXPORT Flickering_Light_Brain : public Light_Brain {
public:
	// delaymin and delaymax in ticks (60 ticks per second)
	Flickering_Light_Brain(Vec3D<float> position, SDL_Colour colour1, SDL_Colour colour2, int delaymin, int delaymax, float reach, float falloff);
	virtual ~Flickering_Light_Brain();

	SDL_Colour get_colour();
	SDL_Colour get_colour1();
	SDL_Colour get_colour2();
	void set_colour2(SDL_Colour colour2);

	void update();
	bool save(std::string &out);

protected:
	void swap();

	SDL_Colour *current_colour;
	SDL_Colour colour2;
	int delaymin;
	int delaymax;
	int count;
};

} // End namespace Nooskewl_Engine

#endif // BRAIN_H

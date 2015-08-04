// This is a building block and does nothing itself
#ifndef BRAIN_H
#define BRAIN_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Map_Entity;

class NOOSKEWL_ENGINE_EXPORT Brain {
public:
	bool l, r, u, d, b1;

	Brain();
	virtual ~Brain();

	virtual void handle_event(TGUI_Event *event);
	virtual void update();
	virtual void activate(Map_Entity *activator);
	virtual void collide(Map_Entity *collider);
	virtual void reset();
	virtual bool save(SDL_RWops *file);
	virtual void set_map_entity(Map_Entity *map_entity);

protected:
	Map_Entity *map_entity;
};

} // End namespace Nooskewl_Engine

#endif // BRAIN_H

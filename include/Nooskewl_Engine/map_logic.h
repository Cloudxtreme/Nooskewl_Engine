#ifndef MAP_LOGIC_H
#define MAP_LOGIC_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Map_Entity;

class NOOSKEWL_ENGINE_EXPORT Map_Logic {
public:
	virtual void start(bool been_here_before);
	virtual void end();
	virtual void trigger(Map_Entity *entity);
	virtual void update();
	virtual void activate(Map_Entity *activator, Map_Entity *activated);
	virtual Map_Entity *mutate_loaded_entity(Map_Entity *entity);
};

} // End namespace Nooskewl_Engine

#endif // MAP_LOGIC_H

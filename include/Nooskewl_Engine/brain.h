// This is a building block and does nothing itself
#ifndef BRAIN_H
#define BRAIN_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/map_entity.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Brain {
public:
	bool l, r, u, d, b1;

	Brain();
	virtual ~Brain();

	virtual void handle_event(TGUI_Event *event);
	virtual void activate(Map_Entity *activator, Map_Entity *activated);
	virtual void reset();
	virtual bool save(SDL_RWops *file);
};

} // End namespace Nooskewl_Engine

#endif // BRAIN_H
#ifndef BRAIN_H
#define BRAIN_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class NOOSKEWL_ENGINE_EXPORT Brain {
public:
	bool l, r, u, d, b1;

	Brain();
	virtual ~Brain();

	virtual void handle_event(TGUI_Event *event) = 0;
	virtual void reset();
};

} // End namespace Nooskewl_Engine

#endif // BRAIN_H
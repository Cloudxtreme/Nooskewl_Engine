#ifndef BRAIN_H
#define BRAIN_H

#include "Nooskewl_Engine/main.h"

class EXPORT Brain {
public:
	bool l, r, u, d, b1;

	Brain();
	virtual ~Brain();

	virtual void handle_event(TGUI_Event *event) = 0;
};

#endif // BRAIN_H
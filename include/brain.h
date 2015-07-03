#ifndef BRAIN_H
#define BRAIN_H

#include "starsquatters.h"

class Brain {
public:
	bool l, r, u, d, b1;

	Brain();
	virtual ~Brain();

	virtual void handle_event(SDL_Event *event) = 0;
};

#endif // BRAIN_H
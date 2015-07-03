#ifndef BRAIN_H
#define BRAIN_H

#include "starsquatters.h"

class Brain {
public:
	bool l, r, u, d, b1;

	Brain();
	virtual ~Brain();

	virtual void update(void *input) = 0;
};

class Player_Brain : public Brain {
public:
		Player_Brain();
		~Player_Brain();

		void update(void *input);
};

#endif // BRAIN_H
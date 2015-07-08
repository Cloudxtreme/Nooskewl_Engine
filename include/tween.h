#ifndef TWEEN_H
#define TWEEN_H

#include "starsquatters.h"
#include "dllist.h"

class Tween {
public:
	bool started;
	Uint32 start_time;

	Tween();
	virtual ~Tween();

	virtual bool update(Tween *tween) = 0;
};

typedef std::vector< DLList<Tween *> * > Tweens;

void update_tweens(Tweens tweens);
void destroy_tweens(Tweens tweens);

#endif // TWEEN_H
#ifndef TWEEN_H
#define TWEEN_H

#include "Nooskewl_Engine/main.h"
#include "Nooskewl_Engine/dllist.h"

class EXPORT Tween {
public:
	bool started;
	Uint32 start_time;

	Tween();
	virtual ~Tween();

	virtual bool update(Tween *tween) = 0;
};

typedef std::vector< DLList<Tween *> * > Tweens;

EXPORT void update_tweens(Tweens tweens);
EXPORT void destroy_tweens(Tweens tweens);

#endif // TWEEN_H
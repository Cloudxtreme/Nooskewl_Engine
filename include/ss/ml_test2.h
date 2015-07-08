#ifndef SS_ML_TEST2_H
#define SS_ML_TEST2_H

#include "map_logic.h"
#include "tween.h"

class SS_ML_Test2 : public Map_Logic {
public:
	void start(Map *map);
	void end(Map *map);
	void trigger(Map *map, Map_Entity *entity);
	void update(Map *map);

private:
	Tweens tweens;
	Map_Entity *test;
};

#endif // SS_ML_TEST2_H
#ifndef SS_ML_TEST_H
#define SS_ML_TEST_H

#include "map_logic.h"
#include "tween.h"

class SS_ML_Test : public Map_Logic {
public:
	void trigger(Map *map, Map_Entity *entity);
};

#endif // SS_ML_TEST_H
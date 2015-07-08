#include "map_entity.h"
#include "tween.h"
#include "types.h"

class SS_Move_Tween : public Tween {
public:
	Map_Entity *entity;
	Point<int> start_pos;
	Point<int> destination;
	Uint32 start_time;
	float speed; // pixels/second

	bool update(Tween *tween);
};
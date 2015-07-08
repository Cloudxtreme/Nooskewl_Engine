#ifndef MAP_LOGIC_H
#define MAP_LOGIC_H

class Map;
class Map_Entity;

class Map_Logic {
public:
	virtual void start(Map *map);
	virtual void end(Map *map);
	virtual void trigger(Map *map, Map_Entity *entity);
	virtual void update(Map *map);
};

#endif // MAP_LOGIC_H
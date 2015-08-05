#ifndef INVENTORY_H
#define INVENTORY_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Item;

class Inventory {
public:
	Uint32 gold;

	Inventory();

	void add(Item *item);
	void remove(Item *item);

	int get_total_weight();

private:
	int find(Item *item);

	std::vector< std::vector<Item *> > items;
};

} // End namespace Nooskewl_Engine

#endif // INVENTORY_H

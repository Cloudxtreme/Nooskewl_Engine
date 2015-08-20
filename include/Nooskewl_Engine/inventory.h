#ifndef INVENTORY_H
#define INVENTORY_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Item;

class NOOSKEWL_ENGINE_EXPORT Inventory {
public:
	int32_t gold;
	std::vector< std::vector<Item *> > items;

	Inventory();

	void add(Item *item);
	void remove(Item *item);

	int get_total_weight();

private:
	int find(Item *item);
};

} // End namespace Nooskewl_Engine

#endif // INVENTORY_H

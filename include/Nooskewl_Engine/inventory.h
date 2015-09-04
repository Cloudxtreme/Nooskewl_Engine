#ifndef INVENTORY_H
#define INVENTORY_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Item;

class NOOSKEWL_ENGINE_EXPORT Inventory {
public:
	static std::string weight_to_string(int weight);

	int32_t gold;
	std::vector< std::vector<Item *> > items;

	Inventory();
	~Inventory();

	void add(Item *item);
	void remove(Item *item);

	int get_total_weight();
	void sort(int start = -1, int end = -1);
	std::string to_string();
	void from_string(std::string s);

private:
	int find(Item *item);
};

} // End namespace Nooskewl_Engine

#endif // INVENTORY_H

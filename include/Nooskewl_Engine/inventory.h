#ifndef INVENTORY_H
#define INVENTORY_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

class Item;

class NOOSKEWL_ENGINE_EXPORT Inventory {
public:
	static std::string decimal_to_string(int decimal);

	int32_t gold;
	std::vector< std::vector<Item *> > items;

	Inventory();
	~Inventory();

	void add(Item *item);
	void remove(Item *item);
	int find(std::string id); // < 0 == no match

	int get_total_weight();
	void sort(int start = -1, int end = -1);
	std::string to_string();
	void from_string(std::string s);
	Inventory *clone();
	void remove(Inventory *inventory); // remove matching items
	void remove(std::string id); // remove first item with id == id

private:
	int find(Item *item);
};

} // End namespace Nooskewl_Engine

#endif // INVENTORY_H

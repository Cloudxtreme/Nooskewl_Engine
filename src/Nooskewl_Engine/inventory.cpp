#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"

using namespace Nooskewl_Engine;

Inventory::Inventory()
{
	gold = 0;
}

void Inventory::add(Item *item)
{
	int index = find(item);

	if (index < 0) {
		std::vector<Item *> v;
		v.push_back(item);
		items.push_back(v);
	}
	else {
		items[index].push_back(item);
	}
}

void Inventory::remove(Item *item)
{
	int index = find(item);
	if (index >= 0) {
		items[index].pop_back();
		if (items[index].size() == 0) {
			items.erase(items.begin() + index);
		}
	}
}

int Inventory::get_total_weight()
{
	int total_weight = 0;

	for (size_t i = 0; i < items.size(); i++) {
		for (size_t j = 0; j < items.size(); j++) {
			total_weight += items[i][j]->weight;
		}
	}

	return total_weight;
}

int Inventory::find(Item *item)
{
	int index = -1;

	for (size_t i = 0; i < items.size(); i++) {
		if (items[i].size() > 0) {
			Item *it = items[i][0];
			if (it->name == item->name) {
				index = i;
				break;
			}
		}
	}

	return index;
}

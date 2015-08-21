#include "Nooskewl_Engine/internal.h"
#include "Nooskewl_Engine/inventory.h"
#include "Nooskewl_Engine/item.h"
#include "Nooskewl_Engine/stats.h"

using namespace Nooskewl_Engine;

static bool sort_items(const std::vector<Item *> &a, const std::vector<Item *> &b)
{
	return a[0]->name < b[0]->name;
}

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

	sort();
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

void Inventory::sort(int start, int end)
{
	std::vector< std::vector<Item *> >::iterator start_it;
	std::vector< std::vector<Item *> >::iterator end_it;

	if (start < 0) {
		start_it = items.begin();
		end_it = items.end();
	}
	else {
		start_it = items.begin() + start;
		if (end < 0 || end >= (int)items.size()) {
			end_it = items.end();
		}
		else {
			end_it = items.begin() + end;
		}
	}

	std::sort(start_it, end_it, sort_items);
}

std::string Inventory::to_string()
{
	std::string s = itos(gold) + ",";
	int count = 0;
	for (size_t i = 0; i < items.size(); i++) {
		int num = items[i].size();
		if (num != 0) {
			Item *item = items[i][0];
			s += string_printf("%s%s=%d", count == 0 ? "" : ",", item->id.c_str(), num);
			count++;
		}
	}

	return s;
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

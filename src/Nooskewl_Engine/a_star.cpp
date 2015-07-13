#include "Nooskewl_Engine/a_star.h"

using namespace Nooskewl_Engine;

A_Star::A_Star(Tilemap *tilemap) :
	tilemap(tilemap)
{
}

A_Star::~A_Star()
{
	destroy_nodes(open);
	destroy_nodes(closed);
}

std::list<A_Star::Node *> A_Star::find_path(Point<int> start, Point<int> goal)
{
	destroy_nodes(open);
	destroy_nodes(closed);

	Node *start_node = new Node();
	start_node->parent = NULL;
	start_node->position = start;
	start_node->cost_from_start = 0;
	start_node->cost_to_goal = (goal-start).length();
	start_node->total_cost = start_node->cost_from_start + start_node->cost_to_goal;
	open.push_back(start_node);

	while (open.size() > 0) {
		Node *node = open.front();
		open.pop_front();

		if (node->position == goal) {
			std::list<Node *> path;
			while (node->parent != NULL) {
				path.push_front(node);
				node = node->parent;
			}
			return path;
		}
		else {
			branch(node, Point<int>(0, -1), goal);
			branch(node, Point<int>(-1, 0), goal);
			branch(node, Point<int>(1, 0), goal);
			branch(node, Point<int>(0, 1), goal);
		}
		closed.push_back(node);
	}

	return std::list<Node *>(); // failed
}

A_Star::Node *A_Star::find_in_list(Point<int> position, std::list<Node *> &list)
{
	for (std::list<Node *>::iterator it = list.begin(); it != list.end(); it++) {
		Node *n = *it;
		if (n->position == position) {
			return n;
		}
	}

	return NULL;
}

void A_Star::remove_from_list(Node *node, std::list<Node *> &list)
{
	std::list<Node *>::iterator it = std::find(list.begin(), list.end(), node);
	if (it != list.end()) {
		list.erase(it);
	}
}

void A_Star::add_to_open(Node *node)
{
	std::list<Node *>::iterator it;
	for (it = open.begin(); it != open.end(); it++) {
		Node *n = *it;
		if (n->total_cost >= node->total_cost) {
			break;
		}
	}
	open.insert(it, node);
}

void A_Star::destroy_nodes(std::list<Node *> &list)
{
	for (std::list<Node *>::iterator it = list.begin(); it != list.end();) {
		delete *it;
		it = list.erase(it);
	}
}

void A_Star::branch(Node *node, Point<int> offset, Point<int> goal)
{
	Point<int> new_position = node->position + offset;
	if (tilemap->is_solid(-1, new_position)) {
		return;
	}

	float new_cost = node->cost_from_start + 1.0f;
	bool in_open = false;
	bool in_closed = false;
	Node *new_node = find_in_list(new_position, open);
	if (new_node == NULL) {
		in_open = false;
		new_node = find_in_list(new_position, closed);
		if (new_node == NULL) {
			in_closed = false;
		}
		else {
			in_closed = true;
		}
	}
	else {
		in_open = true;
		in_closed = find_in_list(new_position, closed) != NULL;
	}
	if (new_node == NULL || new_node->cost_from_start > new_cost) {
		if (new_node == NULL) {
			new_node = new Node();
			new_node->position = new_position;
		}
		new_node->parent = node;
		new_node->cost_from_start = new_cost;
		new_node->cost_to_goal = (goal-new_node->position).length();
		new_node->total_cost = new_node->cost_from_start + new_node->cost_to_goal;
		if (in_closed) {
			remove_from_list(new_node, closed);
		}
		if (in_open) {
			remove_from_list(new_node, open);
			add_to_open(new_node);
		}
		else {
			add_to_open(new_node);
		}
	}
}

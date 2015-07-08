#include "dllist.h"

DLList_Node::DLList_Node(void *data) :
	prev(0),
	next(0),
	data(data)
{
}

void DLList_Node::remove()
{
	DLList_Node *next = next;
	prev->next = next;
	if (next) {
		next->prev = prev;
	}
}

void DLList_Node::insert_before(void *data)
{
	DLList_Node *node = new DLList_Node(data);
	prev->next = node;
	node->prev = prev;
	node->next = this;
	prev = node;
}

void DLList_Node::insert_after(void *data)
{
	DLList_Node *node = new DLList_Node(data);
	next->prev = node;
	node->prev = this;
	node->next = next;
	next = node;
}

DLList::DLList() :
	nodes(0)
{
}

DLList::~DLList()
{
	DLList_Node *node = nodes;
	DLList_Node *last = node->prev;
	while (node != last) {
		delete node->data;
		DLList_Node *next = node->next;
		delete node;
		node = next;
	}
	if (node) {
		delete node->data;
		delete node;
	}
}

DLList_Node *DLList::push_back(void *data)
{
	if (nodes == 0) {
		nodes = new DLList_Node(data);
		return nodes;
	}
	else {
		DLList_Node *node = nodes;
		while (node->next) {
			node = node->next;
		}
		node->next = new DLList_Node(data);
		node->next->prev = node;
		return node->next;
	}
}
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

void DLList::push_back(void *data)
{
	if (nodes == 0) {
		nodes = new DLList_Node(data);
	}
	else {
		DLList_Node *prev = nodes->prev;
		prev->next = new DLList_Node(data);
		prev->next->prev = prev;
	}
}
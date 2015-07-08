#ifndef DLLIST_H
#define DLLIST_H

#include "Nooskewl_Engine/main.h"

template<typename T> struct DLList_Node {
	T data;
	DLList_Node<T> *prev;
	DLList_Node<T> *next;

	DLList_Node::DLList_Node(T data) :
		prev(0),
		next(0),
		data(data)
	{
	}

	void DLList_Node::remove()
	{
		DLList_Node<T> *next = next;
		prev->next = next;
		if (next) {
			next->prev = prev;
		}
	}

	void DLList_Node::insert_before(T data)
	{
		DLList_Node<T> *node = new DLList_Node<T>(data);
		prev->next = node;
		node->prev = prev;
		node->next = this;
		prev = node;
	}

	void DLList_Node::insert_after(T data)
	{
		DLList_Node<T> *node = new DLList_Node(data);
		next->prev = node;
		node->prev = this;
		node->next = next;
		next = node;
	}
};

template<typename T> struct DLList {
	DLList_Node<T> *nodes;

	DLList::DLList() :
		nodes(0)
	{
	}

	DLList::~DLList()
	{
		DLList_Node<T> *node = nodes;
		DLList_Node<T> *last = node->prev;
		while (node != last) {
			delete node->data;
			DLList_Node<T> *next = node->next;
			delete node;
			node = next;
		}
		if (node) {
			delete node->data;
			delete node;
		}
	}

	DLList_Node<T> *DLList::push_back(T data)
	{
		if (nodes == 0) {
			nodes = new DLList_Node<T>(data);
			return nodes;
		}
		else {
			DLList_Node<T> *node = nodes;
			while (node->next) {
				node = node->next;
			}
			node->next = new DLList_Node<T>(data);
			node->next->prev = node;
			return node->next;
		}
	}
};

#endif // DLLIST_H

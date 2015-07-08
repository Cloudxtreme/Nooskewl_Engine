#include "dllist.h"

template<typename T> DLList_Node<T>::DLList_Node(T data) :
	prev(0),
	next(0),
	data(data)
{
}

template<typename T> void DLList_Node<T>::remove()
{
	DLList_Node<T> *next = next;
	prev->next = next;
	if (next) {
		next->prev = prev;
	}
}

template<typename T> void DLList_Node<T>::insert_before(T data)
{
	DLList_Node<T> *node = new DLList_Node<T>(data);
	prev->next = node;
	node->prev = prev;
	node->next = this;
	prev = node;
}

template<typename T> void DLList_Node<T>::insert_after(T data)
{
	DLList_Node<T> *node = new DLList_Node(data);
	next->prev = node;
	node->prev = this;
	node->next = next;
	next = node;
}

template<typename T> DLList<T>::DLList() :
	nodes(0)
{
}

template<typename T> DLList<T>::~DLList()
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

template<typename T> DLList_Node<T> *DLList<T>::push_back(T data)
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
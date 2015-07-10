// Doubly-linked list

#ifndef DLLIST_H
#define DLLIST_H

#include "Nooskewl_Engine/main.h"

namespace Nooskewl_Engine {

template<typename T> struct DLList {
	template<typename T> struct Node {
		T data;
		Node<T> *prev;
		Node<T> *next;

		Node::Node(T data) :
			prev(0),
			next(0),
			data(data)
		{
		}

		void Node::remove()
		{
			Node<T> *next = next;
			prev->next = next;
			if (next) {
				next->prev = prev;
			}
		}

		void Node::insert_before(T data)
		{
			Node<T> *node = new Node<T>(data);
			prev->next = node;
			node->prev = prev;
			node->next = this;
			prev = node;
		}

		void Node::insert_after(T data)
		{
			Node<T> *node = new Node<T>(data);
			next->prev = node;
			node->prev = this;
			node->next = next;
			next = node;
		}
	};

	Node<T> *nodes;

	DLList::DLList() :
		nodes(0)
	{
	}

	DLList::~DLList()
	{
		Node<T> *node = nodes;
		Node<T> *last = node->prev;
		while (node != last) {
			delete node->data;
			Node<T> *next = node->next;
			delete node;
			node = next;
		}
		if (node) {
			delete node->data;
			delete node;
		}
	}

	Node<T> *DLList::push_back(T data)
	{
		if (nodes == 0) {
			nodes = new Node<T>(data);
			return nodes;
		}
		else {
			Node<T> *node = nodes;
			while (node->next) {
				node = node->next;
			}
			node->next = new Node<T>(data);
			node->next->prev = node;
			return node->next;
		}
	}
};

} // End namespace Nooskewl_Engine

#endif // DLLIST_H
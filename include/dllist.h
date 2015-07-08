#ifndef DLLIST_H
#define DLLIST_H

struct DLList_Node {
	DLList_Node *prev;
	DLList_Node *next;
	void *data;

	DLList_Node(void *data);

	void remove();
	void insert_before(void *data);
	void insert_after(void *data);
};

struct DLList {
	DLList_Node *nodes;

	DLList();
	~DLList();

	DLList_Node *push_back(void *data);
};

#endif // DLLIST_H
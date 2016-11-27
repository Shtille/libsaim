#ifndef __SAIM_LIST_H__
#define __SAIM_LIST_H__

#include <stdbool.h>

typedef struct saim_list_node {
	void * data;
	struct saim_list_node * prev;
	struct saim_list_node * next;
} saim_list_node;

typedef struct {
	void (*destroy_func)(void*);
	saim_list_node * head; //!< pointer to the first element
	saim_list_node * tail; //!< pointer to the last element
	unsigned int length;
} saim_list;

void saim_list_create(saim_list * list, void (*destroy_func)(void*));
void saim_list_destroy(saim_list * list); //! deletes all the data

void * saim_list_front(saim_list * list);
void * saim_list_back(saim_list * list);

bool saim_list_empty(saim_list * list);
void saim_list_clear(saim_list * list);
void saim_list_push_front(saim_list * list, void * data);
void saim_list_push_back(saim_list * list, void * data);
void * saim_list_pop_front(saim_list * list);
void * saim_list_pop_back(saim_list * list);
void * saim_list_delete(saim_list * list, saim_list_node * node);
void saim_list_sort(saim_list * list, bool (*sort_func)(const void*, const void*));
saim_list_node * saim_list_find(saim_list * list, const void * data, bool (*cmp_func)(const void*, const void*));

#endif
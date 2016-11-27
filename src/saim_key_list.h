#ifndef __SAIM_KEY_LIST_H__
#define __SAIM_KEY_LIST_H__

#include "util/saim_list.h"
#include "util/saim_set.h"
#include "saim_key_pair.h"

/*
* List stores set nodes.
*/

typedef struct {
	saim_list list;
} key_list_t;

void key_list_create(key_list_t * list);
void key_list_destroy(key_list_t * list);

void key_list_clear(key_list_t * list);
void key_list_insert(key_list_t * list, saim_set_node * node);
void key_list_delete(key_list_t * list, saim_list_node * node);
void key_list_sort(key_list_t * list);
saim_list_node * key_list_find_size_compatible(key_list_t * list, file_size_t size);
saim_list_node * key_list_get_first(key_list_t * list);

#endif
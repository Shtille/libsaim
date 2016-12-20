#ifndef __SAIM_MAP_NODES_LIST_H__
#define __SAIM_MAP_NODES_LIST_H__

#include "util/saim_list.h"
#include "util/saim_set.h"
#include "saim_key_pair.h"

/*
* List stores set nodes.
*/

typedef struct {
	saim_list list;
} saim_map_nodes_list;

void saim_map_nodes_list__create(saim_map_nodes_list * list);
void saim_map_nodes_list__destroy(saim_map_nodes_list * list);

void saim_map_nodes_list__clear(saim_map_nodes_list * list);
void saim_map_nodes_list__insert(saim_map_nodes_list * list, saim_set_node * node);
void saim_map_nodes_list__delete(saim_map_nodes_list * list, saim_list_node * node);
void saim_map_nodes_list__sort(saim_map_nodes_list * list);
saim_list_node * saim_map_nodes_list__find_size_compatible(saim_map_nodes_list * list, file_size_t size);
saim_list_node * saim_map_nodes_list__get_first(saim_map_nodes_list * list);

#endif
#ifndef __SAIM_STORAGE_INFO_MAP_H__
#define __SAIM_STORAGE_INFO_MAP_H__

#include "saim_storage_info_pair.h"
#include "util/saim_set.h"

typedef struct {
	saim_set * set;
} storage_info_map_t;

void storage_info_map_create(storage_info_map_t * map);
void storage_info_map_destroy(storage_info_map_t * map);

void storage_info_map_clear(storage_info_map_t * map);
saim_set_node * storage_info_map_insert(storage_info_map_t * map, storage_info_pair_t * pair);
void storage_info_map_erase(storage_info_map_t * map, saim_set_node * node);
void storage_info_map_cut(storage_info_map_t * map, saim_set_node * node);
saim_set_node * storage_info_map_search(storage_info_map_t * map, const saim_string * name);

#endif
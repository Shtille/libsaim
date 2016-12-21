#ifndef __SAIM_STORAGE_INFO_MAP_H__
#define __SAIM_STORAGE_INFO_MAP_H__

#include "saim_storage_info_pair.h"
#include "util/saim_set.h"

typedef struct {
	saim_set * set;
} saim_storage_info_map;

void saim_storage_info_map__create(saim_storage_info_map * map);
void saim_storage_info_map__destroy(saim_storage_info_map * map);

void saim_storage_info_map__clear(saim_storage_info_map * map);
saim_set_node * saim_storage_info_map__insert(saim_storage_info_map * map, saim_storage_info_pair * pair);
void saim_storage_info_map__erase(saim_storage_info_map * map, saim_set_node * node);
void saim_storage_info_map__cut(saim_storage_info_map * map, saim_set_node * node);
saim_set_node * saim_storage_info_map__search(saim_storage_info_map * map, const saim_string * name);

#endif
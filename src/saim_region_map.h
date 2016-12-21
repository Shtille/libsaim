#ifndef __SAIM_REGION_MAP_H__
#define __SAIM_REGION_MAP_H__

#include "saim_region_map_pair.h"
#include "util/saim_set.h"

typedef struct {
	saim_set * set;
} saim_region_map;

void saim_region_map__create(saim_region_map * map);
void saim_region_map__destroy(saim_region_map * map);

void saim_region_map__clear(saim_region_map * map);
saim_set_node * saim_region_map__insert(saim_region_map * map, saim_region_map_pair * pair);
void saim_region_map__erase(saim_region_map * map, saim_set_node * node);
void saim_region_map__cut(saim_region_map * map, saim_set_node * node);
saim_set_node * saim_region_map__search(saim_region_map * map, const saim_string * name);
saim_set_node * saim_region_map__search_text(saim_region_map * map, const char * name);

#endif
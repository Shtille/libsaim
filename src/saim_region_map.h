#ifndef __SAIM_REGION_MAP_H__
#define __SAIM_REGION_MAP_H__

#include "saim_region_map_pair.h"
#include "util/saim_set.h"

typedef struct {
	saim_set * set;
} region_map_t;

void region_map_create(region_map_t * map);
void region_map_destroy(region_map_t * map);

void region_map_clear(region_map_t * map);
saim_set_node * region_map_insert(region_map_t * map, region_map_pair_t * pair);
void region_map_erase(region_map_t * map, saim_set_node * node);
void region_map_cut(region_map_t * map, saim_set_node * node);
saim_set_node * region_map_search(region_map_t * map, const saim_string * name);
saim_set_node * region_map_search_text(region_map_t * map, const char * name);

#endif
#ifndef __SAIM_KEY_OFFSET_MAP__
#define __SAIM_KEY_OFFSET_MAP__

#include "util/saim_set.h"
#include "saim_key_pair.h"

typedef struct {
	saim_set * set;
} key_offset_map_t;

void key_offset_map_create(key_offset_map_t * map);
void key_offset_map_destroy(key_offset_map_t * map);

void key_offset_map_clear(key_offset_map_t * map);
saim_set_node * key_offset_map_insert(key_offset_map_t * map, key_pair_t * pair);
void key_offset_map_erase(key_offset_map_t * map, saim_set_node * node);

saim_set_node * key_offset_map_search(key_offset_map_t * map, const data_key_t * key);

#endif
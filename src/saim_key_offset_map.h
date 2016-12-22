#ifndef __SAIM_KEY_OFFSET_MAP_H__
#define __SAIM_KEY_OFFSET_MAP_H__

#include "util/saim_set.h"
#include "saim_key_pair.h"

typedef struct {
	saim_set * set;
} saim_key_offset_map;

void saim_key_offset_map__create(saim_key_offset_map * map);
void saim_key_offset_map__destroy(saim_key_offset_map * map);

void saim_key_offset_map__clear(saim_key_offset_map * map);
saim_set_node * saim_key_offset_map__insert(saim_key_offset_map * map, key_pair_t * pair);
void saim_key_offset_map__erase(saim_key_offset_map * map, saim_set_node * node);

saim_set_node * saim_key_offset_map__search(saim_key_offset_map * map, const saim_data_key * key);

#endif
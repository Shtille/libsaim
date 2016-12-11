#ifndef __SAIM_BITMAP_MAP_H__
#define __SAIM_BITMAP_MAP_H__

#include "saim_bitmap_info_pair.h"

#include "../util/saim_set.h"

typedef struct {
	saim_set * set;
} saim_bitmap_map;

void saim_bitmap_map__create(saim_bitmap_map * map);
void saim_bitmap_map__destroy(saim_bitmap_map * map);

unsigned int saim_bitmap_map__size(saim_bitmap_map * map);
void saim_bitmap_map__clear(saim_bitmap_map * map);
saim_set_node * saim_bitmap_map__insert(saim_bitmap_map * map, saim_bitmap_info_pair * pair);
void saim_bitmap_map__erase(saim_bitmap_map * map, saim_set_node * node);

saim_set_node * saim_bitmap_map__search(saim_bitmap_map * map, const data_key_t * key);

#endif
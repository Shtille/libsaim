#ifndef __SAIM_REGION_MAP_PAIR_H__
#define __SAIM_REGION_MAP_PAIR_H__

#include "util/saim_string.h"
#include "saim_region_info.h"

typedef struct {
	saim_string name;
	saim_stored_region_info info;
} saim_region_map_pair;

void saim_region_map_pair__create(saim_region_map_pair * pair);
void saim_region_map_pair__destroy(saim_region_map_pair * pair);

int saim_region_map_pair__compare(const saim_region_map_pair * a, const saim_region_map_pair * b);

#endif
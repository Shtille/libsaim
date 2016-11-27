#ifndef __SAIM_REGION_MAP_PAIR__
#define __SAIM_REGION_MAP_PAIR__

#include "util/saim_string.h"
#include "saim_region_info.h"

typedef struct {
	saim_string name;
	stored_region_info_t info;
} region_map_pair_t;

void region_map_pair_create(region_map_pair_t * pair);
void region_map_pair_destroy(region_map_pair_t * pair);

int region_map_pair_compare(const region_map_pair_t * a, const region_map_pair_t * b);

#endif
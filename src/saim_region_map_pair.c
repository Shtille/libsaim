#include "saim_region_map_pair.h"

void region_map_pair_create(region_map_pair_t * pair)
{
	saim_string_create(&pair->name);
	stored_region_info_create(&pair->info);
}
void region_map_pair_destroy(region_map_pair_t * pair)
{
	saim_string_destroy(&pair->name);
	stored_region_info_destroy(&pair->info);
}
int region_map_pair_compare(const region_map_pair_t * a, const region_map_pair_t * b)
{
	return saim_string_compare(&a->name, &b->name);
}
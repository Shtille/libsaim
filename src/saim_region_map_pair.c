#include "saim_region_map_pair.h"

void saim_region_map_pair__create(saim_region_map_pair * pair)
{
	saim_string_create(&pair->name);
	saim_stored_region_info__create(&pair->info);
}
void saim_region_map_pair__destroy(saim_region_map_pair * pair)
{
	saim_string_destroy(&pair->name);
	saim_stored_region_info__destroy(&pair->info);
}
int saim_region_map_pair__compare(const saim_region_map_pair * a, const saim_region_map_pair * b)
{
	return saim_string_compare(&a->name, &b->name);
}
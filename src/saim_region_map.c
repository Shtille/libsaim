#include "saim_region_map.h"

#include "util/saim_memory.h"

static int compare_func(const void * p1, const void * p2)
{
	return region_map_pair_compare((const region_map_pair_t *)p1, (const region_map_pair_t *)p2);
}
static void destroy_func(void * p)
{
	region_map_pair_t * pair = (region_map_pair_t *)p;
	region_map_pair_destroy(pair);
	SAIM_FREE(pair);
}

void region_map_create(region_map_t * map)
{
	map->set = saim_set_create(compare_func, destroy_func);
}
void region_map_destroy(region_map_t * map)
{
	saim_set_destroy(map->set);
}
void region_map_clear(region_map_t * map)
{
	saim_set_clear(map->set);
}
saim_set_node * region_map_insert(region_map_t * map, region_map_pair_t * pair)
{
	return saim_set_insert(map->set, pair);
}
void region_map_erase(region_map_t * map, saim_set_node * node)
{
	saim_set_delete(map->set, node);
}
void region_map_cut(region_map_t * map, saim_set_node * node)
{
	saim_set_cut(map->set, node);
}
saim_set_node * region_map_search(region_map_t * map, const saim_string * name)
{
	saim_set_node * node;
	region_map_pair_t pair;

	saim_string_create(&pair.name);
	saim_string_set(&pair.name, name);
	node = saim_set_search(map->set, &pair);
	saim_string_destroy(&pair.name);
	return node;
}
saim_set_node * region_map_search_text(region_map_t * map, const char * name)
{
	saim_set_node * node;
	region_map_pair_t pair;

	saim_string_create(&pair.name);
	saim_string_set_text(&pair.name, name);
	node = saim_set_search(map->set, &pair);
	saim_string_destroy(&pair.name);
	return node;
}
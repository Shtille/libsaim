#include "saim_bitmap_map.h"

#include "../util/saim_memory.h"

static int compare_func(const void * p1, const void * p2)
{
	return saim_bitmap_info_pair__compare((const saim_bitmap_info_pair *)p1, (const saim_bitmap_info_pair *)p2);
}
static void destroy_func(void * p)
{
	saim_bitmap_info_pair * pair = (saim_bitmap_info_pair *)p;
	saim_bitmap_info_pair__destroy(pair);
	SAIM_FREE(pair);
}

void saim_bitmap_map__create(saim_bitmap_map * map)
{
	map->set = saim_set_create(compare_func, destroy_func);
}
void saim_bitmap_map__destroy(saim_bitmap_map * map)
{
	saim_set_destroy(map->set);
}
unsigned int saim_bitmap_map__size(saim_bitmap_map * map)
{
	return map->set->size;
}
void saim_bitmap_map__clear(saim_bitmap_map * map)
{
	saim_set_clear(map->set);
}
saim_set_node * saim_bitmap_map__insert(saim_bitmap_map * map, saim_bitmap_info_pair * pair)
{
	return saim_set_insert(map->set, pair);
}
void saim_bitmap_map__erase(saim_bitmap_map * map, saim_set_node * node)
{
	saim_set_delete(map->set, node);
}
saim_set_node * saim_bitmap_map__search(saim_bitmap_map * map, const data_key_t * key)
{
	saim_bitmap_info_pair pair;
	pair.key = *key;
	return saim_set_search(map->set, &pair);
}
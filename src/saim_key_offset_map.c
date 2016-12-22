#include "saim_key_offset_map.h"

#include "util/saim_memory.h"

static int compare_func(const void * p1, const void * p2)
{
	return key_pair_compare((const key_pair_t *)p1, (const key_pair_t *)p2);
}
static void destroy_func(void * p)
{
	// key_pair_t is a simple struct, so do not need to deinitialize
	SAIM_FREE((key_pair_t *)p);
}

void saim_key_offset_map__create(saim_key_offset_map * map)
{
	map->set = saim_set_create(compare_func, destroy_func);
}
void saim_key_offset_map__destroy(saim_key_offset_map * map)
{
	saim_set_destroy(map->set);
}
void saim_key_offset_map__clear(saim_key_offset_map * map)
{
	saim_set_clear(map->set);
}
saim_set_node * saim_key_offset_map__insert(saim_key_offset_map * map, key_pair_t * pair)
{
	return saim_set_insert(map->set, pair);
}
void saim_key_offset_map__erase(saim_key_offset_map * map, saim_set_node * node)
{
	saim_set_delete(map->set, node);
}
saim_set_node * saim_key_offset_map__search(saim_key_offset_map * map, const saim_data_key * key)
{
	key_pair_t pair;
	pair.key = *key;
	return saim_set_search(map->set, &pair);
}
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

void key_offset_map_create(key_offset_map_t * map)
{
	map->set = saim_set_create(compare_func, destroy_func);
}
void key_offset_map_destroy(key_offset_map_t * map)
{
	saim_set_destroy(map->set);
}
void key_offset_map_clear(key_offset_map_t * map)
{
	saim_set_clear(map->set);
}
saim_set_node * key_offset_map_insert(key_offset_map_t * map, key_pair_t * pair)
{
	return saim_set_insert(map->set, pair);
}
void key_offset_map_erase(key_offset_map_t * map, saim_set_node * node)
{
	saim_set_delete(map->set, node);
}
saim_set_node * key_offset_map_search(key_offset_map_t * map, const data_key_t * key)
{
	key_pair_t pair;
	pair.key = *key;
	return saim_set_search(map->set, &pair);
}
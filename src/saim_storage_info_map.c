#include "saim_storage_info_map.h"

#include "util/saim_memory.h"

static int compare_func(const void * p1, const void * p2)
{
	return storage_info_pair_compare((const storage_info_pair_t *)p1, (const storage_info_pair_t *)p2);
}
static void destroy_func(void * p)
{
	storage_info_pair_t * pair = (storage_info_pair_t *)p;
	storage_info_pair_destroy(pair);
	SAIM_FREE(pair);
}

void storage_info_map_create(storage_info_map_t * map)
{
	map->set = saim_set_create(compare_func, destroy_func);
}
void storage_info_map_destroy(storage_info_map_t * map)
{
	saim_set_destroy(map->set);
}
void storage_info_map_clear(storage_info_map_t * map)
{
	saim_set_clear(map->set);
}
saim_set_node * storage_info_map_insert(storage_info_map_t * map, storage_info_pair_t * pair)
{
	return saim_set_insert(map->set, pair);
}
void storage_info_map_erase(storage_info_map_t * map, saim_set_node * node)
{
	saim_set_delete(map->set, node);
}
void storage_info_map_cut(storage_info_map_t * map, saim_set_node * node)
{
	saim_set_cut(map->set, node);
}
saim_set_node * storage_info_map_search(storage_info_map_t * map, const saim_string * name)
{
	saim_set_node * node;
	storage_info_pair_t pair;

	saim_string_create(&pair.name);
	saim_string_set(&pair.name, name);
	node = saim_set_search(map->set, &pair);
	saim_string_destroy(&pair.name);
	return node;
}
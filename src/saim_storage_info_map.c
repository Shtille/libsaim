#include "saim_storage_info_map.h"

#include "util/saim_memory.h"

static int compare_func(const void * p1, const void * p2)
{
	return saim_storage_info_pair__compare((const saim_storage_info_pair *)p1, (const saim_storage_info_pair *)p2);
}
static void destroy_func(void * p)
{
	saim_storage_info_pair * pair = (saim_storage_info_pair *)p;
	saim_storage_info_pair__destroy(pair);
	SAIM_FREE(pair);
}

void saim_storage_info_map__create(saim_storage_info_map * map)
{
	map->set = saim_set_create(compare_func, destroy_func);
}
void saim_storage_info_map__destroy(saim_storage_info_map * map)
{
	saim_set_destroy(map->set);
}
void saim_storage_info_map__clear(saim_storage_info_map * map)
{
	saim_set_clear(map->set);
}
saim_set_node * saim_storage_info_map__insert(saim_storage_info_map * map, saim_storage_info_pair * pair)
{
	return saim_set_insert(map->set, pair);
}
void saim_storage_info_map__erase(saim_storage_info_map * map, saim_set_node * node)
{
	saim_set_delete(map->set, node);
}
void saim_storage_info_map__cut(saim_storage_info_map * map, saim_set_node * node)
{
	saim_set_cut(map->set, node);
}
saim_set_node * saim_storage_info_map__search(saim_storage_info_map * map, const saim_string * name)
{
	saim_set_node * node;
	saim_storage_info_pair pair;

	saim_string_create(&pair.name);
	saim_string_set(&pair.name, name);
	node = saim_set_search(map->set, &pair);
	saim_string_destroy(&pair.name);
	return node;
}
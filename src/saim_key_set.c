#include "saim_key_set.h"

#include "util/saim_memory.h"

static int compare_func(const void * key1, const void * key2)
{
	return data_key_compare((const data_key_t *)key1, (const data_key_t *)key2);
}
static void destroy_func(void * victim)
{
	data_key_t * key = (data_key_t *)victim;
	data_key_destroy(key);
	SAIM_FREE(key);
}

void saim_key_set__create(saim_key_set * set)
{
	set->set = saim_set_create(compare_func, destroy_func);
}
void saim_key_set__destroy(saim_key_set * set)
{
	saim_set_destroy(set->set);
}
void saim_key_set__clear(saim_key_set * set)
{
	saim_set_clear(set->set);
}
void saim_key_set__insert(saim_key_set * set, const data_key_t * key)
{
	saim_set_node * node;
	node = saim_set_search(set->set, key);
	if (node == set->set->nil)
		(void)saim_set_insert(set->set, (void*)key);
}
void saim_key_set__erase(saim_key_set * set, saim_set_node * node)
{
	saim_set_delete(set->set, node);
}
saim_set_node * saim_key_set__search(saim_key_set * set, const data_key_t * key)
{
	return saim_set_search(set->set, key);
}
unsigned int saim_key_set__size(saim_key_set * set)
{
	return saim_set_size(set->set);
}
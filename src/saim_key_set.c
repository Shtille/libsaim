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

void key_set_create(key_set_t * set)
{
	set->set = saim_set_create(compare_func, destroy_func);
}
void key_set_destroy(key_set_t * set)
{
	saim_set_destroy(set->set);
}
void key_set_clear(key_set_t * set)
{
	saim_set_clear(set->set);
}
void key_set_insert(key_set_t * set, const data_key_t * key)
{
	saim_set_node * node;
	node = saim_set_search(set->set, key);
	if (node == set->set->nil)
		(void)saim_set_insert(set->set, (void *)key);
}
void key_set_erase(key_set_t * set, saim_set_node * node)
{
	saim_set_delete(set->set, node);
}
saim_set_node * key_set_search(key_set_t * set, const data_key_t * key)
{
	return saim_set_search(set->set, key);
}
unsigned int key_set_size(key_set_t * set)
{
	return saim_set_size(set->set);
}
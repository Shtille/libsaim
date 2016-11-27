#include "saim_key_list.h"

#include "util/saim_memory.h"

#include <stdlib.h>
#include <assert.h>

static void key_destroy_func(void* p)
{
	SAIM_FREE((key_pair_t *)p);
}
void key_list_create(key_list_t * list)
{
	saim_list_create(&list->list, key_destroy_func);
}
void key_list_destroy(key_list_t * list)
{
	saim_list_destroy(&list->list);
}
void key_list_clear(key_list_t * list)
{
	saim_list_clear(&list->list);
}
void key_list_insert(key_list_t * list, saim_set_node * node)
{
	saim_list_push_front(&list->list, node);
}
void key_list_delete(key_list_t * list, saim_list_node * node)
{
	/* We don't need to free data that has been popped from list,
	because it simply stores existing set node. */
	(void)saim_list_delete(&list->list, node);
}
static bool key_sort_func(const void * p1, const void * p2)
{
	const key_pair_t * pair1 = (const key_pair_t *)p1;
	const key_pair_t * pair2 = (const key_pair_t *)p2;
	return pair1->info.counter < pair2->info.counter;
}
void key_list_sort(key_list_t * list)
{
	saim_list_sort(&list->list, key_sort_func);
}
static bool size_fit_func(const void* s, const void* p)
{
	const file_size_t * size = (const file_size_t *)s;
	const key_pair_t * pair = (const key_pair_t *)p;
	return *size <= pair->info.size;
}
saim_list_node * key_list_find_size_compatible(key_list_t * list, file_size_t size)
{
	return saim_list_find(&list->list, &size, size_fit_func);
}
saim_list_node * key_list_get_first(key_list_t * list)
{
	return list->list.head;
}
#include "saim_bitmap_cache_info_list.h"

#include "../util/saim_memory.h"

static void destroy_function(void* p)
{
	saim_bitmap_cache_info * info = (saim_bitmap_cache_info *)p;
	// data key is a simple type, thus we don't need to destroy data
	SAIM_FREE(info);
}
static bool sort_function(const void* lhs, const void* rhs)
{
	const saim_bitmap_cache_info * info1 = (saim_bitmap_cache_info *) lhs;
	const saim_bitmap_cache_info * info2 = (saim_bitmap_cache_info *) rhs;
	return info1->usage < info2->usage;
}

void saim_bitmap_cache_info_list__create(saim_bitmap_cache_info_list * list)
{
	saim_list_create(&list->list, destroy_function);
}
void saim_bitmap_cache_info_list__destroy(saim_bitmap_cache_info_list * list)
{
	saim_list_destroy(&list->list);
}
void saim_bitmap_cache_info_list__clear(saim_bitmap_cache_info_list * list)
{
	saim_list_clear(&list->list);
}
void saim_bitmap_cache_info_list__sort(saim_bitmap_cache_info_list * list)
{
	saim_list_sort(&list->list, sort_function);
}
saim_bitmap_cache_info * saim_bitmap_cache_info_list__front(saim_bitmap_cache_info_list * list)
{
	return saim_list_front(&list->list);
}
saim_bitmap_cache_info * saim_bitmap_cache_info_list__pop_front(saim_bitmap_cache_info_list * list)
{
	return saim_list_pop_front(&list->list);
}
void saim_bitmap_cache_info_list__push_back(saim_bitmap_cache_info_list * list, saim_bitmap_cache_info * info)
{
	saim_list_push_back(&list->list, info);
}
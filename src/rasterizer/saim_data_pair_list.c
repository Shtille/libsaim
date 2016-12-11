#include "saim_data_pair_list.h"

#include "../util/saim_memory.h"

static void data_pair_destroy_func(void* p)
{
	saim_data_pair * pair = (saim_data_pair *) p;
	saim_data_pair__destroy(pair);
	SAIM_FREE(pair);
}
void saim_data_pair_list__create(saim_data_pair_list * list)
{
	saim_list_create(&list->list, data_pair_destroy_func);
}
void saim_data_pair_list__destroy(saim_data_pair_list * list)
{
	saim_list_destroy(&list->list);
}
void saim_data_pair_list__clear(saim_data_pair_list * list)
{
	saim_list_clear(&list->list);
}
void saim_data_pair_list__push_back(saim_data_pair_list * list, saim_data_pair * pair)
{
	saim_list_push_back(&list->list, pair);
}
saim_data_pair * saim_data_pair_list__pop_front(saim_data_pair_list * list)
{
	return (saim_data_pair *) saim_list_pop_front(&list->list);
}
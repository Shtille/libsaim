#ifndef __SAIM_DATA_PAIR_LIST_H__
#define __SAIM_DATA_PAIR_LIST_H__

#include "saim_data_pair.h"
#include "../util/saim_list.h"

typedef struct {
	saim_list list;
} saim_data_pair_list;

void saim_data_pair_list__create(saim_data_pair_list * list);
void saim_data_pair_list__destroy(saim_data_pair_list * list);

void saim_data_pair_list__clear(saim_data_pair_list * list);
void saim_data_pair_list__push_back(saim_data_pair_list * list, saim_data_pair * pair);
saim_data_pair * saim_data_pair_list__pop_front(saim_data_pair_list * list);

#endif
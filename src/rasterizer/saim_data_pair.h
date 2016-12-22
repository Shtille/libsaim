#ifndef __SAIM_DATA_PAIR_H__
#define __SAIM_DATA_PAIR_H__

#include "../saim_data_key.h"
#include "../util/saim_string.h"

typedef struct {
	saim_data_key key;
	saim_string data;
} saim_data_pair;

void saim_data_pair__create(saim_data_pair * data_pair);
void saim_data_pair__destroy(saim_data_pair * data_pair);

#endif
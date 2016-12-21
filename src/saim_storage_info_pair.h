#ifndef __SAIM_STORAGE_INFO_PAIR_H__
#define __SAIM_STORAGE_INFO_PAIR_H__

#include "saim_storage_info.h"
#include "util/saim_string.h"

typedef struct {
	saim_string name;
	saim_storage_info info;
} saim_storage_info_pair;

void saim_storage_info_pair__create(saim_storage_info_pair * pair);
void saim_storage_info_pair__destroy(saim_storage_info_pair * pair);

int saim_storage_info_pair__compare(const saim_storage_info_pair * a, const saim_storage_info_pair * b);

#endif
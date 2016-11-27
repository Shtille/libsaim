#ifndef __SAIM_STORAGE_INFO_PAIR_H__
#define __SAIM_STORAGE_INFO_PAIR_H__

#include "saim_storage_info.h"
#include "util/saim_string.h"

typedef struct {
	saim_string name;
	storage_info_t info;
} storage_info_pair_t;

void storage_info_pair_create(storage_info_pair_t * pair);
void storage_info_pair_destroy(storage_info_pair_t * pair);

int storage_info_pair_compare(const storage_info_pair_t * a, const storage_info_pair_t * b);

#endif
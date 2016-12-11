#ifndef __SAIM_BITMAP_INFO_PAIR_H__
#define __SAIM_BITMAP_INFO_PAIR_H__

#include "../saim_data_key.h"
#include "saim_bitmap_info.h"

typedef struct {
	data_key_t key;
	saim_bitmap_info info;
} saim_bitmap_info_pair;

int saim_bitmap_info_pair__compare(const saim_bitmap_info_pair* p1, const saim_bitmap_info_pair* p2);

#endif
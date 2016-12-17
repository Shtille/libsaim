#ifndef __SAIM_BITMAP_INFO_H__
#define __SAIM_BITMAP_INFO_H__

#include "saim_bitmap.h"

typedef struct {
	saim_bitmap bitmap;
	int* p_usage;
} saim_bitmap_info;

void saim_bitmap_info__create(saim_bitmap_info * bitmap_info);
void saim_bitmap_info__destroy(saim_bitmap_info * bitmap_info);

#endif
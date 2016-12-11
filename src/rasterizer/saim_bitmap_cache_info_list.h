#ifndef __SAIM_BITMAP_CACHE_INFO_LIST_H__
#define __SAIM_BITMAP_CACHE_INFO_LIST_H__

#include "saim_bitmap_cache_info.h"

#include "../util/saim_list.h"

typedef struct {
	saim_list list;
} saim_bitmap_cache_info_list;

void saim_bitmap_cache_info_list__create(saim_bitmap_cache_info_list * list);
void saim_bitmap_cache_info_list__destroy(saim_bitmap_cache_info_list * list);

void saim_bitmap_cache_info_list__clear(saim_bitmap_cache_info_list * list);
void saim_bitmap_cache_info_list__sort(saim_bitmap_cache_info_list * list);
saim_bitmap_cache_info * saim_bitmap_cache_info_list__front(saim_bitmap_cache_info_list * list);
saim_bitmap_cache_info * saim_bitmap_cache_info_list__pop_front(saim_bitmap_cache_info_list * list);
void saim_bitmap_cache_info_list__push_back(saim_bitmap_cache_info_list * list, saim_bitmap_cache_info * info);

#endif
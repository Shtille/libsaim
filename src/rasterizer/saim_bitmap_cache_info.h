#ifndef __SAIM_BITMAP_CACHE_INFO_H__
#define __SAIM_BITMAP_CACHE_INFO_H__

#include "../saim_data_key.h"

typedef struct {
	data_key_t key;
	int usage; //!< keep the last time of access to this key
} saim_bitmap_cache_info;

#endif
#ifndef __SAIM_BITMAP_BUFFER_H__
#define __SAIM_BITMAP_BUFFER_H__

#include "saim_bitmap.h"

#include <stdbool.h>

typedef struct {
	const saim_bitmap * bitmap;
	int level_ascending; // level - low_level, should be 0 or positive
} saim_buffered_bitmap_info;

//! Some struct to hold bitmap buffer data
typedef struct {
	saim_buffered_bitmap_info * data;
	int key_min_x;
	int key_min_y;
	int key_max_x;
	int key_max_y;
	int width;
	int level_of_detail; // base level of detail
	bool is_break;
	bool has_any_low_detailed_bitmap;
} saim_bitmap_buffer;

void saim_bitmap_buffer__create(saim_bitmap_buffer * buffer);
void saim_bitmap_buffer__destroy(saim_bitmap_buffer * buffer);

#endif
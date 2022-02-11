/*************************************************************************
* libsaim 1.0
* ------------------------------------------------------------------------
*  Copyright (c) 2022 Vladimir Sviridov <v.shtille@gmail.com>
* 
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
* 
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
* 
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
* 
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
* 
*  3. This notice may not be removed or altered from any source
*     distribution.
* 
**************************************************************************/

#ifndef __SAIM_RASTERIZER_ASYNC_H__
#define __SAIM_RASTERIZER_ASYNC_H__

#include "saim_bitmap_map.h"
#include "../saim_key_set.h"
#include "saim_bitmap_buffer.h"
#include "saim_data_pair_list.h"
#include "saim_bitmap_cache_info_list.h"
#include "saim_line_buffer.h"
#include "../util/saim_spin.h" // faster than mutex

#include "saim_target_info.h"

#include "../../deps/tinycthread.h"

#include <stdbool.h>

typedef struct saim_rasterizer_async saim_rasterizer_async;

struct saim_rasterizer_async {
	struct saim_instance * instance;    //!< pointer to library instance

	saim_spin request_mutex;			//!< mutex to protect requests
	saim_spin bitmap_mutex;				//!< mutex to protect bitmap map and cache
	saim_spin pending_data_mutex;		//!< mutex to protect pending data
	saim_data_pair_list pending_data;   //!< data that pending for conversion
	saim_key_set requested_keys;        //!< to not make another calls if already requested
	saim_bitmap_map bitmap_map;         //!< extracted bitmaps
	int render_counter;                 //!< counter for optimal cache sort
	int sort_shift_counter;             //!< counter to not do sort every frame
	saim_bitmap_cache_info_list bitmap_cache;
	unsigned int max_bitmap_cache_size; //!< maximum bitmap cache size
};

bool saim_rasterizer_async__create(saim_rasterizer_async * rasterizer, struct saim_instance * instance);
void saim_rasterizer_async__destroy(saim_rasterizer_async * rasterizer);

int saim_rasterizer_async__render_aligned(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude);
int saim_rasterizer_async__render_common(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude, float angle);
int saim_rasterizer_async__render_mapped_cube(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	int face, int lod, int x, int y);

// Some function for inner use

// TS
void saim_rasterizer_async__push_request(saim_rasterizer_async * rasterizer, const saim_data_key * key);

//! Clear all the data
void saim_rasterizer_async__clear(saim_rasterizer_async * rasterizer);

// TS
const saim_bitmap * saim_rasterizer_async__get_bitmap(saim_rasterizer_async * rasterizer, const saim_data_key * key, bool use_counter);

//! Whether key been requested
// TS
bool saim_rasterizer_async__is_requested(saim_rasterizer_async * rasterizer, const saim_data_key * key);

//! Whether key been loaded
// It is not being used anywhere. Done for test.
bool saim_rasterizer_async__is_loaded(saim_rasterizer_async * rasterizer, const saim_data_key * key);

// TS
void saim_rasterizer_async__add_request(saim_rasterizer_async * rasterizer, const saim_data_key * key);

//! Transform pending data to bitmap
void saim_rasterizer_async__data_transform(saim_rasterizer_async * rasterizer, saim_target_info * target_info);

void saim_rasterizer_async__pre_render(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int* num_tiles_to_load, int* optimal_lod);
void saim_rasterizer_async__pre_render_cube(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	int face, int lod, int x, int y,
	int* num_tiles_to_load, int* optimal_lod);

void saim_rasterizer_async__render_aligned_impl(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int level_of_detail);
void saim_rasterizer_async__render_common_impl(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int level_of_detail, float sin_angle, float cos_angle);
void saim_rasterizer_async__render_mapped_cube_impl(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	int face, double u_min, double v_min, double u_max, double v_max, int level_of_detail);

void saim_rasterizer_async__initialize_bitmap_buffer(saim_target_info * target_info,
	int key_min_x, int key_max_x, int key_min_y, int key_max_y, int level_of_detail, bool is_break);
void saim_rasterizer_async__set_bitmap_for_buffer(saim_target_info * target_info, 
	int key_x, int key_y, const saim_bitmap * bitmap);
void saim_rasterizer_async__set_bitmap_low_for_buffer(saim_target_info * target_info,
	int key_z_low, int key_x, int key_y, const saim_bitmap * bitmap);

const saim_buffered_bitmap_info * saim_rasterizer_async__get_bitmap_from_buffer(saim_target_info * target_info, int key_x, int key_y);
void saim_rasterizer_async__erase_key_from_buffer(saim_target_info * target_info, const saim_data_key * key);
void saim_rasterizer_async__sort_cache(saim_rasterizer_async * rasterizer);

#endif
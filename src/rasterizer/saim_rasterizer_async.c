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

#include "saim_rasterizer_async.h"

#include "saim_mercator.h"
#include "saim_line_buffer.h"
#include "saim_decoder_jpeg.h"
#include "saim_decoder_png.h"
#include "../saim_instance.h"
#include "../saim_provider.h"
#include "../saim_cache.h"

#include "../util/saim_memory.h"

#include <math.h>
#include <assert.h>

bool saim_rasterizer_async__create(saim_rasterizer_async * rasterizer, saim_instance * instance)
{
	const unsigned int kDefaultBitmapCapacity = 500;

	rasterizer->instance = instance;

	if (mtx_init(&rasterizer->request_mutex, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	if (mtx_init(&rasterizer->bitmap_mutex, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		mtx_destroy(&rasterizer->request_mutex);
		return false;
	}
	if (mtx_init(&rasterizer->pending_data_mutex, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		mtx_destroy(&rasterizer->bitmap_mutex);
		mtx_destroy(&rasterizer->request_mutex);
		return false;
	}
	saim_data_pair_list__create(&rasterizer->pending_data);
	saim_key_set__create(&rasterizer->requested_keys);
	saim_bitmap_map__create(&rasterizer->bitmap_map);
	saim_bitmap_cache_info_list__create(&rasterizer->bitmap_cache);

	rasterizer->render_counter = 0;
	rasterizer->sort_shift_counter = 0;
	rasterizer->max_bitmap_cache_size = kDefaultBitmapCapacity;

	return true;
}
void saim_rasterizer_async__destroy(saim_rasterizer_async * rasterizer)
{
	saim_rasterizer_async__clear(rasterizer);
	saim_bitmap_cache_info_list__destroy(&rasterizer->bitmap_cache);
	saim_bitmap_map__destroy(&rasterizer->bitmap_map);
	saim_key_set__destroy(&rasterizer->requested_keys);
	saim_data_pair_list__destroy(&rasterizer->pending_data);
	mtx_destroy(&rasterizer->pending_data_mutex);
	mtx_destroy(&rasterizer->bitmap_mutex);
	mtx_destroy(&rasterizer->request_mutex);
}
int saim_rasterizer_async__render_aligned(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude)
{
	int num_tiles_to_load, optimal_lod;
	if (target_info->target_buffer == 0)
	{
		fprintf(stderr, "saim: target buffer hasn't been set\n");
		return -1;
	}
	saim_rasterizer_async__pre_render(rasterizer, target_info,
		upper_latitude, left_longitude, lower_latitude, right_longitude,
		&num_tiles_to_load, &optimal_lod);
	saim_rasterizer_async__render_aligned_impl(rasterizer, target_info,
		upper_latitude, left_longitude, lower_latitude, right_longitude, optimal_lod);
	return num_tiles_to_load;
}
int saim_rasterizer_async__render_common(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude, float angle)
{
	int num_tiles_to_load, optimal_lod;
	float sin_angle, cos_angle;
	const float kDegToRad = 0.017453292f;
	const float angle_rad = angle * kDegToRad;
	if (target_info->target_buffer == 0)
	{
		fprintf(stderr, "saim: target buffer hasn't been set\n");
		return -1;
	}
	sin_angle = sinf(angle_rad);
	cos_angle = cosf(angle_rad);
	saim_rasterizer_async__pre_render(rasterizer, target_info,
		upper_latitude, left_longitude, lower_latitude, right_longitude,
		&num_tiles_to_load, &optimal_lod);
	saim_rasterizer_async__render_common_impl(rasterizer, target_info,
		upper_latitude, left_longitude, lower_latitude, right_longitude, optimal_lod, sin_angle, cos_angle);
	return num_tiles_to_load;
}
int saim_rasterizer_async__render_mapped_cube(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	int face, int lod, int x, int y)
{
	int num_tiles_to_load, optimal_lod;
	if (target_info->target_buffer == 0)
	{
		fprintf(stderr, "saim: target buffer hasn't been set\n");
		return -1;
	}
	// Request tiles to load and process loaded ones
	saim_rasterizer_async__pre_render_cube(rasterizer, target_info,
		face, lod, x, y,
		&num_tiles_to_load, &optimal_lod);

	// Render finally
	const double inv_scale = 2. / (double)(1 << lod);
	const double position_x = -1. + inv_scale * x;
	const double position_y = -1. + inv_scale * y;

	double u_min = position_x;
	double v_min = position_y;
	double u_max = position_x + inv_scale;
	double v_max = position_y + inv_scale;
	saim_rasterizer_async__render_mapped_cube_impl(rasterizer, target_info,
		face, u_min, v_min, u_max, v_max, optimal_lod);
	return num_tiles_to_load;
}

// -------------------- Internal use only functions -------------------------

static void tile_notification_function(saim_instance * instance, const saim_data_key * key, saim_string * data, bool success)
{
	saim_data_pair * pair;

	pair = (saim_data_pair *) SAIM_MALLOC(sizeof(saim_data_pair));
	saim_data_pair__create(pair);
	
	mtx_lock(&instance->rasterizer_async->pending_data_mutex);
	pair->key = *key; // copy key value
	if (success)
	{
		saim_string_swap(&pair->data, data);
	}
	saim_data_pair_list__push_back(&instance->rasterizer_async->pending_data, pair);
	mtx_unlock(&instance->rasterizer_async->pending_data_mutex);
}

void saim_rasterizer_async__push_request(saim_rasterizer_async * rasterizer, const saim_data_key * key)
{
	mtx_lock(&rasterizer->request_mutex);
	// Whether key requested to load or loaded
    if (!saim_rasterizer_async__is_requested(rasterizer, key))
    {
        // Add key to requested list
        saim_rasterizer_async__add_request(rasterizer, key);

        // Perform query for each key that hasn't been loaded
        saim_cache__tile_service_load_query(rasterizer->instance->cache, key, tile_notification_function);
    }
    mtx_unlock(&rasterizer->request_mutex);
}
void saim_rasterizer_async__clear(saim_rasterizer_async * rasterizer)
{
	mtx_lock(&rasterizer->pending_data_mutex);
	saim_data_pair_list__clear(&rasterizer->pending_data);
	mtx_unlock(&rasterizer->pending_data_mutex);

	mtx_lock(&rasterizer->request_mutex);
	saim_key_set__clear(&rasterizer->requested_keys);
	mtx_unlock(&rasterizer->request_mutex);

	mtx_lock(&rasterizer->bitmap_mutex);
	saim_bitmap_map__clear(&rasterizer->bitmap_map);
	saim_bitmap_cache_info_list__clear(&rasterizer->bitmap_cache);
	mtx_unlock(&rasterizer->bitmap_mutex);
}
const saim_bitmap * saim_rasterizer_async__get_bitmap(saim_rasterizer_async * rasterizer, const saim_data_key * key, bool use_counter)
{
	const saim_bitmap * bitmap = NULL;
	saim_set_node * node;
	saim_bitmap_info_pair * pair;

	mtx_lock(&rasterizer->bitmap_mutex);
	node = saim_bitmap_map__search(&rasterizer->bitmap_map, key);
	if (node != rasterizer->bitmap_map.set->nil)
	{
		pair = (saim_bitmap_info_pair *) node->data;
		if (use_counter)
			*pair->info.p_usage = rasterizer->render_counter; // use render counter as bitmap existence time
		bitmap = &(pair->info.bitmap);
	}
	mtx_unlock(&rasterizer->bitmap_mutex);
	return bitmap;
}
bool saim_rasterizer_async__is_requested(saim_rasterizer_async * rasterizer, const saim_data_key * key)
{
	saim_set_node * node;
	node = saim_key_set__search(&rasterizer->requested_keys, key);
	return node != rasterizer->requested_keys.set->nil;
}
bool saim_rasterizer_async__is_loaded(saim_rasterizer_async * rasterizer, const saim_data_key * key)
{
	saim_set_node * node;
	bool loaded;

	mtx_lock(&rasterizer->bitmap_mutex);
	node = saim_bitmap_map__search(&rasterizer->bitmap_map, key);
	loaded = node != rasterizer->bitmap_map.set->nil;
	mtx_unlock(&rasterizer->bitmap_mutex);

	return loaded;
}
void saim_rasterizer_async__add_request(saim_rasterizer_async * rasterizer, const saim_data_key * key)
{
	saim_data_key * key_copy;
	key_copy = (saim_data_key *)SAIM_MALLOC(sizeof(saim_data_key));
	saim_data_key__set_by_other(key_copy, key); // copy
	saim_key_set__insert(&rasterizer->requested_keys, key_copy);
}
static void empty_image(saim_rasterizer_async * rasterizer, saim_target_info * target_info, saim_bitmap * bitmap)
{
	// Make data the same bitness as destination bitmap
	const int kBitmapWidth = rasterizer->instance->provider->bitmap_width;
	const int kBitmapHeight = rasterizer->instance->provider->bitmap_height;
	const int screen_bpp = target_info->target_bpp;
	const int bitmap_size = kBitmapWidth * kBitmapHeight * screen_bpp;
	unsigned char * data;

	saim_bitmap__destroy(bitmap);
	saim_bitmap__create(bitmap);
	bitmap->data = (unsigned char*) SAIM_MALLOC(bitmap_size);
	data = bitmap->data;
	switch (screen_bpp)
	{
	case 1:
		for (int i = 0; i < bitmap_size; i += screen_bpp)
		{
			*data++ = 0xFF;
		}
		break;
	case 2:
		for (int i = 0; i < bitmap_size; i += screen_bpp)
		{
			// TODO: RGB(1,1,1) to R5G6B5
			*data++ = 0xFF;
			*data++ = 0xFF;
		}
		break;
	case 3:
		for (int i = 0; i < bitmap_size; i += screen_bpp)
		{
			*data++ = 0xFF;
			*data++ = 0xFF;
			*data++ = 0xFF;
		}
		break;
	case 4:
		for (int i = 0; i < bitmap_size; i += screen_bpp)
		{
			*data++ = 0xFF;
			*data++ = 0xFF;
			*data++ = 0xFF;
			*data++ = 0xFF;
		}
		break;
	default:
		assert(false && "wrong number of bytes per pixel");
	}
}
static bool decode_png(saim_rasterizer_async * rasterizer, saim_target_info * target_info, 
	const saim_string * data, saim_bitmap * bitmap)
{
	// Width, height and bpp parameters are known
	int width, height, bpp;
	saim_decoder_png__load_from_buffer(data, false, &width, &height, &bpp, bitmap);
	assert(width == rasterizer->instance->provider->bitmap_width);
	assert(height == rasterizer->instance->provider->bitmap_height);
	assert(bpp == target_info->target_bpp);
	return true;
}
static bool decode_jpg(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	const saim_string * data, saim_bitmap * bitmap)
{
	// Width, height and bpp parameters are known
	int width, height, bpp;
	saim_decoder_jpeg__load_from_buffer(data, false, &width, &height, &bpp, bitmap);
	assert(width == rasterizer->instance->provider->bitmap_width);
	assert(height == rasterizer->instance->provider->bitmap_height);
	assert(bpp == target_info->target_bpp);
	return true;
}
static void decode_image(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	const saim_string * data, saim_bitmap * bitmap)
{
	// If we try to decode wrong type of image, our app will crash
	// So we have to recognize the image type
	if (data->length < 3) // number of bytes to decode 'PNG' tag
	{
		empty_image(rasterizer, target_info, bitmap);
		return;
	}
	if (data->data[1] == 'P' &&
		data->data[2] == 'N' &&
		data->data[3] == 'G')
	{
		// Format is PNG
		if (!decode_png(rasterizer, target_info, data, bitmap))
		{
			fprintf(stderr, "saim: PNG image decode error\n");
			empty_image(rasterizer, target_info, bitmap);
		}
	}
	else
	{
		// Format is JPG
		if (!decode_jpg(rasterizer, target_info, data, bitmap))
		{
			fprintf(stderr, "saim: JPG image decode error\n");
			empty_image(rasterizer, target_info, bitmap);
		}
	}
}
void saim_rasterizer_async__data_transform(saim_rasterizer_async * rasterizer, saim_target_info * target_info)
{
	saim_data_pair * pair;
	const saim_data_key * key;
	saim_set_node * node;
	saim_bitmap_cache_info * info;
	saim_bitmap_info_pair * info_pair;
	saim_bitmap * bitmap;

	mtx_lock(&rasterizer->bitmap_mutex);
	++rasterizer->render_counter;

	// Perform cache sort once per 10 frames
	++rasterizer->sort_shift_counter;
	if (rasterizer->sort_shift_counter == 10)
	{
		rasterizer->sort_shift_counter = 0;
		saim_rasterizer_async__sort_cache(rasterizer);
	}
	mtx_unlock(&rasterizer->bitmap_mutex);

	for (;;)
	{
		mtx_lock(&rasterizer->pending_data_mutex);
		pair = saim_data_pair_list__pop_front(&rasterizer->pending_data);
		mtx_unlock(&rasterizer->pending_data_mutex);
		if (!pair)
			break; // there is no data to transform

		key = &pair->key;

		// Check for skipped data
		if (pair->data.length != 0)
		{
			mtx_lock(&rasterizer->bitmap_mutex);
			if (saim_bitmap_map__size(&rasterizer->bitmap_map) == rasterizer->max_bitmap_cache_size)
			{
				// Our cache is full, so we gonna replace existing bitmaps
				info = saim_bitmap_cache_info_list__pop_front(&rasterizer->bitmap_cache);
				saim_rasterizer_async__erase_key_from_buffer(target_info, &info->key);
				node = saim_bitmap_map__search(&rasterizer->bitmap_map, &info->key);
				saim_bitmap_map__erase(&rasterizer->bitmap_map, node);
				SAIM_FREE(info);
            }

			// Insert a bitmap into our map
            info = (saim_bitmap_cache_info *) SAIM_MALLOC(sizeof(saim_bitmap_cache_info));
            // saim_bitmap_cache_info is a simple struct
            info->key = *key;
            info->usage = rasterizer->render_counter;
            saim_bitmap_cache_info_list__push_back(&rasterizer->bitmap_cache, info);
            info_pair = (saim_bitmap_info_pair *) SAIM_MALLOC(sizeof(saim_bitmap_info_pair));
            saim_bitmap_info_pair__create(info_pair);
            info_pair->key = *key;
            node = saim_bitmap_map__insert(&rasterizer->bitmap_map, info_pair);
            info_pair->info.p_usage = & info->usage;
            bitmap = & info_pair->info.bitmap;
            mtx_unlock(&rasterizer->bitmap_mutex);

			// Pair stores encoded image data, so we have to decode it before use
			decode_image(rasterizer, target_info, &pair->data, bitmap);
		}

		// Data is loaded, so no we don't need this key in requested list
		mtx_lock(&rasterizer->request_mutex);
		node = saim_key_set__search(&rasterizer->requested_keys, key);
		saim_key_set__erase(&rasterizer->requested_keys, node);
		mtx_unlock(&rasterizer->request_mutex);

		// Finally
		saim_data_pair__destroy(pair);
		SAIM_FREE(pair);
	}
}
void saim_rasterizer_async__pre_render(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int* num_tiles_to_load, int* optimal_lod)
{
	int left, right, top, bottom;
	saim_data_key key, key_low;
	const saim_bitmap * bitmap, * bitmap_low;

	double screen_pixel_size_x = (right_longitude - left_longitude)/(double)target_info->target_width;
    *optimal_lod = saim_get_optimal_level_of_detail(rasterizer->instance->provider, screen_pixel_size_x);

    // Compute tile keys for bound rect
    saim_lat_long_to_tile_xy(rasterizer->instance->provider, upper_latitude, saim_normalized_longitude(left_longitude),
		*optimal_lod, &left, &top);
    saim_lat_long_to_tile_xy(rasterizer->instance->provider, lower_latitude, saim_normalized_longitude(right_longitude),
		*optimal_lod, &right, &bottom);

    int tiles_per_side = 1 << *optimal_lod;

    // Calculate number of tiles (bitmaps) in longitude (x) direction
    int num_x = right - left + 1;
    bool has_break = (num_x <= 0);
    if (has_break)
        num_x += tiles_per_side;

    saim_rasterizer_async__initialize_bitmap_buffer(target_info, left, right, top, bottom, *optimal_lod, has_break);

    // Transform rect to list of keys
    *num_tiles_to_load = 0;

    for (int y = top; y <= bottom; ++y)
    for (int i = 0; i < num_x; ++i)
    {
        int x = (left + i) % tiles_per_side;
        // Add a key to the list
        saim_data_key__create(&key, x, y, *optimal_lod);
        bitmap = saim_rasterizer_async__get_bitmap(rasterizer, &key, true);

        if (bitmap == NULL) // not loaded yet
        {
            ++*num_tiles_to_load;

            saim_rasterizer_async__push_request(rasterizer, &key);

            // Try to use low detailed version of bitmap when it didn't load
            bool bitmap_found = false;
            bool lazy_bitmap_found = false;
            for (int i = 1; i <= *optimal_lod - rasterizer->instance->provider->min_lod; ++i)
            {
                int lod = *optimal_lod - i;
                int key_x_low = x >> i;
                int key_y_low = y >> i;
                saim_data_key__create(&key_low, key_x_low, key_y_low, lod);
                bitmap_low = saim_rasterizer_async__get_bitmap(rasterizer, &key_low, false);
                if (bitmap_low)
                {
                    bitmap_found = true;
                    saim_rasterizer_async__set_bitmap_low_for_buffer(target_info, lod, x, y, bitmap_low);
                    break;
                }
                else
                {
                    // Poor internet fix to push low detailed cached bitmaps to load
                    if (!lazy_bitmap_found && saim_cache__is_exist(rasterizer->instance->cache, &key_low))
                    {
                        lazy_bitmap_found = true;
                        saim_rasterizer_async__push_request(rasterizer, &key_low);
                    }
                }
            }
            
            // Otherwise nothing to render
            if (!bitmap_found)
                saim_rasterizer_async__set_bitmap_for_buffer(target_info, x, y, NULL);
        }
        else
        {
            saim_rasterizer_async__set_bitmap_for_buffer(target_info, x, y, bitmap);
        }
    }

    // Transform any pending data
    saim_rasterizer_async__data_transform(rasterizer, target_info);
}
void saim_rasterizer_async__pre_render_cube(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	int face, int lod, int x, int y,
	int* num_tiles_to_load, int* optimal_lod)
{
	int left, right, top, bottom;
	saim_data_key key, key_low;
	const saim_bitmap * bitmap, *bitmap_low;

	double two_power_lod = (double)(1 << lod);
	double screen_pixel_size_x = (90.0 / two_power_lod) / (double)target_info->target_width;
	*optimal_lod = saim_get_optimal_level_of_detail(rasterizer->instance->provider, screen_pixel_size_x);

	int tiles_per_side = 1 << *optimal_lod;

	// Calculate bounding box via two different methods
	if (lod == 0) // bad case
	{
		left = top = 0;
		right = bottom = tiles_per_side - 1;
	}
	else // lod != 0, normal case
	{
		const double inv_scale = 2. / two_power_lod;
		const double position_x = -1. + inv_scale * x;
		const double position_y = -1. + inv_scale * y;

		double u_min = position_x;
		double v_min = position_y;
		double u_max = position_x + inv_scale;
		double v_max = position_y + inv_scale;

		// Calculate bounding box
		double points[4][2] = {
			// Angular points
			{ u_min, v_min },
			{ u_max, v_min },
			{ u_max, v_max },
			{ u_min, v_max }
		};
		double latitude, longitude;
		double max_latitude, min_longitude, min_latitude, max_longitude;
		min_latitude = 100.0;
		min_longitude = 200.0;
		max_latitude = -100.0;
		max_longitude = -200.0;
		for (int i = 0; i < 4; ++i)
		{
			saim_cube_point_to_lat_lon(face, points[i][0], points[i][1], &latitude, &longitude);
			const double kEpsilon = 0.000001;
			const bool is_pole = (latitude > 90.0 - kEpsilon) || (latitude < -90.0 + kEpsilon); // latitude == 90.0 or -90.0
			const bool is_jump = (longitude > 180.0 - kEpsilon) || (longitude < -180.0 + kEpsilon); // longitude == 180.0 or -180.0
			if (is_jump)
			{
				// Correct sign of longitude to the center of the tile
				double center_latitude, center_longitude;
				saim_cube_point_to_lat_lon(face, 0.5 * (u_min + u_max), 0.5 * (v_min + v_max), &center_latitude, &center_longitude);
				longitude = (center_longitude > 0.0) ? 180.0 : -180.0;
			}
			if (latitude < min_latitude)
				min_latitude = latitude;
			if (longitude < min_longitude && !is_pole)
				min_longitude = longitude;
			if (latitude > max_latitude)
				max_latitude = latitude;
			if (longitude > max_longitude && !is_pole)
				max_longitude = longitude;
		}

		// Compute tile keys for bound rect
		saim_lat_long_to_tile_xy(rasterizer->instance->provider, max_latitude, saim_normalized_longitude(min_longitude),
			*optimal_lod, &left, &top);
		saim_lat_long_to_tile_xy(rasterizer->instance->provider, min_latitude, saim_normalized_longitude(max_longitude),
			*optimal_lod, &right, &bottom);
	}

	// Calculate number of tiles (bitmaps) in longitude (x) direction
	int num_x = right - left + 1;
	bool has_break = (num_x <= 0);
	if (has_break)
		num_x += tiles_per_side;

	saim_rasterizer_async__initialize_bitmap_buffer(target_info, left, right, top, bottom, *optimal_lod, has_break);

	// Transform rect to list of keys
	*num_tiles_to_load = 0;

	for (int y = top; y <= bottom; ++y)
	{
		for (int i = 0; i < num_x; ++i)
		{
			int x = (left + i) % tiles_per_side;
			// Add a key to the list
			saim_data_key__create(&key, x, y, *optimal_lod);
			bitmap = saim_rasterizer_async__get_bitmap(rasterizer, &key, true);

			if (bitmap == NULL) // not loaded yet
			{
				++*num_tiles_to_load;

				saim_rasterizer_async__push_request(rasterizer, &key);

				// Try to use low detailed version of bitmap when it didn't load
				bool bitmap_found = false;
				bool lazy_bitmap_found = false;
				for (int i = 1; i <= *optimal_lod - rasterizer->instance->provider->min_lod; ++i)
				{
					int lod = *optimal_lod - i;
					int key_x_low = x >> i;
					int key_y_low = y >> i;
					saim_data_key__create(&key_low, key_x_low, key_y_low, lod);
					bitmap_low = saim_rasterizer_async__get_bitmap(rasterizer, &key_low, false);
					if (bitmap_low)
					{
						bitmap_found = true;
						saim_rasterizer_async__set_bitmap_low_for_buffer(target_info, lod, x, y, bitmap_low);
						break;
					}
					else
					{
						// Poor internet fix to push low detailed cached bitmaps to load
						if (!lazy_bitmap_found && saim_cache__is_exist(rasterizer->instance->cache, &key_low))
						{
							lazy_bitmap_found = true;
							saim_rasterizer_async__push_request(rasterizer, &key_low);
						}
					}
				}

				// Otherwise nothing to render
				if (!bitmap_found)
					saim_rasterizer_async__set_bitmap_for_buffer(target_info, x, y, NULL);
			}
			else
			{
				saim_rasterizer_async__set_bitmap_for_buffer(target_info, x, y, bitmap);
			}
		} // for i
	} // for y

	// Transform any pending data
	saim_rasterizer_async__data_transform(rasterizer, target_info);
}
void saim_rasterizer_async__render_aligned_impl(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude,
	double left_longitude,
	double lower_latitude,
	double right_longitude,
	int level_of_detail)
{
	// Bitmap should have the same BPP as the target buffer
	unsigned char * dst = target_info->target_buffer;

	const int screen_width = target_info->target_width;
	const int screen_height = target_info->target_height;
	const int screen_bpp = target_info->target_bpp;
	const int kBitmapWidth = rasterizer->instance->provider->bitmap_width;
	const int kBitmapHeight = rasterizer->instance->provider->bitmap_height;

	int map_size = kBitmapWidth << level_of_detail;

	// Compute pixel sizes
	double screen_pixel_size_x = (right_longitude - left_longitude)/(double)screen_width;
	double screen_pixel_size_y = (upper_latitude - lower_latitude)/(double)screen_height;

	// Doubled size of maximum dimension done due to rotation perspective
	const int total_buffer_size = (screen_width > screen_height) ? (screen_width << 1) : (screen_height << 1);

	// Resize buffers
	saim_line_buffer * line_buffer = (saim_line_buffer *) target_info->line_buffer;
	saim_line_buffer__reallocate(line_buffer, total_buffer_size);

	// Copy pointers to variables for faster access
	int * x_keys    = line_buffer->x_keys;
	int * x_samples = line_buffer->x_samples;
	int * x_pixels  = line_buffer->x_pixels;
	int * y_keys    = line_buffer->y_keys;
	int * y_samples = line_buffer->y_samples;
	int * y_pixels  = line_buffer->y_pixels;

	// Fill buffers
	for (int x = 0; x < screen_width; ++x)
	{
		double pixel_lon = saim_normalized_longitude(left_longitude + screen_pixel_size_x * ((double)x + 0.5));
		int bitmap_x;
		saim_longitude_to_pixel_x(rasterizer->instance->provider, pixel_lon, map_size, &bitmap_x);
		x_keys[x]    = bitmap_x / kBitmapWidth;
		x_samples[x] = bitmap_x % kBitmapWidth;
		x_pixels[x]  = bitmap_x;
	}
	for (int y = 0; y < screen_height; ++y)
	{
		double pixel_lat = upper_latitude - screen_pixel_size_y * ((double)y + 0.5);
		int bitmap_y;
		saim_latitude_to_pixel_y(rasterizer->instance->provider, pixel_lat, map_size, &bitmap_y);
		y_keys[y]    = bitmap_y / kBitmapHeight;
		y_samples[y] = bitmap_y % kBitmapHeight;
		y_pixels[y]  = bitmap_y;
	}

	int dst_index = 0;
	for (int y = 0; y < screen_height; ++y)
	{
		int key_y = y_keys[y];
		int sample_y_dot_width = y_samples[y] * kBitmapWidth;
		for (int x = 0; x < screen_width; ++x)
		{
			int key_x = x_keys[x];

			const saim_buffered_bitmap_info * info;
			info = saim_rasterizer_async__get_bitmap_from_buffer(target_info, key_x, key_y);
			if (info->bitmap == NULL)
			{
				// Tile hasn't been loaded yet
				dst_index += screen_bpp;
				continue;
			}

			unsigned char * src = info->bitmap->data;
			assert(src);

			int src_index;
			if (info->level_ascending == 0)
			{
				int sample_x = x_samples[x];
				src_index = sample_y_dot_width + sample_x;
			}
			else
			{
				int bitmap_x = x_pixels[x];
				int bitmap_y = y_pixels[y];
				int bitmap_x_low = bitmap_x >> info->level_ascending;
				int sample_x_low = bitmap_x_low % kBitmapWidth;
				int bitmap_y_low = bitmap_y >> info->level_ascending;
				int sample_y_low = bitmap_y_low % kBitmapHeight;
				src_index = sample_y_low * kBitmapWidth + sample_x_low;
			}
			src_index *= screen_bpp;

			// Formula for pixel offset is pixel[(y * width + x) * bpp]
			memcpy(dst + dst_index, src + src_index, screen_bpp);

			dst_index += screen_bpp;
		} // for x
	} // for y
}
void saim_rasterizer_async__render_common_impl(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	double upper_latitude, double left_longitude, double lower_latitude, double right_longitude,
	int level_of_detail, float sin_angle, float cos_angle)
{
	unsigned char * dst = target_info->target_buffer;

	const int screen_width = target_info->target_width;
	const int screen_height = target_info->target_height;
	const int screen_bpp = target_info->target_bpp;
	const int kBitmapWidth = rasterizer->instance->provider->bitmap_width;
	const int kBitmapHeight = rasterizer->instance->provider->bitmap_height;

	int map_size = kBitmapWidth << level_of_detail;

	float frotated_screen_width = fabsf((float)screen_width * cos_angle) +
								  fabsf((float)screen_height * sin_angle);
	float frotated_screen_height = fabsf((float)screen_width * sin_angle) +
								   fabsf((float)screen_height * cos_angle);
	int rotated_screen_width = (int)(frotated_screen_width);
	int rotated_screen_height = (int)(frotated_screen_height);

	// Compute pixel sizes
	double screen_pixel_size_x = (right_longitude - left_longitude)/frotated_screen_width;
	double screen_pixel_size_y = (upper_latitude - lower_latitude)/frotated_screen_height;

	// We gonna rotate our screen, so maximum size will be sqrt(w^2 + h^2)
	// But we gonna allocate max(w,h)*2 that is more than enough
	const int total_buffer_size = (screen_width > screen_height) ? (screen_width << 1) : (screen_height << 1);

	// Resize buffers
	saim_line_buffer * line_buffer = (saim_line_buffer *) target_info->line_buffer;
	saim_line_buffer__reallocate(line_buffer, total_buffer_size);

	// Copy pointers to variables for faster access
	int * x_keys    = line_buffer->x_keys;
	int * x_samples = line_buffer->x_samples;
	int * x_pixels  = line_buffer->x_pixels;
	int * y_keys    = line_buffer->y_keys;
	int * y_samples = line_buffer->y_samples;
	int * y_pixels  = line_buffer->y_pixels;

	// Fill buffers
	for (int x = 0; x < rotated_screen_width; ++x)
	{
		double pixel_lon = saim_normalized_longitude(left_longitude + screen_pixel_size_x * ((double)x + 0.5));
		int bitmap_x;
		saim_longitude_to_pixel_x(rasterizer->instance->provider, pixel_lon, map_size, &bitmap_x);
		x_keys[x]    = bitmap_x / kBitmapWidth;
		x_samples[x] = bitmap_x % kBitmapWidth;
		x_pixels[x]  = bitmap_x;
	}
	for (int y = 0; y < rotated_screen_height; ++y)
	{
		double pixel_lat = upper_latitude - screen_pixel_size_y * ((double)y + 0.5);
		int bitmap_y;
		saim_latitude_to_pixel_y(rasterizer->instance->provider, pixel_lat, map_size, &bitmap_y);
		y_keys[y]    = bitmap_y / kBitmapHeight;
		y_samples[y] = bitmap_y % kBitmapHeight;
		y_pixels[y]  = bitmap_y;
	}

	float x0 = (frotated_screen_width/2.0f)
		- (float)(screen_width/2)  * cos_angle
		+ (float)(screen_height/2) * sin_angle;
	float y0 = (frotated_screen_height/2.0f)
		- (float)(screen_width/2)  * sin_angle
		- (float)(screen_height/2) * cos_angle;

	int dst_index = 0;
	float xtr, ytr;
	for (int y = 0; y < screen_height; ++y)
	{
		float yinv = (float)(y);
		xtr = x0 - yinv * sin_angle;
		ytr = y0 + yinv * cos_angle;
		for (int x = 0; x < screen_width; ++x)
		{
			//float xtr = x0 + xinv * cos_angle - yinv * sin_angle;
			//float ytr = y0 + xinv * sin_angle + yinv * cos_angle;
			int x_mapped = (int)(xtr);
			int y_mapped = (int)(ytr);
			xtr += cos_angle;
			ytr += sin_angle;
			// Clamp values to not cause exceed buffer size
			if (x_mapped < 0) x_mapped = 0;
			if (y_mapped < 0) y_mapped = 0;
			if (x_mapped >= rotated_screen_width) x_mapped = rotated_screen_width-1;
			if (y_mapped >= rotated_screen_height) y_mapped = rotated_screen_height-1;

			int key_x = x_keys[x_mapped];
			int key_y = y_keys[y_mapped];

			const saim_buffered_bitmap_info * info;
			info = saim_rasterizer_async__get_bitmap_from_buffer(target_info, key_x, key_y);
			if (info->bitmap == NULL)
			{
				// Tile hasn't been loaded yet
				dst_index += screen_bpp;
				continue;
			}

			unsigned char * src = info->bitmap->data;
			assert(src);

			int src_index;
			if (info->level_ascending == 0)
			{
				int sample_x = x_samples[x_mapped];
				int sample_y = y_samples[y_mapped];
				src_index = sample_y * kBitmapWidth + sample_x;
			}
			else
			{
				int bitmap_x = x_pixels[x_mapped];
				int bitmap_y = y_pixels[y_mapped];
				int bitmap_x_low = bitmap_x >> info->level_ascending;
				int sample_x_low = bitmap_x_low % kBitmapWidth;
				int bitmap_y_low = bitmap_y >> info->level_ascending;
				int sample_y_low = bitmap_y_low % kBitmapHeight;
				src_index = sample_y_low * kBitmapWidth + sample_x_low;
			}
			src_index *= screen_bpp;

			// Formula for pixel offset is pixel[(y * width + x) * bpp]
			memcpy(dst + dst_index, src + src_index, screen_bpp);

			dst_index += screen_bpp;
		} // for x
	} // for y
}
void saim_rasterizer_async__render_mapped_cube_impl(saim_rasterizer_async * rasterizer, saim_target_info * target_info,
	int face, double u_min, double v_min, double u_max, double v_max, int level_of_detail)
{
	// Bitmap should have the same BPP as the target buffer
	unsigned char * dst = target_info->target_buffer;

	const int screen_width = target_info->target_width;
	const int screen_height = target_info->target_height;
	const int screen_bpp = target_info->target_bpp;
	const int kBitmapWidth = rasterizer->instance->provider->bitmap_width;
	const int kBitmapHeight = rasterizer->instance->provider->bitmap_height;

	int map_size = kBitmapWidth << level_of_detail;

	// Compute pixel sizes
	double screen_pixel_size_u = (u_max - u_min) / (double)screen_width;
	double screen_pixel_size_v = (v_max - v_min) / (double)screen_height;

	// Nothing to bufferize. Pretty sad.
	int dst_index = 0;
	for (int y = 0; y < screen_height; ++y)
	{
		double v = v_min + ((double)y + 0.5) * screen_pixel_size_v;
		for (int x = 0; x < screen_width; ++x)
		{
			double u = u_min + ((double)x + 0.5) * screen_pixel_size_u;
			double latitude, longitude;
			saim_cube_point_to_lat_lon(face, u, v, &latitude, &longitude);
			double pixel_lon = saim_normalized_longitude(longitude);
			double pixel_lat = latitude;
			int bitmap_x, bitmap_y;
			saim_longitude_to_pixel_x(rasterizer->instance->provider, pixel_lon, map_size, &bitmap_x);
			saim_latitude_to_pixel_y(rasterizer->instance->provider, pixel_lat, map_size, &bitmap_y);
			int key_x = bitmap_x / kBitmapWidth;
			int key_y = bitmap_y / kBitmapHeight;

			const saim_buffered_bitmap_info * info;
			info = saim_rasterizer_async__get_bitmap_from_buffer(target_info, key_x, key_y);
			assert(info);
			if (info->bitmap == NULL)
			{
				// Tile hasn't been loaded yet
				dst_index += screen_bpp;
				continue;
			}

			int sample_x = bitmap_x % kBitmapWidth;
			int sample_y = bitmap_y % kBitmapHeight;

			unsigned char * src = info->bitmap->data;
			assert(src);

			int src_index;
			if (info->level_ascending == 0)
			{
				src_index = sample_y * kBitmapWidth + sample_x;
			}
			else
			{
				int bitmap_x_low = bitmap_x >> info->level_ascending;
				int sample_x_low = bitmap_x_low % kBitmapWidth;
				int bitmap_y_low = bitmap_y >> info->level_ascending;
				int sample_y_low = bitmap_y_low % kBitmapHeight;
				src_index = sample_y_low * kBitmapWidth + sample_x_low;
			}
			src_index *= screen_bpp;

			// Formula for pixel offset is pixel[(y * width + x) * bpp]
			memcpy(dst + dst_index, src + src_index, screen_bpp);

			dst_index += screen_bpp;
		} // for x
	} // for y
}
void saim_rasterizer_async__initialize_bitmap_buffer(saim_target_info * target_info,
	int key_min_x, int key_max_x, int key_min_y, int key_max_y, int level_of_detail, bool is_break)
{
	int width, height, bitmap_height;
	saim_bitmap_buffer * buffer = (saim_bitmap_buffer *) target_info->bitmap_buffer;

	width = key_max_x - key_min_x + 1;
	if (width <= 0)
    	width += (1 << level_of_detail);
	height = key_max_y - key_min_y + 1;

	if (buffer->data) // there was some data
	{
		// Check whether we need to realloc data
		bitmap_height = buffer->key_max_y - buffer->key_min_y + 1;
		if (width > buffer->width || height > bitmap_height)
		{
			SAIM_FREE(buffer->data);
			buffer->data = (saim_buffered_bitmap_info *)SAIM_MALLOC(width * height * sizeof(saim_buffered_bitmap_info));
			memset(buffer->data, 0, width * height * sizeof(saim_buffered_bitmap_info));
		}
		else
		{
			// Otherwise we will just update the data
			memset(buffer->data, 0, buffer->width * bitmap_height * sizeof(saim_buffered_bitmap_info));
		}
	}
	else
	{
		buffer->data = (saim_buffered_bitmap_info *)SAIM_MALLOC(width * height * sizeof(saim_buffered_bitmap_info));
	}

	// Set new parameters
	buffer->key_min_x = key_min_x;
	buffer->width = width;
	buffer->key_min_y = key_min_y;
	buffer->key_max_x = key_max_x;
	buffer->key_max_y = key_max_y;
	buffer->level_of_detail = level_of_detail;
	buffer->is_break = is_break;
	buffer->has_any_low_detailed_bitmap = false;
}
void saim_rasterizer_async__set_bitmap_for_buffer(saim_target_info * target_info, int key_x, int key_y, const saim_bitmap * bitmap)
{
	int x, y, index;
	saim_buffered_bitmap_info * data;
	saim_bitmap_buffer * buffer = (saim_bitmap_buffer *) target_info->bitmap_buffer;

	x = key_x - buffer->key_min_x;
	if (x < 0) // within break
	{
	    x += (1 << buffer->level_of_detail);
	}
	y = key_y - buffer->key_min_y;
	index = y * buffer->width + x;
#ifdef DEBUG
	assert(x >= 0 && x < buffer->width);
	assert(y >= 0 && y < buffer->key_max_y - buffer->key_min_y + 1);
#endif
	data = buffer->data + index;
	data->bitmap = bitmap;
	data->level_ascending = 0;
}
void saim_rasterizer_async__set_bitmap_low_for_buffer(saim_target_info * target_info, 
	int key_z_low, int key_x, int key_y, const saim_bitmap * bitmap)
{
	int x, y, index;
	saim_buffered_bitmap_info * data;
	saim_bitmap_buffer * buffer = (saim_bitmap_buffer *) target_info->bitmap_buffer;

	x = key_x - buffer->key_min_x;
	if (x < 0) // within break
	{
		x += (1 << buffer->level_of_detail);
	}
	y = key_y - buffer->key_min_y;
	index = y * buffer->width + x;
#ifdef DEBUG
	assert(x >= 0 && x < buffer->width);
	assert(y >= 0 && y < buffer->key_max_y - buffer->key_min_y + 1);
#endif
	data = buffer->data + index;
	data->bitmap = bitmap;
	data->level_ascending = buffer->level_of_detail - key_z_low;

	buffer->has_any_low_detailed_bitmap = true;
}
const saim_buffered_bitmap_info * saim_rasterizer_async__get_bitmap_from_buffer(saim_target_info * target_info, int key_x, int key_y)
{
	int x, y, index;
	saim_bitmap_buffer * buffer = (saim_bitmap_buffer *) target_info->bitmap_buffer;

	bool in_x_range = (buffer->is_break)
		? (key_x >= buffer->key_min_x || key_x <= buffer->key_max_x)
		: (key_x >= buffer->key_min_x && key_x <= buffer->key_max_x);
	if (in_x_range && key_y >= buffer->key_min_y && key_y <= buffer->key_max_y)
	{
		x = key_x - buffer->key_min_x;
		if (x < 0) // within break
			x += (1 << buffer->level_of_detail);
		y = key_y - buffer->key_min_y;
		index = y * buffer->width + x;
		return buffer->data + index;
	}
	else
		return NULL;
}
void saim_rasterizer_async__erase_key_from_buffer(saim_target_info * target_info, const saim_data_key * key)
{
	int key_x, key_y, key_z;
	saim_buffered_bitmap_info * data;
	saim_bitmap_buffer * buffer = (saim_bitmap_buffer *) target_info->bitmap_buffer;

	key_x = saim_data_key__get_x(key);
	key_y = saim_data_key__get_y(key);
	key_z = saim_data_key__get_z(key);
	if (buffer->has_any_low_detailed_bitmap && key_z != buffer->level_of_detail)
	{
		int tiles_per_side = 1 << buffer->level_of_detail;
		int height = buffer->key_max_y - buffer->key_min_y + 1;
		int index = 0;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < buffer->width; ++x)
			{
				data = buffer->data + index;
				if (data->level_ascending != 0) // low detailed image
				{
					int cur_key_x = (buffer->key_min_x + x) % tiles_per_side;
					int cur_key_y = buffer->key_min_y + y;
					bool z_condition = (key_z + data->level_ascending == buffer->level_of_detail);
					bool x_condition = (key_x == (cur_key_x >> data->level_ascending));
					bool y_condition = (key_y == (cur_key_y >> data->level_ascending));
					if (z_condition && x_condition && y_condition)
					{
						data->bitmap = NULL;
						data->level_ascending = 0;
					}
				}
				++index;
			}
		}
	}
	else
	{
		bool in_x_range = (buffer->is_break)
			? (key_x >= buffer->key_min_x || key_x <= buffer->key_max_x)
			: (key_x >= buffer->key_min_x && key_x <= buffer->key_max_x);
		if (key_z == buffer->level_of_detail &&
			in_x_range &&
			key_y >= buffer->key_min_y && key_y <= buffer->key_max_y)
		{
			int x = key_x - buffer->key_min_x;
			if (x < 0) // within break
				x += (1 << buffer->level_of_detail);
			int y = key_y - buffer->key_min_y;
			int index = y * buffer->width + x;
			data = buffer->data + index;
			data->bitmap = NULL;
			data->level_ascending = 0;
		}
	}
}
void saim_rasterizer_async__sort_cache(saim_rasterizer_async * rasterizer)
{
	// Predicate is inside this function
	saim_bitmap_cache_info_list__sort(&rasterizer->bitmap_cache);
}

// --------------------------------------------------------------------------
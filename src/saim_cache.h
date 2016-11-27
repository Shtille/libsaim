#ifndef __SAIM_CACHE_H__
#define __SAIM_CACHE_H__

#include "saim_storage.h"
#include "saim_tile_service.h"
#include "saim_region_service.h"

typedef struct {
	saim_tile_service_t tile_service;
	saim_region_service_t region_service;
	storage_t storage;
} saim_cache;

void saim_cache_create(saim_cache * cache, const char* hash_string);
void saim_cache_destroy(saim_cache * cache);

bool saim_cache_initialize(saim_cache * cache);

//! Whether key exists in our database
bool saim_cache_is_exist(saim_cache * cache, const data_key_t * key);

//! Query to load a single tile using tile service
void saim_cache_tile_service_load_query(saim_cache * cache, const data_key_t * key, saim_tile_notification_function function);

//! Query to load a single tile using region service
void saim_cache_region_service_load_query(saim_cache * cache, const data_key_t * key, const saim_string* name, saim_region_notification_function function);

//! Clear all regions download tasks
void saim_cache_clear_region_service_tasks(saim_cache * cache);

//! Should be called before any saim_cache_region_service_load_query call
bool saim_cache_region_add(saim_cache * cache, const region_info_t * region_info);

bool saim_cache_region_rename(saim_cache * cache, const saim_string* old_name, const saim_string* new_name);

bool saim_cache_region_delete(saim_cache * cache, const saim_string* name);

bool saim_cache_region_mark_stored(saim_cache * cache, const saim_string* name);

bool saim_cache_region_mark_invalid(saim_cache * cache, const saim_string* name);

bool saim_cache_get_region_info(saim_cache * cache, const saim_string* name, stored_region_info_t ** info);

big_file_size_t saim_cache_get_region_file_size(saim_cache * cache, const saim_string* name);

unsigned int saim_cache_get_key_count(saim_cache * cache);

//! Estimated storage file size for specified number of tiles and average data size
big_file_size_t saim_cache_get_estimated_storage_size(unsigned int num_tiles, unsigned int data_size);

#endif
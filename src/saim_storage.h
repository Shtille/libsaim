#ifndef __SAIM_STORAGE_H__
#define __SAIM_STORAGE_H__

#include "saim_key_pair.h"
#include "saim_region_info.h"
#include "saim_regions_header_file.h"
#include "saim_region_map.h"
#include "saim_storage_info_map.h"
#include "saim_key_set.h"

#include "../deps/tinycthread.h"

typedef enum {
	kStorage_Success,
	kStorage_Failed,
	kStorage_Regenerated
} initialize_result_t;

typedef struct storage_t storage_t;

struct storage_t {
	unsigned int hash_value;				//!< stored hash value for selected provider
	mtx_t critical_section;					//!< critical section handle
	mtx_t critical_section_key_set;			//!< critical section handle for key set
	key_set_t key_set;						//!< set of keys, done for fast rendering access
	storage_info_t main_info;				//!< tile service data main file
	regions_header_file_t regions_header_file;
	region_map_t region_map;				//!< map with regions information
	storage_info_map_t region_info_map;		//!< map with regions data information
};

/* =========== External functions =========== */

bool storage_create(storage_t * storage, const char* hash_string);
void storage_destroy(storage_t * storage);

//! Storage initialization
bool storage_initialize(storage_t * storage);

//! Called by tile service, finds storage for a key
key_pair_t * storage_get_key_pair(storage_t * storage, const data_key_t * key, storage_info_t ** storage_info);

//! Called by rendering thread, checks whether key is in any storage
bool storage_is_exist(storage_t * storage, const data_key_t * key);

//! Called by tile service, loads data from a chosen source
bool storage_load(storage_t * storage, key_pair_t * pair, saim_string * data, storage_info_t * info);

//! Called by tile service, saves data to main storage file
bool storage_save_main(storage_t * storage, const data_key_t * key, const saim_string * data);

//! Called by region service, saves data to some named region storage file
bool storage_save_separate(storage_t * storage, const data_key_t * key, const saim_string * data, const saim_string * name);

bool storage_region_add(storage_t * storage, const region_info_t * region_info);
bool storage_region_rename(storage_t * storage, const saim_string * old_name, const saim_string * new_name);
bool storage_region_delete(storage_t * storage, const saim_string * name);
bool storage_region_mark_stored(storage_t * storage, const saim_string * name);
bool storage_region_mark_invalid(storage_t * storage, const saim_string * name);

//! Returns information about region
bool storage_get_region_info(storage_t * storage, const saim_string * name, stored_region_info_t ** info);
//! Returns information about regions
//bool storage_get_regions_info(storage_t * storage, std::vector<StoredRegionInfo>& regions_info);
//! Returns region file size in bytes
big_file_size_t storage_get_region_file_size(storage_t * storage, const saim_string * name);

unsigned int storage_get_key_count(storage_t * storage);

//! Estimated storage file size for specified number of tiles and average data size
big_file_size_t storage_get_estimated_file_size(unsigned int num_tiles, unsigned int data_size);

//! Maximum storage file size for a region
big_file_size_t storage_get_maximum_region_file_size();

/* =========== Internal functions =========== */

initialize_result_t storage_initialize_file(storage_t * storage, storage_info_t * info);
bool storage_initialize_regions_header_file(storage_t * storage);
void storage_initialize_key_set(storage_t * storage);
void storage_deinitialize_regions(storage_t * storage);
void storage_deinitialize(storage_t * storage);
void storage_generate_region_file_name(storage_t * storage, char* buffer);
bool storage_save(storage_t * storage, const data_key_t * key, const saim_string * data, storage_info_t * info);

#endif
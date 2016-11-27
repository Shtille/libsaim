#ifndef __SAIM_REGIONS_HEADER_FILE_H__
#define __SAIM_REGIONS_HEADER_FILE_H__

#include "saim_file.h"
#include "saim_region_map.h"

typedef struct {
	saim_file file;
	unsigned int unique_id;		//!< unique region identifier (counter)
	unsigned int num_regions;	//!< number of regions
} regions_header_file_t;

void regions_header_file_create(regions_header_file_t * file, const char* filename);
void regions_header_file_destroy(regions_header_file_t * file);

bool regions_header_file_add_region(regions_header_file_t * file, region_map_t * map);
bool regions_header_file_remove_region(regions_header_file_t * file, region_map_t * map);
bool regions_header_file_update(regions_header_file_t * file, region_map_t * map); //!< simply rewrite entire file
void regions_header_file_read_all_regions(regions_header_file_t * file, region_map_t * map);

bool regions_header_file_flush(regions_header_file_t * file);
void regions_header_file_regenerate(regions_header_file_t * file);

void regions_header_file_mark_operations_begin(regions_header_file_t * file);
void regions_header_file_mark_operations_end(regions_header_file_t * file);

// Write operations
void regions_header_file_write_header(regions_header_file_t * file);
void regions_header_file_write_region_info(regions_header_file_t * file, const region_info_t * region);
void regions_header_file_write_region_stored_info(regions_header_file_t * file, const stored_region_info_t * region);

// Read operations
bool regions_header_file_read_header(regions_header_file_t * file);
void regions_header_file_read_region_info(regions_header_file_t * file, region_info_t * region);
void regions_header_file_read_region_stored_info(regions_header_file_t * file, stored_region_info_t * region);


#endif
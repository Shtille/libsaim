#ifndef __SAIM_REGIONS_HEADER_FILE_H__
#define __SAIM_REGIONS_HEADER_FILE_H__

#include "saim_file.h"
#include "saim_region_map.h"

typedef struct {
	saim_file file;
	unsigned int unique_id;		//!< unique region identifier (counter)
	unsigned int num_regions;	//!< number of regions
} saim_regions_header_file;

void saim_regions_header_file__create(saim_regions_header_file * file, const char* filename);
void saim_regions_header_file__destroy(saim_regions_header_file * file);

bool saim_regions_header_file__add_region(saim_regions_header_file * file, saim_region_map * map);
bool saim_regions_header_file__remove_region(saim_regions_header_file * file, saim_region_map * map);
bool saim_regions_header_file__update(saim_regions_header_file * file, saim_region_map * map); //!< simply rewrite entire file
void saim_regions_header_file__read_all_regions(saim_regions_header_file * file, saim_region_map * map);

bool saim_regions_header_file__flush(saim_regions_header_file * file);
void saim_regions_header_file__regenerate(saim_regions_header_file * file);

void saim_regions_header_file__mark_operations_begin(saim_regions_header_file * file);
void saim_regions_header_file__mark_operations_end(saim_regions_header_file * file);

// Write operations
void saim_regions_header_file__write_header(saim_regions_header_file * file);
void saim_regions_header_file__write_region_info(saim_regions_header_file * file, const saim_region_info * region);
void saim_regions_header_file__write_region_stored_info(saim_regions_header_file * file, const saim_stored_region_info * region);

// Read operations
bool saim_regions_header_file__read_header(saim_regions_header_file * file);
void saim_regions_header_file__read_region_info(saim_regions_header_file * file, saim_region_info * region);
void saim_regions_header_file__read_region_stored_info(saim_regions_header_file * file, saim_stored_region_info * region);


#endif
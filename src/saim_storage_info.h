#ifndef __SAIM_STORAGE_INFO_H__
#define __SAIM_STORAGE_INFO_H__

#include "saim_storage_file.h"

typedef struct {
	saim_storage_file file;			//!< storage file handler
	saim_key_offset_map offsets;	//!< offsets map
	saim_map_nodes_list list;		//!< need to get fast access to a key with lowest counter
} saim_storage_info;

void saim_storage_info__create(saim_storage_info * info);
void saim_storage_info__destroy(saim_storage_info * info);

#endif
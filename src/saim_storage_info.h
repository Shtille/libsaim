#ifndef __SAIM_STORAGE_INFO_H__
#define __SAIM_STORAGE_INFO_H__

#include "saim_storage_file.h"

typedef struct {
	saim_storage_file file;		//!< storage file handler
	key_offset_map_t offsets;	//!< offsets map
	saim_map_nodes_list list;			//!< need to get fast access to a key with lowest counter
} storage_info_t;

void storage_info_create(storage_info_t * info);
void storage_info_destroy(storage_info_t * info);

#endif
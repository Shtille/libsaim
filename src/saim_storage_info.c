#include "saim_storage_info.h"

#include <stdlib.h>

void storage_info_create(storage_info_t * info)
{
	key_offset_map_create(&info->offsets);
	saim_map_nodes_list__create(&info->list);
}
void storage_info_destroy(storage_info_t * info)
{
	key_offset_map_destroy(&info->offsets);
	saim_map_nodes_list__destroy(&info->list);
}
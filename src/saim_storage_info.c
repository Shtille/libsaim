#include "saim_storage_info.h"

#include <stdlib.h>

void storage_info_create(storage_info_t * info)
{
	key_offset_map_create(&info->offsets);
	key_list_create(&info->list);
}
void storage_info_destroy(storage_info_t * info)
{
	key_offset_map_destroy(&info->offsets);
	key_list_destroy(&info->list);
}
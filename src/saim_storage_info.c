#include "saim_storage_info.h"

#include <stdlib.h>

void saim_storage_info__create(saim_storage_info * info)
{
	saim_key_offset_map__create(&info->offsets);
	saim_map_nodes_list__create(&info->list);
}
void saim_storage_info__destroy(saim_storage_info * info)
{
	saim_key_offset_map__destroy(&info->offsets);
	saim_map_nodes_list__destroy(&info->list);
}
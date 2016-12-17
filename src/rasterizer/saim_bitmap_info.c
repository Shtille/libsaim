#include "saim_bitmap_info.h"

void saim_bitmap_info__create(saim_bitmap_info * bitmap_info)
{
	saim_bitmap__create(&bitmap_info->bitmap);
}
void saim_bitmap_info__destroy(saim_bitmap_info * bitmap_info)
{
	saim_bitmap__destroy(&bitmap_info->bitmap);
}
#include "saim_bitmap.h"

#include "../util/saim_memory.h"

void saim_bitmap__create(saim_bitmap * bitmap)
{
	bitmap->data = NULL;
}
void saim_bitmap__destroy(saim_bitmap * bitmap)
{
	if (bitmap->data)
		SAIM_FREE(bitmap->data);
}
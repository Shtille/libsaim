#include "saim_bitmap_buffer.h"

#include "../util/saim_memory.h"

void saim_bitmap_buffer__create(saim_bitmap_buffer * buffer)
{
	buffer->data = 0;
}
void saim_bitmap_buffer__destroy(saim_bitmap_buffer * buffer)
{
	if (buffer->data)
	{
		SAIM_FREE(buffer->data);
		buffer->data = NULL;
	}
}
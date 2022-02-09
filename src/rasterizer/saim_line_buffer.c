#include "saim_line_buffer.h"

#include "../util/saim_memory.h"

#include <stdlib.h>

void saim_line_buffer__create(saim_line_buffer * line_buffer)
{
	line_buffer->x_buffer_size = 0;
	line_buffer->y_buffer_size = 0;
	line_buffer->x_keys = NULL;
	line_buffer->x_samples = NULL;
	line_buffer->x_pixels = NULL;
	line_buffer->y_keys = NULL;
	line_buffer->y_samples = NULL;
	line_buffer->y_pixels = NULL;
}
void saim_line_buffer__destroy(saim_line_buffer * line_buffer)
{
	if (line_buffer->x_buffer_size)
	{
		SAIM_FREE(line_buffer->x_keys);
		SAIM_FREE(line_buffer->x_samples);
		SAIM_FREE(line_buffer->x_pixels);
		line_buffer->x_buffer_size = 0;
	}
	if (line_buffer->y_buffer_size)
	{
		SAIM_FREE(line_buffer->y_keys);
		SAIM_FREE(line_buffer->y_samples);
		SAIM_FREE(line_buffer->y_pixels);
		line_buffer->y_buffer_size = 0;
	}
}
void saim_line_buffer__reallocate(saim_line_buffer * line_buffer, int total_buffer_size)
{
	if (line_buffer->x_buffer_size != total_buffer_size)
	{
		int alloc_size = total_buffer_size * sizeof(int);
		if (line_buffer->x_buffer_size)
		{
			line_buffer->x_keys    = (int*) SAIM_REALLOC(line_buffer->x_keys,    alloc_size);
			line_buffer->x_samples = (int*) SAIM_REALLOC(line_buffer->x_samples, alloc_size);
			line_buffer->x_pixels  = (int*) SAIM_REALLOC(line_buffer->x_pixels,  alloc_size);
		}
		else // x_buffer_size == 0
		{
			line_buffer->x_keys    = (int*) SAIM_MALLOC(alloc_size);
			line_buffer->x_samples = (int*) SAIM_MALLOC(alloc_size);
			line_buffer->x_pixels  = (int*) SAIM_MALLOC(alloc_size);
		}
		line_buffer->x_buffer_size = total_buffer_size;
	}
	if (line_buffer->y_buffer_size != total_buffer_size)
	{
		int alloc_size = total_buffer_size * sizeof(int);
		if (line_buffer->y_buffer_size)
		{
			line_buffer->y_keys    = (int*) SAIM_REALLOC(line_buffer->y_keys,    alloc_size);
			line_buffer->y_samples = (int*) SAIM_REALLOC(line_buffer->y_samples, alloc_size);
			line_buffer->y_pixels  = (int*) SAIM_REALLOC(line_buffer->y_pixels,  alloc_size);
		}
		else // y_buffer_size == 0
		{
			line_buffer->y_keys    = (int*) SAIM_MALLOC(alloc_size);
			line_buffer->y_samples = (int*) SAIM_MALLOC(alloc_size);
			line_buffer->y_pixels  = (int*) SAIM_MALLOC(alloc_size);
		}
		line_buffer->y_buffer_size = total_buffer_size;
	}
}
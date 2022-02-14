/*************************************************************************
* libsaim 1.0
* ------------------------------------------------------------------------
*  Copyright (c) 2022 Vladimir Sviridov <v.shtille@gmail.com>
* 
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
* 
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
* 
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
* 
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
* 
*  3. This notice may not be removed or altered from any source
*     distribution.
* 
**************************************************************************/

#include "saim_target_info.h"

#include "../util/saim_memory.h"
#include "saim_bitmap_buffer.h"
#include "saim_line_buffer.h"

bool saim_target_info__create(saim_target_info * info, unsigned char* buffer, int width, int height, int bpp)
{
	saim_line_buffer * line_buffer;

	info->target_buffer = buffer;
	info->target_width = width;
	info->target_height = height;
	info->target_bpp = bpp;
	info->bitmap_buffer = NULL;
	info->line_buffer = NULL;

	info->bitmap_buffer = SAIM_MALLOC(sizeof(saim_bitmap_buffer));
	if (info->bitmap_buffer == NULL)
		return false;
	saim_bitmap_buffer__create((saim_bitmap_buffer *) info->bitmap_buffer);

	info->line_buffer = SAIM_MALLOC(sizeof(saim_line_buffer));
	if (info->line_buffer == NULL)
	{
		SAIM_FREE(info->bitmap_buffer);
		info->bitmap_buffer = NULL;
		return false;
	}
	line_buffer = (saim_line_buffer *) info->line_buffer;
	saim_line_buffer__create(line_buffer);

	// To speadup the rendering, reallocate buffers on preprocess
	const int total_buffer_size = (width > height) ? (width << 1) : (height << 1);
	saim_line_buffer__reallocate(line_buffer, total_buffer_size);

	return true;
}
void saim_target_info__destroy(saim_target_info * info)
{
	if (info->bitmap_buffer != NULL)
	{
		saim_bitmap_buffer__destroy((saim_bitmap_buffer *) info->bitmap_buffer);
		SAIM_FREE(info->bitmap_buffer);
		info->bitmap_buffer = NULL;
	}
	if (info->line_buffer != NULL)
	{
		saim_line_buffer__destroy((saim_line_buffer *) info->line_buffer);
		SAIM_FREE(info->line_buffer);
		info->line_buffer = NULL;
	}
}
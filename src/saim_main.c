/*************************************************************************
* libsaim 1.0
* ------------------------------------------------------------------------
*  Copyright (c) 2016 Vladimir Sviridov <v.shtille@gmail.com>
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

#include "saim.h"

#include "saim_instance.h"
#include "rasterizer/saim_rasterizer.h"
#include "util/saim_memory.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

saim_instance * saim_init(const char* path, saim_provider_info * provider_info, 
    int flags, int service_count, int * error)
{
	size_t length;
	char* hash_string;
	saim_instance * instance;

	instance = (saim_instance *) SAIM_MALLOC(sizeof(saim_instance));
	if (!saim_instance__create(instance))
	{
		fprintf(stderr, "instance creation failed\n");
		if (error) *error = 9;
		SAIM_FREE(instance);
		return NULL;
	}

	// Append path delimeter here
	length = strlen(path);
	if (length > 259)
	{
		fprintf(stderr, "path parameter is too long\n");
		if (error) *error = 1;
		saim_instance__destroy(instance);
		SAIM_FREE(instance);
		return NULL;
	}
	strncpy(instance->path, path, length);
	if (length > 0)
	{
		if (path[length-1] != '/')
		{
			instance->path[length] = '/';
			instance->path[length+1] = '\0';
		}
		else
			instance->path[length] = '\0';
	}
	else
		instance->path[length] = '\0';

	// Initialize cache
	if (provider_info != NULL)
		hash_string = provider_info->string;
	else
		hash_string = "default"; // it's normal to pass temporary object!
	if (!saim_instance__initialize(instance, hash_string, service_count))
	{
		fprintf(stderr, "cache initialization failed\n");
		if (error) *error = 2;
		saim_instance__deinitialize(instance);
		saim_instance__destroy(instance);
		SAIM_FREE(instance);
		return NULL;
	}

	// Set provider parameters
	if (!saim_instance__set_provider(instance, provider_info, flags))
	{
		fprintf(stderr, "provider setup failed\n");
		if (error) *error = 3;
		saim_instance__deinitialize(instance);
		saim_instance__destroy(instance);
		SAIM_FREE(instance);
		return NULL;
	}

	if (error) *error = 0;
	return instance;
}
void saim_cleanup(saim_instance * instance)
{
	// Deinitialize instance
	saim_instance__deinitialize(instance);
	saim_instance__destroy(instance);
	SAIM_FREE(instance);
}
void saim_set_target(saim_instance * instance, 
	unsigned char * buffer, int width, int height, int bytes_per_pixel)
{
	instance->rasterizer->target_buffer = buffer;
	instance->rasterizer->target_width = width;
	instance->rasterizer->target_height = height;
	instance->rasterizer->target_bpp = bytes_per_pixel;
}
void saim_set_bitmap_cache_size(saim_instance * instance, unsigned int size)
{
	instance->rasterizer->max_bitmap_cache_size = size;
}
void saim_set_memory_functions(
	void* (*user_malloc)(size_t /*size*/),
	void* (*user_calloc)(size_t /*num*/, size_t /*size*/),
	void* (*user_realloc)(void* /*what*/, size_t /*size*/),
	void(*user_free)(void* /*what*/),
	char* (*user_strdup)(const char* /*str*/)
)
{
#ifdef SAIM_USING_USER_MEMORY_FUNCTIONS
	saim_user_memory_functions * funcs = saim_memory_get_functions_ptr();
	funcs->user_malloc = user_malloc;
	funcs->user_calloc = user_calloc;
	funcs->user_realloc = user_realloc;
	funcs->user_free = user_free;
	funcs->user_strdup = user_strdup;
#else
	assert(!"You should define SAIM_USING_USER_MEMORY_FUNCTIONS to use this function");
#endif
}
int saim_render_aligned(saim_instance * instance, double upper_latitude, double left_longitude, double lower_latitude, double right_longitude)
{
	return saim_rasterizer__render_aligned(instance->rasterizer,
		upper_latitude, left_longitude, lower_latitude, right_longitude);
}
int saim_render_common(saim_instance * instance, double upper_latitude, double left_longitude, double lower_latitude, double right_longitude, float angle)
{
	return saim_rasterizer__render_common(instance->rasterizer,
		upper_latitude, left_longitude, lower_latitude, right_longitude, angle);
}
int saim_render_mapped_cube(saim_instance * instance, int face, int lod, int x, int y)
{
	return saim_rasterizer__render_mapped_cube(instance->rasterizer,
		face, lod, x, y);
}
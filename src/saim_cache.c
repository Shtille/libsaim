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

#include "saim_cache.h"

#include "util/saim_memory.h"

bool saim_cache__create(saim_cache * cache, const char* hash_string, int service_count, struct saim_instance * instance)
{
	int i;
	cache->tile_service_counter = 0;
	cache->tile_service_count = service_count;
	if (service_count <= 0)
		cache->tile_service_count = 1; // default value
	if (cache->tile_service_count > SAIM_MAX_TILE_SERVICE_COUNT)
		cache->tile_service_count = SAIM_MAX_TILE_SERVICE_COUNT;
	for (i = 0; i < cache->tile_service_count; ++i)
	{
		if (!saim_tile_service__create(&cache->tile_services[i]))
			return false;
		saim_tile_service__run_service(&cache->tile_services[i]);
	}

	if (!saim_storage__create(&cache->storage, hash_string, instance))
		return false;

	return true;
}
void saim_cache__destroy(saim_cache * cache)
{
	int i;
	for (i = 0; i < cache->tile_service_count; ++i)
	{
		saim_tile_service__stop_service(&cache->tile_services[i]);
		saim_tile_service__destroy(&cache->tile_services[i]);
	}

	saim_storage__destroy(&cache->storage);
}
bool saim_cache__initialize(saim_cache * cache)
{
	return saim_storage__initialize(&cache->storage);
}
bool saim_cache__is_exist(saim_cache * cache, const saim_data_key * key)
{
	return saim_storage__is_exist(&cache->storage, key);
}
void saim_cache__tile_service_load_query(saim_cache * cache, const saim_data_key * key, saim_tile_notification_function function)
{
	saim_tile_service_task * task;
	int index;
	index = cache->tile_service_counter % cache->tile_service_count;
	++cache->tile_service_counter;
	task = (saim_tile_service_task *)SAIM_MALLOC(sizeof(saim_tile_service_task));
	saim_tile_service_task__create(task, &cache->storage, &cache->tile_services[index].curl_wrapper, function, key);
	saim_tile_service__add_task(&cache->tile_services[index], task);
}
void saim_cache__region_service_load_query(saim_cache * cache, const saim_data_key * key, const saim_string* name, saim_region_notification_function function)
{
	saim_region_service_task * task;
	task = (saim_region_service_task *)SAIM_MALLOC(sizeof(saim_region_service_task));
	saim_region_service_task__create(task, &cache->storage, &cache->region_service.curl_wrapper, function, key, name);
	saim_region_service__add_task(&cache->region_service, task);
}
void saim_cache__clear_region_service_tasks(saim_cache * cache)
{
	saim_region_service__clear_tasks(&cache->region_service);
}
bool saim_cache__region_add(saim_cache * cache, const saim_region_info * region_info)
{
	return saim_storage__region_add(&cache->storage, region_info);
}
bool saim_cache__region_rename(saim_cache * cache, const saim_string* old_name, const saim_string* new_name)
{
	return saim_storage__region_rename(&cache->storage, old_name, new_name);
}
bool saim_cache__region_delete(saim_cache * cache, const saim_string* name)
{
	return saim_storage__region_delete(&cache->storage, name);
}
bool saim_cache__region_mark_stored(saim_cache * cache, const saim_string* name)
{
	return saim_storage__region_mark_stored(&cache->storage, name);
}
bool saim_cache__region_mark_invalid(saim_cache * cache, const saim_string* name)
{
	return saim_storage__region_mark_invalid(&cache->storage, name);
}
bool saim_cache__get_region_info(saim_cache * cache, const saim_string* name, saim_stored_region_info ** info)
{
	return saim_storage__get_region_info(&cache->storage, name, info);
}
big_file_size_t saim_cache__get_region_file_size(saim_cache * cache, const saim_string* name)
{
	return saim_storage__get_region_file_size(&cache->storage, name);
}
unsigned int saim_cache__get_key_count(saim_cache * cache)
{
	return saim_storage__get_key_count(&cache->storage);
}
big_file_size_t saim_cache__get_estimated_storage_size(unsigned int num_tiles, unsigned int data_size)
{
	return saim_storage__get_estimated_file_size(num_tiles, data_size);
}
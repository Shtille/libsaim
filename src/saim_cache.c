#include "saim_cache.h"

#include "util/saim_memory.h"

bool saim_cache_create(saim_cache * cache, const char* hash_string)
{
	if (!saim_tile_service__create(&cache->tile_service))
		return false;
	saim_tile_service__run_service(&cache->tile_service);

	if (!storage_create(&cache->storage, hash_string))
		return false;

	return true;
}
void saim_cache_destroy(saim_cache * cache)
{
	saim_tile_service__stop_service(&cache->tile_service);
	saim_tile_service__destroy(&cache->tile_service);

	storage_destroy(&cache->storage);
}
bool saim_cache_initialize(saim_cache * cache)
{
	return storage_initialize(&cache->storage);
}
bool saim_cache_is_exist(saim_cache * cache, const data_key_t * key)
{
	return storage_is_exist(&cache->storage, key);
}
void saim_cache_tile_service_load_query(saim_cache * cache, const data_key_t * key, saim_tile_notification_function function)
{
	saim_tile_service_task_t * task;
	task = (saim_tile_service_task_t *)SAIM_MALLOC(sizeof(saim_tile_service_task_t));
	saim_tile_service_task__create(task, &cache->storage, &cache->tile_service.curl_wrapper, function, key);
	saim_tile_service__add_task(&cache->tile_service, task);
}
void saim_cache_region_service_load_query(saim_cache * cache, const data_key_t * key, const saim_string* name, saim_region_notification_function function)
{
	saim_region_service_task_t * task;
	task = (saim_region_service_task_t *)SAIM_MALLOC(sizeof(saim_region_service_task_t));
	saim_region_service_task__create(task, &cache->storage, &cache->region_service.curl_wrapper, function, key, name);
	saim_region_service__add_task(&cache->region_service, task);
}
void saim_cache_clear_region_service_tasks(saim_cache * cache)
{
	saim_region_service__clear_tasks(&cache->region_service);
}
bool saim_cache_region_add(saim_cache * cache, const region_info_t * region_info)
{
	return storage_region_add(&cache->storage, region_info);
}
bool saim_cache_region_rename(saim_cache * cache, const saim_string* old_name, const saim_string* new_name)
{
	return storage_region_rename(&cache->storage, old_name, new_name);
}
bool saim_cache_region_delete(saim_cache * cache, const saim_string* name)
{
	return storage_region_delete(&cache->storage, name);
}
bool saim_cache_region_mark_stored(saim_cache * cache, const saim_string* name)
{
	return storage_region_mark_stored(&cache->storage, name);
}
bool saim_cache_region_mark_invalid(saim_cache * cache, const saim_string* name)
{
	return storage_region_mark_invalid(&cache->storage, name);
}
bool saim_cache_get_region_info(saim_cache * cache, const saim_string* name, stored_region_info_t ** info)
{
	return storage_get_region_info(&cache->storage, name, info);
}
big_file_size_t saim_cache_get_region_file_size(saim_cache * cache, const saim_string* name)
{
	return storage_get_region_file_size(&cache->storage, name);
}
unsigned int saim_cache_get_key_count(saim_cache * cache)
{
	return storage_get_key_count(&cache->storage);
}
big_file_size_t saim_cache_get_estimated_storage_size(unsigned int num_tiles, unsigned int data_size)
{
	return storage_get_estimated_file_size(num_tiles, data_size);
}
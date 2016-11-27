#include "saim_tile_service_task.h"

#include "saim_storage.h"
#include "saim_curl_wrapper.h"
#include "saim_provider.h"

extern saim_provider * s_provider;

static size_t on_data_received(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	saim_string * str = (saim_string *)(userdata);
	const char* data_buffer = (const char*)(buffer);
	saim_string_append(str, data_buffer, (unsigned int)(nmemb * size));
	return nmemb * size;
}

void saim_tile_service_task__create(saim_tile_service_task_t * task, 
	struct storage_t * storage,
	struct saim_curl_wrapper_t * curl_wrapper,
	saim_tile_notification_function function,
	const data_key_t* key)
{
	saim_string_create(&task->data);
	task->storage = storage;
	task->curl_wrapper = curl_wrapper;
	task->function = function;
	task->key = *key;
	task->is_load = false;
}
void saim_tile_service_task__destroy(saim_tile_service_task_t * task)
{
	saim_string_destroy(&task->data);
}
bool saim_tile_service_task__execute(saim_tile_service_task_t * task)
{
	storage_info_t * storage_info;
	key_pair_t * pair;
	char url_buffer[260];

	pair = storage_get_key_pair(task->storage, &task->key, &storage_info);
	task->is_load = (pair != 0);

	if (task->is_load) // load
	{
		return storage_load(task->storage, pair, &task->data, storage_info);
	}
	else // download
	{
		// Setup working key to make buffer be filled properly
		saim_provider__fill_buffer_for_key(s_provider, &task->key, url_buffer);
		return saim_curl_wrapper__download(task->curl_wrapper,
			url_buffer, (void*)&task->data, on_data_received);
	}
}
void saim_tile_service_task__notify(saim_tile_service_task_t * task, bool success)
{
	if (success)
	{
		if (task->is_load)
		{
			// Then notify main observer (to let copy or move data)
			if (task->function)
				task->function(&task->key, &task->data, true);
		}
		else // download
		{
			// Save data to the main storage
			if (storage_save_main(task->storage, &task->key, &task->data))
			{
				// Then notify main observer (to let copy or move data)
				if (task->function)
					task->function(&task->key, &task->data, true);
			}
			else
			{
				fprintf(stderr, "saim: failed to save to main storage\n");
			}
		}
	}
	else // skip notification
	{
		if (task->function)
			task->function(&task->key, &task->data, false);
	}
}
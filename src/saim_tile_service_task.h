#ifndef __SAIM_TILE_SERVICE_TASK_H__
#define __SAIM_TILE_SERVICE_TASK_H__

#include "saim_data_key.h"

#include "util/saim_string.h"

#include <stdbool.h>

typedef void (*saim_tile_notification_function)(const data_key_t * key, saim_string * data, bool success);

typedef struct saim_tile_service_task saim_tile_service_task;

struct saim_tile_service_task {
	saim_string data;
	struct saim_storage * storage;					//!< storage pointer
	struct saim_curl_wrapper * curl_wrapper;	//!< curl wrapper pointer
	saim_tile_notification_function function;	//!< notification function
	data_key_t key;								//!< data key
	bool is_load;								//!< load task or download
};

void saim_tile_service_task__create(saim_tile_service_task * task, 
	struct saim_storage * storage,
	struct saim_curl_wrapper * curl_wrapper,
	saim_tile_notification_function function,
	const data_key_t* key);
void saim_tile_service_task__destroy(saim_tile_service_task * task);

bool saim_tile_service_task__execute(saim_tile_service_task * task);
void saim_tile_service_task__notify(saim_tile_service_task * task, bool success);

#endif
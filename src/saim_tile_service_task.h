#ifndef __SAIM_TILE_SERVICE_TASK_H__
#define __SAIM_TILE_SERVICE_TASK_H__

#include "saim_data_key.h"

#include "util/saim_string.h"

#include <stdbool.h>

typedef void (*saim_tile_notification_function)(const data_key_t * key, saim_string * data, bool success);

typedef struct saim_tile_service_task_t saim_tile_service_task_t;

struct saim_tile_service_task_t {
	saim_string data;
	struct storage_t * storage;					//!< storage pointer
	struct saim_curl_wrapper_t * curl_wrapper;	//!< curl wrapper pointer
	saim_tile_notification_function function;	//!< notification function
	data_key_t key;								//!< data key
	bool is_load;								//!< load task or download
};

void saim_tile_service_task__create(saim_tile_service_task_t * task, 
	struct storage_t * storage,
	struct saim_curl_wrapper_t * curl_wrapper,
	saim_tile_notification_function function,
	const data_key_t* key);
void saim_tile_service_task__destroy(saim_tile_service_task_t * task);

bool saim_tile_service_task__execute(saim_tile_service_task_t * task);
void saim_tile_service_task__notify(saim_tile_service_task_t * task, bool success);

#endif
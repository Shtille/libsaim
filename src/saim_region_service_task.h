#ifndef __SAIM_REGION_SERVICE_TASK_H__
#define __SAIM_REGION_SERVICE_TASK_H__

#include "saim_data_key.h"

#include "util/saim_string.h"

#include <stdbool.h>

typedef void (*saim_region_notification_function)(
	const data_key_t * key, saim_string * data, const saim_string * region_name, bool success);

typedef struct saim_region_service_task_t saim_region_service_task_t;

struct saim_region_service_task_t {
	saim_string data;
	struct storage_t * storage;					//!< storage pointer
	struct saim_curl_wrapper_t * curl_wrapper;	//!< curl wrapper pointer
	saim_region_notification_function function;	//!< notification function
	data_key_t key;								//!< data key
	saim_string region_name;					//!< name of the region
};

void saim_region_service_task__create(saim_region_service_task_t * task, 
	struct storage_t * storage,
	struct saim_curl_wrapper_t * curl_wrapper,
	saim_region_notification_function function,
	const data_key_t * key,
	const saim_string * region_name);
void saim_region_service_task__destroy(saim_region_service_task_t * task);

bool saim_region_service_task__execute(saim_region_service_task_t * task);
void saim_region_service_task__notify(saim_region_service_task_t * task, bool success);

#endif
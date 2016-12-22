#ifndef __SAIM_REGION_SERVICE_TASK_H__
#define __SAIM_REGION_SERVICE_TASK_H__

#include "saim_data_key.h"

#include "util/saim_string.h"

#include <stdbool.h>

typedef void (*saim_region_notification_function)(
	const saim_data_key * key, saim_string * data, const saim_string * region_name, bool success);

typedef struct saim_region_service_task saim_region_service_task;

struct saim_region_service_task {
	saim_string data;
	struct saim_storage * storage;					//!< storage pointer
	struct saim_curl_wrapper * curl_wrapper;	//!< curl wrapper pointer
	saim_region_notification_function function;	//!< notification function
	saim_data_key key;								//!< data key
	saim_string region_name;					//!< name of the region
};

void saim_region_service_task__create(saim_region_service_task * task, 
	struct saim_storage * storage,
	struct saim_curl_wrapper * curl_wrapper,
	saim_region_notification_function function,
	const saim_data_key * key,
	const saim_string * region_name);
void saim_region_service_task__destroy(saim_region_service_task * task);

bool saim_region_service_task__execute(saim_region_service_task * task);
void saim_region_service_task__notify(saim_region_service_task * task, bool success);

#endif
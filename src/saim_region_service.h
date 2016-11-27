#ifndef __SAIM_REGION_SERVICE_H__
#define __SAIM_REGION_SERVICE_H__

#include "saim_region_service_task.h"
#include "saim_curl_wrapper.h"
#include "util/saim_list.h"

#include "../deps/tinycthread.h"

typedef struct saim_region_service_t saim_region_service_t;

struct saim_region_service_t {
	mtx_t critical_section;				//!< critical section handle
	thrd_t thread;						//!< thread handle
	saim_curl_wrapper_t curl_wrapper;	//!< each service has its own instance of curl wrapper
	saim_list tasks;					//!< tasks list
	bool finishing;						//!< finishing flag
};

bool saim_region_service__create(saim_region_service_t * service);
void saim_region_service__destroy(saim_region_service_t * service);

void saim_region_service__run_service(saim_region_service_t * service);
void saim_region_service__stop_service(saim_region_service_t * service);

void saim_region_service__clear_tasks(saim_region_service_t * service);
void saim_region_service__add_task(saim_region_service_t * service, saim_region_service_task_t * task);

unsigned int saim_region_service__get_pending_count(saim_region_service_t * service);

#endif
#ifndef __SAIM_TILE_SERVICE_H__
#define __SAIM_TILE_SERVICE_H__

#include "saim_tile_service_task.h"
#include "saim_curl_wrapper.h"
#include "util/saim_list.h"

#include "../deps/tinycthread.h"

typedef struct saim_tile_service saim_tile_service;

struct saim_tile_service {
	mtx_t critical_section;				//!< critical section handle
	thrd_t thread;						//!< thread handle
	saim_curl_wrapper curl_wrapper;	//!< each service has its own instance of curl wrapper
	saim_list tasks;					//!< tasks list
	bool finishing;						//!< finishing flag
};

bool saim_tile_service__create(saim_tile_service * service);
void saim_tile_service__destroy(saim_tile_service * service);

void saim_tile_service__run_service(saim_tile_service * service);
void saim_tile_service__stop_service(saim_tile_service * service);

void saim_tile_service__clear_tasks(saim_tile_service * service);
void saim_tile_service__add_task(saim_tile_service * service, saim_tile_service_task * task);

unsigned int saim_tile_service__get_pending_count(saim_tile_service * service);

#endif
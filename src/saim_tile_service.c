#include "saim_tile_service.h"

#include "util/saim_memory.h"
#include "saim_manager.h"

#include <stdio.h>

static int thread_func(void * arg)
{
	volatile bool finishing = false;
	saim_tile_service_t * service = (saim_tile_service_t *)arg;
	saim_tile_service_task_t *task, *cur_task;
	saim_list_node *node, *best_node;

	while (!finishing)
	{
        task = NULL;
		mtx_lock(&service->critical_section);
		finishing = service->finishing;
		// Find a task with lowest detail and remove it from list
		if (service->tasks.length != 0)
		{
			best_node = service->tasks.head;
            task = (saim_tile_service_task_t *)best_node->data;
			node = best_node->next;
			while (node != NULL)
			{
				cur_task = (saim_tile_service_task_t *)node->data;
				if (data_key_get_z(&cur_task->key) < data_key_get_z(&task->key)) // less detailed
				{
					task = cur_task;
					best_node = node;
				}
				node = node->next;
			}
			(void)saim_list_delete(&service->tasks, best_node);
		}
		mtx_unlock(&service->critical_section);

		if (task == NULL)
		{
			// There is no tasks to process, sleep a bit
			thrd_yield();
			continue;
		}

		// Check if our key in viewport
		if (!saim_manager__is_in_viewport(&task->key))
		{
			// We don't need this task anymore
			saim_tile_service_task__notify(task, false);
			saim_tile_service_task__destroy(task);
			SAIM_FREE(task);
			continue;
		}

		// Start downloading
		if (saim_tile_service_task__execute(task))
		{
			// There is only observer for the task, so we just notify it about event
			saim_tile_service_task__notify(task, true);
		}
		else
		{
			// We need to remove responding key from requested ones
			saim_tile_service_task__notify(task, false);
		}
		saim_tile_service_task__destroy(task);
		SAIM_FREE(task);
	}
	return 0;
}
static void task_destroy_func(void* task)
{
	SAIM_FREE((saim_tile_service_task_t *)task);
}

bool saim_tile_service__create(saim_tile_service_t * service)
{
	if (mtx_init(&service->critical_section, mtx_plain) == thrd_error)
	{
		fprintf(stderr, "saim: mutex init failed\n");
		return false;
	}
	if (!saim_curl_wrapper__create(&service->curl_wrapper))
		return false;
	// Init task list
	saim_list_create(&service->tasks, task_destroy_func);
	return true;
}
void saim_tile_service__destroy(saim_tile_service_t * service)
{
	// Some tasks still may exist
	saim_tile_service__clear_tasks(service);
	// Destroy task list
	saim_list_destroy(&service->tasks);

	saim_curl_wrapper__destroy(&service->curl_wrapper);
	mtx_destroy(&service->critical_section);
}
void saim_tile_service__run_service(saim_tile_service_t * service)
{
	if (thrd_create(&service->thread, thread_func, service) != thrd_success)
	{
		fprintf(stderr, "saim: thread create failed\n");
	}
}
void saim_tile_service__stop_service(saim_tile_service_t * service)
{
	mtx_lock(&service->critical_section);
	service->finishing = true;
	mtx_unlock(&service->critical_section);
	thrd_join(service->thread, NULL);
}
void saim_tile_service__clear_tasks(saim_tile_service_t * service)
{
	mtx_lock(&service->critical_section);
	saim_list_clear(&service->tasks);
	mtx_unlock(&service->critical_section);
}
void saim_tile_service__add_task(saim_tile_service_t * service, saim_tile_service_task_t * task)
{
	mtx_lock(&service->critical_section);
	saim_list_push_back(&service->tasks, task);
	mtx_unlock(&service->critical_section);
}
unsigned int saim_tile_service__get_pending_count(saim_tile_service_t * service)
{
	unsigned int task_count;
	mtx_lock(&service->critical_section);
	task_count = service->tasks.length;
	mtx_unlock(&service->critical_section);
	return task_count;
}
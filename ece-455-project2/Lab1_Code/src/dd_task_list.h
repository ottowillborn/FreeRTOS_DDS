#ifndef DD_TASK_LIST_H
#define DD_TASK_LIST_H

#include <stdint.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"

typedef struct dd_task {
	TaskHandle_t t_handle;
//	task_type type;
	uint32_t task_id;
	uint32_t release_time;
	uint32_t absolute_deadline;
	uint32_t completion_time;
	uint32_t execution_time;
	uint32_t active_time;
	uint32_t remaining_time;
}DDTask;

// List node
typedef struct dd_task_node {
	DDTask task;
	struct dd_task_node *next;
} DDTaskNode;

// List
typedef struct {
	DDTaskNode* head;
}DDTaskList;

// Initialize list
void dd_list_init(DDTaskList *list);

// Insert and sort list
void dd_list_insert_sort(DDTaskList *list, DDTask task);

// remove desired handle
void dd_list_remove(DDTaskList *list, TaskHandle_t handle);

//Peek List head
DDTask *dd_list_peek_head(DDTaskList *list);

// Find in list
DDTask *dd_list_find(DDTaskList *list, TaskHandle_t handle);

uint32_t dd_count_list(const DDTaskList *list);

#endif DD_TASK_LIST_H;

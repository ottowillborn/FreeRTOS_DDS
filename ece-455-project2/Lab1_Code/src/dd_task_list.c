#include "dd_task_list.h"
#include "../FreeRTOS_Source/include/task.h"

// Initialize list
void dd_list_init(DDTaskList *list) {
	list->head = NULL;
}

// Insert and sort list
void dd_list_insert_sort(DDTaskList *list, DDTask task) {
	DDTaskNode *new_node = pvPortMalloc(sizeof(DDTaskNode));
	if (new_node == NULL) return;

	new_node->task = task;
	new_node->next = NULL;

	// insert at head (earliest deadline)
	if (list->head == NULL || // empty list
			task.absolute_deadline < list->head->task.absolute_deadline || // new task deadline earlier
			(task.absolute_deadline == list->head->task.absolute_deadline
					&& task.remaining_time < list->head->task.remaining_time)) { // deadlines equal but new task remaining time less
		if (list->head != NULL) {
			//printf("ABS head: %u ABS incoming: %u\n",list->head->task.absolute_deadline, task.absolute_deadline);
			//printf("task in list task: %u with remaining time %u. new task %u with remaining time %u\n", list->head->task.task_id, list->head->task.remaining_time, task.task_id, task.remaining_time);
		}
		new_node->next = list-> head;
		list->head = new_node;
		return;
	}
	// traverse to insertion point
	DDTaskNode *cur = list->head;
	while (cur->next != NULL) {
		uint32_t next_dl = cur->next->task.absolute_deadline;
		uint32_t next_remaining = cur->next->task.remaining_time;

		if (next_dl < task.absolute_deadline) {
			cur = cur->next;
			continue;
		}

		if (next_dl == task.absolute_deadline && next_remaining <= task.remaining_time) {
			//printf("task in list task: %u with remaining time %u. new task %u with remaining time %u\n", cur->next->task.task_id, cur->next->task.remaining_time, task.task_id, task.remaining_time);
			cur = cur->next;
			continue;
		}

		break;


	}

	// found insertion poiunt
	new_node->next = cur->next;
	cur->next = new_node;
}

// remove desired handle
void dd_list_remove(DDTaskList *list, TaskHandle_t handle) {
	DDTaskNode *cur = list->head;
	DDTaskNode *prev = NULL;

	while(cur != NULL) {
		if(cur->task.t_handle == handle) {
			if (prev == NULL) { // start of list
				list->head = cur->next;
			} else {
				prev->next = cur->next;
			}

			vPortFree(cur);
			return;
		}

		prev = cur;
		cur = cur->next;
	}
}

//Peek List head
DDTask *dd_list_peek_head(DDTaskList *list) {
	if (list->head == NULL) return NULL;
	return &list->head->task;
}

// Find in list
DDTask *dd_list_find(DDTaskList *list, TaskHandle_t handle) {
	DDTaskNode *cur = list->head;
	while (cur != NULL) {
		if (cur->task.t_handle == handle) {
			return &cur->task;
		}
		cur = cur->next;
	}
	return NULL;
}

uint32_t dd_count_list(const DDTaskList *list){
	uint32_t count = 0;
	DDTaskNode *cur = list->head;
	while (cur!= NULL){
		count++;
		cur = cur->next;
	}
	return count;
}

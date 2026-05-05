/*
	FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
	All rights reserved

	VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

	This file is part of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License (version 2) as published by the
	Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

	***************************************************************************
	>>!   NOTE: The modification to the GPL is included to allow you to     !<<
	>>!   distribute a combined work that includes FreeRTOS without being   !<<
	>>!   obliged to provide the source code for proprietary components     !<<
	>>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

	FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE.  Full license text is available on the following
	link: http://www.freertos.org/a00114.html

	***************************************************************************
	 *                                                                       *
	 *    FreeRTOS provides completely free yet professionally developed,    *
	 *    robust, strictly quality controlled, supported, and cross          *
	 *    platform software that is more than just the market leader, it     *
	 *    is the industry's de facto standard.                               *
	 *                                                                       *
	 *    Help yourself get started quickly while simultaneously helping     *
	 *    to support the FreeRTOS project by purchasing a FreeRTOS           *
	 *    tutorial book, reference manual, or both:                          *
	 *    http://www.FreeRTOS.org/Documentation                              *
	 *                                                                       *
	***************************************************************************

	http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wwrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

	http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
	including FreeRTOS+Trace - an indispensable productivity tool, a DOS
	compatible FAT file system, and our tiny thread aware UDP/IP stack.

	http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
	Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

	http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
	Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
	licenses offer ticketed support, indemnification and commercial middleware.

	http://www.SafeRTOS.com - High Integrity Systems also provide a safety
	engineered and independently SIL3 certified version for use in safety and
	mission critical applications that require provable dependability.

	1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
#include "dd_task_list.h"
// #include "../Libraries/STM32f4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c"
// #include "../Libraries/STM32f4xx_StdPeriph_Driver/src/stm32f4xx_adc.c"

/*-----------------------------------------------------------*/
#define QueueLength 20
//TB1
//#define task1Period 500
//#define task2Period 500
//#define task3Period 750
//#define task1Runtime 95
//#define task2Runtime 150
//#define task3Runtime 250
//TB2
#define task1Period 250
#define task2Period 500
#define task3Period 750
#define task1Runtime 95
#define task2Runtime 150
#define task3Runtime 250
//TB3
//#define task1Period 500
//#define task2Period 500
//#define task3Period 500
//#define task1Runtime 100
//#define task2Runtime 200
//#define task3Runtime 200

void prvTask1TimerCallback(TimerHandle_t xTimer);
void prvTask2TimerCallback(TimerHandle_t xTimer);
void prvTask3TimerCallback(TimerHandle_t xTimer);
void prvMonitorTimerCallback(TimerHandle_t xTimer);

// Enum for scheduler message types
typedef enum {
	MSG_RELEASE,
	MSG_COMPLETE,
	MSG_GET_ACTIVE,
	MSG_GET_COMPLETED,
	MSG_GET_OVERDUE
} message_type;

typedef struct {
	message_type type;
	DDTask task;
} DDSMessage;


static void prvSetupHardware(void);
TimerHandle_t  xTask1Timer, xTask2Timer, xTask3Timer, xMonitorTimer;

// Periods for tasks respective 123
TickType_t xTask1Interval = pdMS_TO_TICKS(task1Period);
TickType_t xTask2Interval = pdMS_TO_TICKS(task2Period);
TickType_t xTask3Interval = pdMS_TO_TICKS(task3Period);
TickType_t xMonitorInterval = pdMS_TO_TICKS(505);

xQueueHandle xNewTaskQueue = 0;
xQueueHandle xTaskReadyQueue = 0;
xQueueHandle xDDSMessageQueue = 0;
xQueueHandle xActiveListQueue = 0;
xQueueHandle xOverdueListQueue = 0;
xQueueHandle xCompletedListQueue = 0;

// DDS tasks
static void DDSchedulertask(void *pvParameters);
static void DDGeneratorTask(void *pvParameters);
static void DDSMonitorTask(void *pvParameters);

// Job Simulator tasks
static void OneTask(void *pvParameters);
static void TwoTask(void *pvParameters);
static void ThreeTask(void *pvParameters);

/*-----------------------------------------------------------*/
// Callback functions for Task timers
void prvTask1TimerCallback(TimerHandle_t xTimer)
{
	uint16_t id = 1;
	xQueueSend(xNewTaskQueue, &id, portMAX_DELAY);
}
void prvTask2TimerCallback(TimerHandle_t xTimer)
{
	uint16_t id = 2;
	xQueueSend(xNewTaskQueue, &id, portMAX_DELAY);
}
void prvTask3TimerCallback(TimerHandle_t xTimer)
{
	uint16_t id = 3;
	xQueueSend(xNewTaskQueue, &id, portMAX_DELAY);
}
void prvMonitorTimerCallback(TimerHandle_t xTimer)
{

    DDSMessage activeMessage = {
    		.type = MSG_GET_ACTIVE,
			.task = NULL
    };
    xQueueSend(xDDSMessageQueue, &activeMessage, portMAX_DELAY);
    DDSMessage completeMessage = {
			.type = MSG_GET_COMPLETED,
			.task = NULL
	};
	xQueueSend(xDDSMessageQueue, &completeMessage, portMAX_DELAY);
	DDSMessage overdueMessage = {
			.type = MSG_GET_OVERDUE,
			.task = NULL
	};
	xQueueSend(xDDSMessageQueue, &overdueMessage, portMAX_DELAY);
}



int main(void)
{
	prvSetupHardware();
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	printf("Clocks on\n");

	xNewTaskQueue = xQueueCreate(QueueLength, sizeof(uint16_t));
	vQueueAddToRegistry(xNewTaskQueue, "New Task Alert Queue");

	xTaskReadyQueue = xQueueCreate(QueueLength, sizeof(DDTask));
	vQueueAddToRegistry(xTaskReadyQueue, "New Task Ready Queue");

	xDDSMessageQueue = xQueueCreate(QueueLength, sizeof(DDSMessage));
	vQueueAddToRegistry(xDDSMessageQueue, "DDS message queue");

	xActiveListQueue = xQueueCreate(QueueLength, sizeof(DDTaskList *));
	vQueueAddToRegistry(xActiveListQueue, "Active list reference queue");
	xOverdueListQueue = xQueueCreate(QueueLength, sizeof(DDTaskList *));
	vQueueAddToRegistry(xOverdueListQueue, "overdue list reference queue");
	xCompletedListQueue = xQueueCreate(QueueLength, sizeof(DDTaskList *));
	vQueueAddToRegistry(xCompletedListQueue, "Completed list reference queue");

	printf("Added queues\n");

	// Create tasks
	xTaskCreate(DDGeneratorTask, "Deadline Driven Task Generator", 256, NULL, 4, NULL);
	xTaskCreate(DDSchedulertask, "Deadline Driven Task Scheduler", 256, NULL, 5, NULL);
	xTaskCreate(DDSMonitorTask, "Deadline Driven Task Monitor", 256, NULL, 3, NULL);
	printf("Added tasks\n");

	// Set timers, pd true for auto reload
	xTask1Timer = xTimerCreate("Task 1 Timer", xTask1Interval, pdTRUE, (void *)1, prvTask1TimerCallback);
	xTask2Timer = xTimerCreate("Task 2 Timer", xTask2Interval, pdTRUE, (void *)2, prvTask2TimerCallback);
	xTask3Timer = xTimerCreate("Task 3 Timer", xTask3Interval, pdTRUE, (void *)3, prvTask3TimerCallback);
	xMonitorTimer = xTimerCreate("Monitor task timer", xMonitorInterval, pdTRUE, (void *)4, prvMonitorTimerCallback);

	printf("Added timers\n");

	/* Start the tasks and timers running. */

	xTimerStart(xTask1Timer, 0);
	xTimerStart(xTask2Timer, 0);
	xTimerStart(xTask3Timer, 0);
	xTimerStart(xMonitorTimer, 0);

	// Release all tasks at t=0 to start.
	uint16_t id;
	id = 1;
	xQueueSend(xNewTaskQueue, &id, 0);
	id = 2;
	xQueueSend(xNewTaskQueue, &id, 0);
	id = 3;
	xQueueSend(xNewTaskQueue, &id, 0);
	vTaskStartScheduler();



	printf("Tasks and timers started!!!!\n");

	return 0;
}



static void complete_dd_task(DDTaskList *activeList, DDTaskList *completeList, DDTaskList *overdueList, DDTask task, TickType_t startTime){
	TickType_t curTime = xTaskGetTickCount();
	//move from active list to complete list or overdue list

	DDTask *completeTask = dd_list_find(activeList, task.t_handle);
	if (completeTask == NULL) {
		printf("Error\n");
		return;
	}

	DDTask fullInfoTask = *completeTask;
	fullInfoTask.completion_time = curTime;
	dd_list_remove(activeList, task.t_handle);

	if (fullInfoTask.completion_time <= fullInfoTask.absolute_deadline) {
		dd_list_insert_sort(completeList, fullInfoTask);
		printf("Task %u complete ontime at %u ms\n", fullInfoTask.task_id, fullInfoTask.completion_time);
	} else {
		dd_list_insert_sort(overdueList, fullInfoTask);
		printf("Task %u overdue at %u ms\n", fullInfoTask.task_id, fullInfoTask.completion_time);
	}


	// delete task
	vTaskDelete(task.t_handle);

	// start next task
	DDTask *newHead = dd_list_peek_head(activeList);
	if (newHead != NULL) {
		vTaskPrioritySet(newHead->t_handle, 2);
		newHead->active_time = curTime;
	}

}

static void drop_overdue_active_tasks(DDTaskList *activeList, DDTaskList *overdueList, TickType_t startTime) {
	TickType_t curTime = xTaskGetTickCount();
	BaseType_t droppedAny = pdFALSE;

	DDTask *head = dd_list_peek_head(activeList);
	while ((head != NULL) && curTime > head->absolute_deadline) {
		DDTask overdueTask = *head;
		overdueTask.completion_time = curTime;

		dd_list_remove(activeList, overdueTask.t_handle);
		vTaskDelete(overdueTask.t_handle);
		dd_list_insert_sort(overdueList, overdueTask);

		droppedAny = pdTRUE;

		head = dd_list_peek_head(activeList);
		printf("task %u OVERDUE at %u ms\n", overdueTask.task_id, overdueTask.completion_time);
	}

	if (droppedAny == pdTRUE) {
		DDTask *newHead = dd_list_peek_head(activeList);
		vTaskPrioritySet(newHead->t_handle, 2);
		newHead->active_time = curTime;
	}
}

static void get_active_dd_task_list(DDTaskList *activeList) {
	xQueueSend(xActiveListQueue, &activeList, portMAX_DELAY);
}
static void get_overdue_dd_task_list(DDTaskList *overdueList) {
	xQueueSend(xOverdueListQueue, &overdueList, portMAX_DELAY);
}
static void get_completed_dd_task_list(DDTaskList *completedList) {
	xQueueSend(xCompletedListQueue, &completedList, portMAX_DELAY);
}

static void release_dd_task(DDTaskList *list, DDTask newTask, TickType_t startTime) {
    TickType_t curTime = xTaskGetTickCount();

    // Keep track of the current running task (the old head)
    DDTask *oldHead = dd_list_peek_head(list);

    TaskHandle_t oldHandle = NULL;
    TickType_t oldActive;
    uint32_t oldRemaining;

    // timing updates for preemption
    if (oldHead!= NULL) {
    	oldHandle = oldHead->t_handle;
    	uint32_t elapsed = curTime - oldHead->active_time;
    	oldHead->remaining_time = (elapsed < oldHead->remaining_time) ? (oldHead->remaining_time - elapsed) : (0);
    	oldHead->active_time = curTime;
    }

    // Insert the new task into the sorted linked list
    dd_list_insert_sort(list, newTask);

    // Get the new head of the list after insertion
    DDTask *newHead = dd_list_peek_head(list);
    if (newHead == NULL) {
    	printf("Error!!!!!!\n");
    	return;
    }

    // Handle Scheduler Logic (Preemption)
    if (oldHandle != NULL) {

        // If the head of the list changed, the new task has an earlier deadline
        if (newHead->t_handle != oldHandle) {

            // Lower priority of the old task, raise priority of the new one
            vTaskPrioritySet(oldHandle, 1);
            vTaskPrioritySet(newHead->t_handle, 2);

           // DDTask *oldRunningTask = dd_list_find(list, oldHandle);

            newHead->active_time = curTime;
        }
    } else {
        // This is the first task added to an empty list
        vTaskPrioritySet(newHead->t_handle, 2);
        newHead->active_time = curTime;
    }
}

static void DDSchedulertask(void *pvParameters) {

	TickType_t startTime = xTaskGetTickCount();
	TickType_t curTime;

	DDSMessage message;

	// Initialize list
	DDTaskList activeList;
	dd_list_init(&activeList);

	DDTaskList completeList;
	dd_list_init(&completeList);

	DDTaskList overdueList;
		dd_list_init(&overdueList);


	while(1){
		//If task in message Q

		if (xQueueReceive(xDDSMessageQueue, &message, portMAX_DELAY) == pdTRUE){

			//switch
			switch (message.type) {
				case MSG_RELEASE:
					curTime = xTaskGetTickCount();
					release_dd_task(&activeList, message.task, startTime);
					printf("released Task %u at %u ms\n", message.task.task_id, curTime - startTime);
					break;
				case MSG_COMPLETE:
					complete_dd_task(&activeList, &completeList, &overdueList, message.task, startTime);
					break;
				case MSG_GET_ACTIVE:
					get_active_dd_task_list(&activeList);
					break;
				case MSG_GET_COMPLETED:
					get_completed_dd_task_list(&completeList);
					break;
				case MSG_GET_OVERDUE:
					get_overdue_dd_task_list(&overdueList);
					break;
			}
		}

		drop_overdue_active_tasks(&activeList, &overdueList, startTime);
	}
}

// Given parameters for a new task, "create" it by adding it to a queue for DDS
void create_dd_task(TaskHandle_t t_handle, uint32_t task_id, uint32_t release_time,
                    uint32_t absolute_deadline, uint32_t completion_time, uint32_t execution_time) {

    // Initialize the struct using designated initializers for clarity
    DDTask deadlineTask = {
        .t_handle = t_handle,
        .task_id = task_id,
        .release_time = release_time,
        .absolute_deadline = absolute_deadline,
        .completion_time = completion_time,
        .execution_time = execution_time,
		.active_time = 0,
		.remaining_time = execution_time
    };

    DDSMessage message = {
    		.type = MSG_RELEASE,
			.task = deadlineTask
    };
    //create dds_message obj and send to message queue

    xQueueSend(xDDSMessageQueue, &message, portMAX_DELAY);

}

/*
This task will periodically generate DD-Tasks that will be scheduled by the DDS.
Done by calling create_dd_task() and sending dd_task to DDS via binary queue.
*/
static void DDGeneratorTask(void *pvParameters){
	uint16_t newTaskID;
	TaskHandle_t taskHandle;
	TickType_t xTimeStamp;
	TickType_t releaseTick;
	uint32_t first1 = 1;
	uint32_t first2 = 1;
	uint32_t first3 = 1;

	while(1){
		//If task in xNewTaskQueue
		if (xQueueReceive(xNewTaskQueue, &newTaskID, portMAX_DELAY) == pdTRUE){
			xTimeStamp = xTaskGetTickCount();
			switch(newTaskID){
				case 1:
					xTaskCreate(OneTask, "Task 1 created", 256, (void*)(uintptr_t)task1Runtime, 0, &taskHandle);
					if(first1){
						create_dd_task (taskHandle, 1, 0, task1Period, 0, task1Runtime);
						first1 = 0;
					} else {
						create_dd_task (taskHandle, 1, 0, xTimeStamp + task1Period, 0, task1Runtime);
					}
					break;
				case 2:
					xTaskCreate(TwoTask, "Task 2 created", 256, (void*)(uintptr_t)task2Runtime, 0, &taskHandle);
					if(first2){
						create_dd_task (taskHandle, 2, 0, task2Period, 0, task2Runtime);
						first2 = 0;
					}else{
						create_dd_task (taskHandle, 2, 0, xTimeStamp + task2Period, 0, task2Runtime);
					}
					break;
				case 3:
					xTaskCreate(ThreeTask, "Task 3 created", 256, (void*)(uintptr_t)task3Runtime, 0, &taskHandle);
					if (first3){
						create_dd_task (taskHandle, 3, 0, task3Period, 0, task3Runtime);
						first3 = 0;
					}else{
						create_dd_task (taskHandle, 3, 0, xTimeStamp + task3Period, 0, task3Runtime);
					}
					break;
			}



		}
	}
}

// used to monitor all lists.
static void DDSMonitorTask(void *pvParameters) {

	DDTaskList *activeList;
	DDTaskList *completedList;
	DDTaskList *overdueList;
	uint32_t activeCount;
	uint32_t completeCount;
	uint32_t overdueCount;



	while (1) {
		if (xQueueReceive(xActiveListQueue, &activeList, portMAX_DELAY) == pdTRUE){
			activeCount = dd_count_list(activeList);
		}
		if (xQueueReceive(xCompletedListQueue, &completedList, portMAX_DELAY) == pdTRUE){
			completeCount = dd_count_list(completedList);
		}
		if (xQueueReceive(xOverdueListQueue, &overdueList, portMAX_DELAY) == pdTRUE){
			overdueCount = dd_count_list(overdueList);
		}
		printf("Active |%u| Complete |%u| Overdue |%u|\n", activeCount, completeCount, overdueCount);
	}
}

static void simulate_exec(uint32_t runtime_ms) {
	TickType_t start = xTaskGetTickCount();
	TickType_t target = pdMS_TO_TICKS(runtime_ms);
	TickType_t executed = 0u;
	TickType_t lastTick = xTaskGetTickCount();

	// force at least 1 tick execution
	if ((runtime_ms > 0u) && (target == 0u)) {
		target = 1u;
	}

	while (executed < target) {
		TickType_t now = xTaskGetTickCount();
		TickType_t delta = now - lastTick;

		// if no tick passed run nop
		if (delta == 0u) {
			__asm__ volatile("nop");
			continue;
		}

		// if at least 1 tick has passed in this loop increment progress
		if (delta == 1u) {
			executed++;
		}

		lastTick = now;

	}

}

static void notify_done_and_suspend(uint32_t task_id) {
	DDTask completedTask = {
			.t_handle = xTaskGetCurrentTaskHandle(), // Captures this specific task's handle
			.task_id = task_id,                            // Example ID
			.completion_time = xTaskGetTickCount(),  // Record when we finished
			.remaining_time = 0                      // Task is complete
		};


	DDSMessage message = {
			.type = MSG_COMPLETE,
			.task = completedTask
	};
	//create dds_message obj and send to message queue
	xQueueSend(xDDSMessageQueue, &message, portMAX_DELAY);

	vTaskSuspend(NULL);
}

static void OneTask(void *pvParameters){
	uint32_t runtime_ms = (uint32_t)(uintptr_t)pvParameters;
	simulate_exec(runtime_ms);
	notify_done_and_suspend(1);
}

static void TwoTask(void *pvParameters){
	uint32_t runtime_ms = (uint32_t)(uintptr_t)pvParameters;
	simulate_exec(runtime_ms);
	notify_done_and_suspend(2);
}
static void ThreeTask(void *pvParameters){
	uint32_t runtime_ms = (uint32_t)(uintptr_t)pvParameters;
	simulate_exec(runtime_ms);
	notify_done_and_suspend(3);
}

void vApplicationMallocFailedHook(void)
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
	(void)pcTaskName;
	(void)pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
	volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if (xFreeStackSpace > 100)
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware(void)
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping(0);

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}





// for tomorrow:

//finish setting up api calls + waiting for response for monitor (currently doing active list)
//	might need to use a timer / callback to fire api requests off periodically then wait for a response in monitor queue or something.

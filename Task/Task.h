#ifndef TASK_H_
#define TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "util.h"

/* Setting count parameter in tsk_CreateTask() to INFINITE will make the task
 * run for infinite number of times */
#define INFINITE 0

/* Definition of struct Task */
typedef struct Task Task;

/* Task function: takes in no parameters and returns STATUS */
typedef STATUS (*TaskFunction)(void);

/* Request a task. Will call function for count number of times at interval ms.
 * If no more space available, task is set to NULL and returns ERROR */
STATUS tsk_CreateTask(Task** task, TaskFunction function, uint32 intervalMs,
        uint16 count);

/* Pause task. If task is NULL, returns ERROR */
STATUS tsk_Pause(Task* task);

/* Resume task. If task is NULL, returns ERROR */
STATUS tsk_Resume(Task* task);

/* Query whether task is running or not. If task is NULL, returns ERROR */
STATUS tsk_IsRunning(Task* task, uint8* isRunning);

/* Stop and trigger task. Will delete task. If task is NULL, returns ERROR */
STATUS tsk_StopAndTriggerTask(Task** task);

/* Cancel without triggering task. Will delete task. If task is NULL, returns
 * ERROR */
STATUS tsk_Cancel(Task** task);

/* Change TaskFunction. If task is NULL, returns ERROR */
STATUS tsk_ChangeTaskFunction(Task* task, TaskFunction function);

/* Change count number. If task is NULL, returns ERROR */
STATUS tsk_ChangeCount(Task* task, uint16 count);

/* Change interval time. If task is NULL, returns ERROR */
STATUS tsk_ChangeIntervalMs(Task* task, uint16 intervalMs);

/* Delete task and sets task to NULL. If task is NULL, returns ERROR */
STATUS tsk_DeleteTask(Task** task);

/* Start scheduler. Will never return */
STATUS tsk_StartScheduler();

#ifdef __cplusplus
}
#endif

#endif /* TASK_H_ */


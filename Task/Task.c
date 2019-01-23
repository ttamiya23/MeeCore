#include "Task.h"
#include "TaskSettings.h"
#include "Iterator/Iterator.h"
#include "Timer/Timer.h"
#include "Debug/Debug.h"

/* Definition of struct Task */
#pragma pack(1)
typedef struct Task
{
    TaskFunction function;
    uint16 remainingCount;
    uint32 intervalMs;
    uint32 elapsedTimeMs;
    uint8 isRunning : 1;
    uint8 isInfinite : 1;
} Task;

/* List of all Tasks */
static Task taskList[TASK_NUM];

/* Index of the first empty task. -1 if all tasks are used up */
static int16 emptyTaskIndex = 0;

/* Variable to store which task are initialized */
static
#if TASK_NUM <= 8
uint8
#elif ITERATOR_NUM <= 16
uint16
#elif ITERATOR_NUM <= 32
uint32
#else
uint64
#endif
initializedTasks = 0;

/* Request a task. Will call function for count number of times at interval ms.
 * If no more space available, task is set to NULL and returns ERROR */
STATUS tsk_CreateTask(Task** task, TaskFunction function, uint32 intervalMs,
        uint16 count)
{
    STATUS ret = ERROR;
    if (task == NULL || *task == NULL || function == NULL)
        return ret;
    if (emptyTaskIndex < 0)
    {
        *task = NULL;
        return ret;
    }

    // Initialize task
    (taskList + emptyTaskIndex)->function = function;
    (taskList + emptyTaskIndex)->remainingCount = count;
    (taskList + emptyTaskIndex)->intervalMs = intervalMs;
    (taskList + emptyTaskIndex)->elapsedTimeMs = 0;
    (taskList + emptyTaskIndex)->isRunning = TRUE;
    (taskList + emptyTaskIndex)->isInfinite = count == INFINITE;
    *task = taskList + emptyTaskIndex;

    SET_BIT(initializedTasks, emptyTaskIndex);
    ret = SUCCESS;

    // Linearly search for empty iterator
    uint16 i, index;
    for (i = 1; i < TASK_NUM; ++i)
    {
        index = (emptyTaskIndex + i) % TASK_NUM;
        if (!IS_BIT_SET(initializedTasks, index))
        {
            emptyTaskIndex = index;
            return ret;
        }
    }

    // All iterators are used up!
    emptyTaskIndex = -1;
    return ret;

}

/* Pause task. If task is NULL, returns ERROR */
STATUS tsk_Pause(Task* task)
{
    STATUS ret = ERROR;
    if (task == NULL)
        return ret;

    task->isRunning = FALSE;

    ret = SUCCESS;
    return ret;
}

/* Resume task. If task is NULL, returns ERROR */
STATUS tsk_Resume(Task* task)
{
    STATUS ret = ERROR;
    if (task == NULL)
        return ret;

    task->isRunning = TRUE;

    ret = SUCCESS;
    return ret;
}

/* Query whether task is running or not. If task is NULL, returns ERROR */
STATUS tsk_IsRunning(Task* task, uint8* isRunning)
{
    STATUS ret = ERROR;
    if (task == NULL || isRunning == NULL)
        return ret;

    *isRunning = task->isRunning;

    ret = SUCCESS;
    return ret;
}

/* Stop and trigger task. Will delete task. If task is NULL, returns ERROR */
STATUS tsk_StopAndTriggerTask(Task** task)
{
    STATUS ret = ERROR;
    if (task == NULL || *task == NULL)
        return ret;

    (*task)->function();
    ret = tsk_DeleteTask(task);

    return ret;
}

/* Cancel without triggering task. Will delete task. If task is NULL, returns
 * ERROR */
STATUS tsk_Cancel(Task** task)
{
    STATUS ret = ERROR;
    if (task == NULL || *task == NULL)
        return ret;

    ret = tsk_DeleteTask(task);

    return ret;
}

/* Change TaskFunction. If task is NULL, returns ERROR */
STATUS tsk_ChangeTaskFunction(Task* task, TaskFunction function)
{
    STATUS ret = ERROR;
    if (task == NULL || function == NULL)
        return ret;

    task->function = function;

    ret = SUCCESS;
    return ret;
}

/* Change count number. If task is NULL, returns ERROR */
STATUS tsk_ChangeCount(Task* task, uint16 count)
{
    STATUS ret = ERROR;
    if (task == NULL)
        return ret;

    task->remainingCount = count;

    ret = SUCCESS;
    return ret;
}


/* Change interval time. If task is NULL, returns ERROR */
STATUS tsk_ChangeIntervalMs(Task* task, uint16 intervalMs)
{
    STATUS ret = ERROR;
    if (task == NULL)
        return ret;

    task->intervalMs = intervalMs;

    ret = SUCCESS;
    return ret;
}

/* Delete task and sets task to NULL. If task is NULL, returns ERROR */
STATUS tsk_DeleteTask(Task** task)
{
    STATUS ret = ERROR;
    if (task == NULL || *task == NULL)
        return ret;

    // Update emptyIteratorIndex and remove from initializedIterators
    emptyTaskIndex = *task - taskList;
    UNSET_BIT(initializedTasks, emptyTaskIndex);
    *task = NULL;
    ret = SUCCESS;
    return ret;
}

/* Start scheduler. Will never return */
STATUS tsk_StartScheduler()
{
    STATUS ret = ERROR;
    uint32 timeMs = 0, lastMs = 0;
    uint32 diff;
    uint16 index;
    Task* currentTask;

    // Forever loop
    while(TRUE)
    {
        // Tick every millisecond
        tmr_GetTimeMs(&timeMs);
        diff = (uint32)(timeMs-lastMs);
        if (diff == 0)
            continue;

        lastMs = timeMs;
        for (index = 0; index < TASK_NUM; ++index)
        {
            // Ignore uninitialized tasks or paused tasks
            currentTask = taskList + index;
            if (!IS_BIT_SET(initializedTasks, index) || !(currentTask->isRunning))
                continue;

            currentTask->elapsedTimeMs += diff;

            // Trigger task if interval ms time has passed
            if (currentTask->elapsedTimeMs >= currentTask->intervalMs)
            {
                currentTask->function();
                if (!(currentTask->isInfinite))
                {
                    // Decrement remaining count and delete if zero
                    (currentTask->remainingCount)--;
                    if (!(currentTask->remainingCount))
                    {
                        tsk_DeleteTask(&currentTask);
                        continue;
                    }
                }
                currentTask->elapsedTimeMs = 0;
            }
        }
    }
    return ret;
}

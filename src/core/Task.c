#include "Task.h"
#include "TaskSettings.h"
#include "Iterator.h"
#include "TimerMs.h"
#include "Debug.h"
#include "assert.h"

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

/* Request a task. Will call function for count times at interval ms */
STATUS tsk_CreateTask(Task** task, TaskFunction function, uint32 intervalMs,
        uint16 count)
{
    STATUS ret = ERROR;

    assert(task != NULL);
    assert(function != NULL);
    assert(emptyTaskIndex >= 0);

    dbg_LogInformation("Adding new task[%i] with interval %i ms and count %i",
            emptyTaskIndex, intervalMs, count);

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
    uint16 offset, index;
    for (offset = 1; offset < TASK_NUM; ++offset)
    {
        index = (emptyTaskIndex + offset) % TASK_NUM;
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

/* Pause task */
STATUS tsk_Pause(Task* task)
{
    STATUS ret = ERROR;
    int16 id = task - taskList;

    assert(task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    dbg_LogInformation("Paused task[%i]", id);
    task->isRunning = FALSE;

    ret = SUCCESS;
    return ret;
}

/* Resume task */
STATUS tsk_Resume(Task* task)
{
    STATUS ret = ERROR;
    int16 id = task - taskList;

    assert(task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    dbg_LogInformation("Resumed task[%i]", id);
    task->isRunning = TRUE;

    ret = SUCCESS;
    return ret;
}

/* Query whether task is running or not */
uint8 tsk_IsRunning(Task* task)
{
    int16 id = task - taskList;

    assert(task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    return task->isRunning;
}

/* Stop and trigger task. Will delete task. */
STATUS tsk_StopAndTriggerTask(Task** task)
{
    STATUS ret = ERROR;
    assert(task != NULL);
    int16 id = *task - taskList;

    assert(*task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    dbg_LogInformation("Stopping and triggering task[%i]", id);

    (*task)->function();
    ret = tsk_DeleteTask(task);

    return ret;
}

/* Cancel without triggering task. Will delete task */
STATUS tsk_Cancel(Task** task)
{
    STATUS ret = ERROR;
    assert(task != NULL);
    int16 id = *task - taskList;

    assert(*task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    dbg_LogInformation("Canceling task[%i]", id);
    ret = tsk_DeleteTask(task);

    return ret;
}

/* Set TaskFunction */
STATUS tsk_SetTaskFunction(Task* task, TaskFunction function)
{
    STATUS ret = ERROR;
    int16 id = task - taskList;

    assert(task != NULL);
    assert(function != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    dbg_LogInformation("Changing task function for task[%i]", id);

    task->function = function;

    ret = SUCCESS;
    return ret;
}

/* Set count number */
STATUS tsk_SetCount(Task* task, uint16 count)
{
    STATUS ret = ERROR;
    int16 id = task - taskList;

    assert(task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    dbg_LogInformation("Changing count for task[%i] from %i to %i", id,
            task->remainingCount, count);

    task->remainingCount = count;

    ret = SUCCESS;
    return ret;
}

/* Set interval time */
STATUS tsk_SetIntervalMs(Task* task, uint16 intervalMs)
{
    STATUS ret = ERROR;
    int16 id = task - taskList;

    assert(task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    dbg_LogInformation("Changing interval for task[%i] from %i ms to %i ms", id,
            task->intervalMs, intervalMs);

    task->intervalMs = intervalMs;

    ret = SUCCESS;
    return ret;
}

/* Get TaskFunction */
TaskFunction tsk_GetTaskFunction(Task* task)
{
    int16 id = task - taskList;

    assert(task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    return task->function;
}

/* Get count number */
uint16 tsk_GetCount(Task* task)
{
    int16 id = task - taskList;

    assert(task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    return task->remainingCount;
}

/* Get interval time */
uint16 tsk_GetIntervalMs(Task* task)
{
    int16 id = task - taskList;

    assert(task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    return task->intervalMs;
}

/* Delete task and sets task to NULL */
STATUS tsk_DeleteTask(Task** task)
{
    STATUS ret = ERROR;
    assert(task != NULL);
    int16 id = *task - taskList;

    assert(*task != NULL);
    assert(id >= 0);
    assert(id < TASK_NUM);
    assert(IS_BIT_SET(initializedTasks, id));

    // Update emptyIteratorIndex and remove from initializedIterators
    dbg_LogInformation("Deleting task[%i]", id);
    emptyTaskIndex = id;
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

    dbg_LogInformation("Starting task scheduler");

    // Forever loop
    while(TRUE)
    {
        // Tick every millisecond
        tms_GetTimeMs(&timeMs);
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
                dbg_LogTrace("Triggering task[%i]", index);

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

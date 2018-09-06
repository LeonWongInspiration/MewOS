/** Multi-Task Management
 * 
 * @author: Leon Wong
 * @build: 201809061850
 * @brief: This file includes fuctions on multitasking and task management.
 * @usage: This file shoule be both included and compiled.
 * 
 */

#ifndef TASK_H
#define TASK_H 1

#include "Memorie.h"
#include "asm_funcs.h"
#include "GDT.h"
#include "Timer.h"
#include "FIFOBuffer.h"
#include "Structs.h"

extern struct TIMER *taskTimer;
extern struct TASK_MANAGER *taskManager;

const static int TASK_GDT0 = 3; // The first number of segment assigned for tasks.
const static int AR_TSS32 = 0x0089; // Address for TSS.
const static int TASK_FREE = 0; // Free flag for tasks.
const static int TASK_ASSIGNED = 1; // Flag of a task when it has been assigned.
const static int TASK_RUNNING = 2; // Running flag for tasks.
const static int LEVEL_FREE = 0; // Free flag for levels.

/**
 * @brief: Init all task-related structs.
 * @param: (MEMORY_FREE_TABLE *)memman: The MEMORY Management struct.
 * @return: (TASK *): The task inited.
 */ 
struct TASK *initTask(struct MEMORY_FREE_TABLE *memman);

/**
 * @brief: Alloc a task.
 * @return: (TASK *): The task allocated, may be NULL if the number of tasks has reached its maximum value.
 * @warning: This may return NULL if failed to alloc a task!
 */ 
struct TASK *allocTask();

/**
 * @brief: Run a task.
 * @param: (TASK *)task: The task to run.
 * @param: (int)level: The level of the task.
 * @param: (int)priority: The priority of the task.
 */ 
void runTask(struct TASK *task, int level, int priority);

/**
 * @brief: Upon every call of this function, the current task will get switched.
 * @seealso: switchTaskSub.
 */ 
void switchTask();

/**
 * @brief: A function help to switch task.
 * @seealso: switchTask.
 */ 
void switchTaskSub();

/**
 * @brief: Make a task sleep.
 * @param: (TASK *)task: The task to sleep.
 */ 
void setTaskSleep(struct TASK *task);

/**
 * @brief: Return the pointer of the current task.
 * @return: (TASK *): The pointer of the current task.
 */ 
struct TASK *getCurrentTask();

/**
 * @brief: Add a task to the task table.
 * @param: (TASK *)task: The task to add.
 */ 
void addTask(struct TASK *task);

/**
 * @brief: Remove a task.
 * @param: (TASK *)task: The task to get destroyed.
 */ 
void removeTask(struct TASK *task);

/**
 * @brief: Call this function when there are no tasks running.
 */ 
void systemIdle();

#endif

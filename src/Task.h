/** Multi-Task Management
 * 
 * @author: Leon Wong
 * @build: 201809052203
 * @brief: This file includes fuctions on multitasking and task management.
 * @usage: This file shoule be both included and compiled.
 * 
 */

#if (!defined(TASK_H))
#define TASK_H 1

#include "Timer.h"
#include "Memorie.h"
#include "asm_funcs.h"
#include "GDT.h"

extern TIMER *taskTimer;

#define MAX_TASK_PER_LVL 100 // How many tasks are allowed in a level.
#define MAX_TASKSLVLS 10 // How many levels we allow to use.
#define MAX_TASK 1000 // How many tasks we allow to run simultaneously.

const static int TASK_GDT0 = 3; // The first number of segment assigned for tasks.
const static int AR_TSS32 = 0x0089; // Address for TSS.
const static int TASK_FREE = 0; // Free flag for tasks.
const static int TASK_ASSIGNED = 1; // Flag of a task when it has been assigned.
const static int TASK_RUNNING = 2; // Running flag for tasks.
const static int LEVEL_FREE = 0; // Free flag for levels.

/// Task Status Segment, saving status of a task.
typedef struct TASK_STAT_SEGM{
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3; // Status of task
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi; // Data of registers (32bit)
    // EIP: Extended Instruction Pointer, i.e. 32 bit Program Counter (PC)
	int es, cs, ss, ds, fs, gs; // Data of registers (16bit)
	int ldtr, iomap; // Status of task
} TASK_STAT_SEGM;

/// Definition of a task.
typedef struct TASK {
    int selector; // Segment's address
    int flags;
    int level; // Priority of a series of tasks. Tasks will be executed first according to its level,
    int priority; // then its priority.
    TASK_STAT_SEGM tss;
} TASK;

/// A series of tasks with same level.
typedef struct TASK_LEVEL {
    int running; // How many tasks are in this level.
    int now; // Which task is now running.
    TASK *tasks[MAX_TASK_PER_LVL];
} TASK_LEVEL;

/// Where we save tasks.
typedef struct TASK_MANAGER {
    int currentLevel;
    char levelChange; // Whether we need to change level on next task changing.
    TASK_LEVEL level[MAX_TASKSLVLS];
    TASK tasks[MAX_TASK];
} TASK_MANAGER;

/**
 * @brief: Init all task-related structs.
 * @param: (MEMORY_FREE_TABLE *)memman: The MEMORY Management struct.
 * @return: (TASK *): The task inited.
 */ 
TASK *initTask(MEMORY_FREE_TABLE *memman);

/**
 * @brief: Alloc a task.
 * @return: (TASK *): The task allocated, may be NULL if the number of tasks has reached its maximum value.
 * @warning: This may return NULL if failed to alloc a task!
 */ 
TASK *allocTask();

/**
 * @brief: Run a task.
 * @param: (TASK *)task: The task to run.
 * @param: (int)level: The level of the task.
 * @param: (int)priority: The priority of the task.
 */ 
void runTask(TASK *task, int level, int priority);

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
void setTaskSleep(TASK *task);

/**
 * @brief: Return the pointer of the current task.
 * @return: (TASK *): The pointer of the current task.
 */ 
TASK *getCurrentTask();

/**
 * @brief: Add a task to the task table.
 * @param: (TASK *)task: The task to add.
 */ 
void addTask(TASK *task);

/**
 * @brief: Remove a task.
 * @param: (TASK *)task: The task to get destroyed.
 */ 
void removeTask(TASK *task);

extern TASK_MANAGER *taskManager;

#endif

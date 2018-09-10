#include "Task.h"

/// Timer used to control the frequency of task switching.
TIMER *taskTimer;

/// Global task manager.
TASK_MANAGER *taskManager;

TASK *initTask(MEMORY_FREE_TABLE *memman){
    int i;
    TASK *task;
    TASK *idle; // This task just set the system idle, and we add this task to the end of task list, therefore, when all tasks are not running (sleep), the system will halt.
    SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *) GDTAddress;
    taskManager = (TASK_MANAGER *) allocMemoryForSize_Page(memman, sizeof(TASK_MANAGER));
    for (i = 0; i < MAX_TASK; ++i) {
        taskManager->tasks[i].flags = TASK_FREE;
        taskManager->tasks[i].selector = (TASK_GDT0 + i) * 8;
        setGDT(gdt + TASK_GDT0 + i, 103, (int) &taskManager->tasks[i].tss, AR_TSS32);
    }
    for (i = 0; i < MAX_TASKSLVLS; ++i) {
        taskManager->level[i].running = LEVEL_FREE;
        taskManager->level[i].now = 0;
    }
    task = allocTask();
    task->flags = TASK_RUNNING;
    task->priority = 2;
    task->level = 0;
    addTask(task);
    switchTaskSub();
    load_tr(task->selector);
    taskTimer = allocTimer();
    timerSetTimeOut(taskTimer, task->priority); // Priority = 2 means switching tasks every 0.02s

    idle = allocTask();
    idle->tss.esp = allocMemoryForSize_Page(memman, 64 * 1024) + 64 * 1024;
	idle->tss.eip = (int) &systemIdle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	runTask(idle, MAX_TASKSLVLS - 1, 1);
    
    return task;
}

TASK *allocTask(){
    int i;
    TASK *task;
    for (i = 0; i < MAX_TASK; ++i) {
        if (taskManager->tasks[i].flags == TASK_FREE) {
            task = &(taskManager->tasks[i]);
            task->flags = TASK_ASSIGNED;
            task->tss.eflags = 0x00000202; // IF = 1
            task->tss.eax = 0;
            task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;
            task->tss.ss0 = 0;
            return task;
        }
    }
    return NULL;
}

void runTask(TASK *task, int level, int priority){
    if (level < 0) {
        level = task->level; // We do not need to change the task's level
    }
    if (priority > 0){
        task->priority = priority;
    }

    if (task->flags == TASK_RUNNING && task->level != level) { // If we change the level of a task.
        // Then it will be "removed", i.e. its flags will be set TASK_ASSIGNED, which will enable codes below.
        removeTask(task);
    }
    if (task->flags != TASK_RUNNING) {
        // Wake up the task.
        task->level = level;
        addTask(task);
    }

    taskManager->levelChange = 1; // We have run a task, thus we need to check the level on next switching.
}

void switchTask(){
    TASK_LEVEL *tl = &(taskManager->level[taskManager->currentLevel]);
    TASK *newTask;
    TASK *currentTask = tl->tasks[tl->now];
    ++(tl->now);
    if (tl->now == tl->running) { // The "current" task in this level is whose index is outside this level.
        tl->now = 0;
    }
    if (taskManager->levelChange != 0) { // We need to check if the level should be changed.
        switchTaskSub();
        tl = &(taskManager->level[taskManager->currentLevel]);
    }
    newTask = tl->tasks[tl->now];
    timerSetTimeOut(taskTimer, newTask->priority);
    if (newTask != currentTask) {
        farjmp(0, newTask->selector);
    }
}

void switchTaskSub(){
    int i;
    // Find the highest level.
    for (i = 0; i < MAX_TASKSLVLS; ++i) {
        if (taskManager->level[i].running > 0) {
            break; // We find a level in which tasks are assigned.
        }
    }
    taskManager->currentLevel = i;
    taskManager->levelChange = 0; // We have now set the level, thus levelChange is no longer needed.
}

void setTaskSleep(TASK *task){
    TASK *currentTask;
    if (task->flags == TASK_RUNNING) {
        // If the task to sleep is now running.
        currentTask = getCurrentTask();
        removeTask(task); // The flag of this task will be TASK_ASSIGNED.
        if (task == currentTask) {
            // If we let current task sleep, we have to switch task.
            switchTaskSub();
            currentTask = getCurrentTask();
            farjmp(0, currentTask->selector);
        }
    }
}

TASK *getCurrentTask(){
    TASK_LEVEL *tl = &(taskManager->level[taskManager->currentLevel]);
    return tl->tasks[tl->now];
}

void addTask(TASK *task){
    TASK_LEVEL *tl = &(taskManager->level[task->level]);
    tl->tasks[tl->running] = task;
    ++(tl->running);
    task->flags = TASK_RUNNING;
}

void removeTask(TASK *task){
    int i;
    TASK_LEVEL *tl = &(taskManager->level[task->level]);

    // Find the index of the task to remove.
    for (i = 0; i < tl->running; ++i) {
        if (tl->tasks[i] == task) {
            break;
        }
    }

    --(tl->running);
    if (i < tl->now) {
        --(tl->now); // The index of the current task is bigger than the task to remove, thus its index will decrease by one.
    }

    if (tl->now >= tl->running) {
        tl->now = 0; // This may occur when the index of current task is wrong.
    }
    task->flags = TASK_ASSIGNED;

    // Move those tasks behind.
    for (; i < tl->running; ++i) {
        tl->tasks[i] = tl->tasks[i + 1];
    }
}

void systemIdle(){
    while (1) {
        io_hlt();
    }
}

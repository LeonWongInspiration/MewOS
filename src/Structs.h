/** Structs Definition
 * 
 * @author: Leon Wong
 * @build: 201809102146 FINAL
 * @brief: This file contains all definitions of structs used in this project.
 * @usage: This file should be only included.
 * 
 */ 

#ifndef STRUCTS_H
#define STRUCTS_H 1

// ------ BootInfo.h ------ //
struct BootInfo {
	char cyls; // Cylinders
	char leds; // Led mode
	char vmode; // Visual mode
	char reserve;
	short scrnx; // Resolution X
	short scrny; // Resolution Y
	char *vram; // Address of VRAM
};

typedef struct BootInfo BootInfo;

// ------ Console.h ------ //
struct CONSOLE {
    struct SHEET *sheet;
    int curX, curY, curColor;
    struct TIMER *timer;
};

typedef struct CONSOLE CONSOLE;

// ------ FAT.h ------ //
struct FAT12 {
    unsigned char name[8];
    unsigned char ext[3];
    unsigned char type;
    char reserve[10];
    unsigned short time, date, clusterNo;
    unsigned int size;
};

typedef struct FAT12 FAT12;

// ------ FIFOBuffer.h ------ //

/**
 * @brief: A FIFO Buffer including 8 bit data.
 */ 
struct FIFOBuffer8 {
    unsigned char *buf; /// Buffer
    int nextWrite; /// Index of next available byte to write.
    int nextRead; /// Index of next byte to read.
    int size; /// Size of the buffer.
    int free; /// Number of spaces available.
    int flags; /// Bitwise status indicator.
};

typedef struct FIFOBuffer8 FIFO8;


/**
 * @brief: A FIFO Buffer including 32bit data.
 */ 
struct FIFOBuffer32 {
    int *buf; /// Buffer
    int nextWrite; /// Index of next available byte to write.
    int nextRead; /// Index of next byte to read.
    int size; /// Size of the buffer.
    int free; /// Number of spaces available.
    int flags; /// Bitwise status indicator.

    struct TASK *task; /// This allow buffer to wake up a task when adding elems.
};

typedef struct FIFOBuffer32 FIFO32;

// ------ GDT.h ------ //

/**
 * 8 byte GDT info
 */ 
struct SEGMENT_DESCRIPTOR{
    short limit_low;
    short base_low;
    char base_mid;
    char access_right;
    char limit_high;
    char base_high;
};

typedef struct SEGMENT_DESCRIPTOR SEGMENT_DESCRIPTOR;

// ------ IDT.h ------ //

/**
 * 8 byte IDT info.
 */ 
struct GATE_DESCRIPTOR {
	short offset_low;
    short selector;
	char dw_count;
    char access_right;
	short offset_high;
};

typedef struct GATE_DESCRIPTOR GATE_DESCRIPTOR;

// ------ Memorie.h ------ //

/// A node saving available memory info.
struct MEMORY_FREE_NODE {
    unsigned int address; /// Address of the memory.
    unsigned int size; /// Size of the memory.
};

typedef struct MEMORY_FREE_NODE MEMORY_FREE_NODE;

/// Nodes of memory
#define MEMORY_FREE_NODE_NUMBER 4090 /// This is about 32KB.

/// A table containing available memory info.
struct MEMORY_FREE_TABLE {
    int frees; /// Number of available infos.
    int maxFrees; /// Number of max frees.
    int lostSize; /// Size of unnoted memory.
    int losts; /// Number of losts.
    struct MEMORY_FREE_NODE table[MEMORY_FREE_NODE_NUMBER];
};

typedef struct MEMORY_FREE_TABLE MEMORY_FREE_TABLE;

// ------ Mouse.h ------ //

struct MOUSE_DECODER{
    unsigned char decodeBuffer[3]; // Because mouse data are in 3 bytes, we need a 3 byte array to contain them.
    unsigned char phase; // Goes as 0, 1, 2, 3, 1, 2, 3, 1, 2, 3 ... to indicate the stat of mouse data.
                        // Note that when phase == 0, we are waiting for mouse to get ready (0xfa).
    int x, y; // Position.
    int btn; // Stat of the buttons.
};

typedef struct MOUSE_DECODER MOUSE_DECODER;

// ------ Sheet.h ------ //

/// The max number of sheets.
#define MAX_SHEETS 256

/// A struct about one sheet.
struct SHEET {
    unsigned char *buf; // Buffer of this sheet.
    int bxsize; // X size.
    int bysize; // Y size.
    int vx0; // Position (Left top)
    int vy0; // Position (Left top)
    int colorAndInvisibility; // Just as its name.
    int height; // The order of the sheet from top to bottom.
    int flags;
    struct SHEET_MANAGER *ctl;
    struct TASK *task;
};

typedef struct SHEET SHEET;

/// Sheet manager
struct SHEET_MANAGER{
    unsigned char *vram; // Address of VRAM
    unsigned char *map; // A map of the screen indicating which pixels' height.
    int xsize; // The X size of the UI
    int ysize; // The Y size of the UI
    int top; // The max number of layers currently. (i.e. height of the highest sheet)
    struct SHEET *sheets[MAX_SHEETS]; // Table of sheets after ordering.
    struct SHEET sheets0[MAX_SHEETS]; // Array to contain all sheets.
};

typedef struct SHEET_MANAGER SHEET_MANAGER;

// ------ Task.h ------ //

#define MAX_TASK_PER_LVL 100 // How many tasks are allowed in a level.
#define MAX_TASKSLVLS 10 // How many levels we allow to use.
#define MAX_TASK 1000 // How many tasks we allow to run simultaneously.

/// Task Status Segment, saving status of a task.
struct TASK_STAT_SEGM {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3; // Status of task
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi; // Data of registers (32bit)
    // EIP: Extended Instruction Pointer, i.e. 32 bit Program Counter (PC)
	int es, cs, ss, ds, fs, gs; // Data of registers (16bit)
	int ldtr, iomap; // Status of task
};

typedef struct TASK_STAT_SEGM TASK_STAT_SEGM;

/// Definition of a task.
struct TASK {
    int selector; // Segment's address
    int flags;
    int level; // Priority of a series of tasks. Tasks will be executed first according to its level,
    int priority; // then its priority.
    struct FIFOBuffer32 fifo;
    struct TASK_STAT_SEGM tss;
    struct CONSOLE *cons; // The console this task runs on.
    int DSBase; // The DS segment register base which will be added to the apps' segments.
    int consStack; // Stack of the console.
};

typedef struct TASK TASK;

/// A series of tasks with same level.
struct TASK_LEVEL {
    int running; // How many tasks are in this level.
    int now; // Which task is now running.
    struct TASK *tasks[MAX_TASK_PER_LVL];
};

typedef struct TASK_LEVEL TASK_LEVEL;

/// Where we save tasks.
struct TASK_MANAGER {
    int currentLevel;
    char levelChange; // Whether we need to change level on next task changing.
    struct TASK_LEVEL level[MAX_TASKSLVLS];
    struct TASK tasks[MAX_TASK];
};

typedef struct TASK_MANAGER TASK_MANAGER;

// ------ Timer.h ------ //

#define MAX_TIMER 500 // The max number of timers

// Timer struct def
struct TIMER {
    struct TIMER *next; // Pointer for "linked list".
    unsigned int timeout; // Total time out for this timer.
    char flags;
    char ownerStat;
    struct FIFOBuffer32 *fifo;
    int data;
};

typedef struct TIMER TIMER;

// Manager for timers
struct TIMER_MANAGER{
    unsigned int count;
    unsigned int next;
    TIMER *t0; // First "Node" of timers
    TIMER timers[MAX_TIMER];
};

typedef struct TIMER_MANAGER TIMER_MANAGER;

#endif

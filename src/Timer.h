/** Programmable Interval Timer (PIT) Management
 * 
 * @author: Leon Wong
 * @build: 201809040932
 * @brief: This file provides functions about PIT and timer.
 * @usage: This file should be both included and compiled.
 * 
 */ 

#if (!(defined(TIMER_H)))
#define TIMER_H 1

#include "asm_funcs.h"
#include "PIC.h"
#include "FIFOBuffer.h"

#include "include\stdio.h"

#define MAX_TIMER 500 // The max number of timers

// Flags definition for timers
const static unsigned int TIMER_FREE = 0;
const static unsigned int TIMER_ALLOCED = 1;
const static unsigned int TIMER_USING = 2;

/// Port help to control PIT
const static int PIT_CTRL = 0x0043;

/// Port help to tell PIT the frequency for timer.
const static int PIT_CNT0 = 0x0040;

// Timer struct def
typedef struct TIMER {
    struct TIMER *next; // Pointer for "linked list".
    unsigned int timeout; // Total time out for this timer.
    unsigned int flags;
    FIFO32 *fifo;
    int data;
} TIMER;

// Manager for timers
typedef struct TIMER_MANAGER{
    unsigned int count;
    unsigned int next;
    TIMER *t0; // First "Node" of timers
    TIMER timers[MAX_TIMER];
} TIMER_MANAGER;

/**
 * @brief: Init PIT with frequency 0x2e9c (0x2e9c = 11932 -> 100Hz)
 */ 
void initPIT();

/**
 * @brief: Alloc a timer and return it.
 * @return (TIMER *) An allocated timer.
 * @warning: If the total number of timers is more than MAX_TIMER, this will return NULL!
 */ 
TIMER *allocTimer();

/**
 * @brief: Free a timer. NOTNULL!
 * @param: (TIMER *)timer: The timer to get freed.
 * @warning: The param timer should NOT be NULL!
 */ 
void freeTimer(TIMER *timer);

/**
 * @brief: Init a timer with given allocated data.
 * @param: (TIMER *)timer: The timer to init.
 * @param: (FIFO32 *)fifo: The fifo buffer alloc for timer.
 * @param: (int)data: Data to init.
 */ 
void initTimer(TIMER *timer, FIFO32 *fifo, int data);

/**
 * @brief: Set timeout for a timer.
 * @param: (TIMER *)timer: The timer to set timeout. NOTNULL
 * @param: (uint)timeout: The timeout ti set.
 * @warning: The param timer should NOT be NULL!
 */ 
void timerSetTimeOut(TIMER *timer, unsigned int timeout);

/**
 * @brief: Handles IRQs from Timer.
 * @param: (int *)esp: ESP register.
 * @seealso: inthandler20
 */ 
void timerInterruptHandler(int *esp);

extern TIMER_MANAGER timerManager;

#endif

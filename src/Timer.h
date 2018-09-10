/** Programmable Interval Timer (PIT) Management
 * 
 * @author: Leon Wong
 * @build: 201809102246 FINAL
 * @brief: This file provides functions about PIT and timer.
 * @usage: This file should be both included and compiled.
 * 
 */ 

#ifndef TIMER_H
#define TIMER_H 1

#include "asm_funcs.h"
#include "PIC.h"
#include "FIFOBuffer.h"
#include "Structs.h"

#include "include\stdio.h"

// Flags definition for timers
const static char TIMER_FREE = 0;
const static char TIMER_ALLOCED = 1;
const static char TIMER_USING = 2;

const static char SYSTIMER = 0;
const static char APPTIMER = 1;

/// Port help to control PIT
const static int PIT_CTRL = 0x0043;

/// Port help to tell PIT the frequency for timer.
const static int PIT_CNT0 = 0x0040;

/**
 * @brief: Init PIT with frequency 0x2e9c (0x2e9c = 11932 -> 100Hz)
 */ 
void initPIT();

/**
 * @brief: Alloc a timer and return it.
 * @return (TIMER *) An allocated timer.
 * @warning: If the total number of timers is more than MAX_TIMER, this will return NULL!
 */ 
struct TIMER *allocTimer();

/**
 * @brief: Free a timer. NOTNULL!
 * @param: (TIMER *)timer: The timer to get freed.
 * @warning: The param timer should NOT be NULL!
 */ 
void freeTimer(struct TIMER *timer);

/**
 * @brief: Init a timer with given allocated data.
 * @param: (TIMER *)timer: The timer to init.
 * @param: (FIFO32 *)fifo: The fifo buffer alloc for timer.
 * @param: (int)data: Data to init.
 */ 
void initTimer(struct TIMER *timer, struct FIFOBuffer32 *fifo, int data);

/**
 * @brief: Set timeout for a timer.
 * @param: (TIMER *)timer: The timer to set timeout. NOTNULL
 * @param: (uint)timeout: The timeout ti set.
 * @warning: The param timer should NOT be NULL!
 */ 
void timerSetTimeOut(struct TIMER *timer, unsigned int timeout);

/**
 * @brief: Handles IRQs from Timer.
 * @param: (int *)esp: ESP register.
 * @seealso: inthandler20
 */ 
void timerInterruptHandler(int *esp);

/**
 * @brief: Cancel a timer when it is counting.
 * @param: (TIMER *)timer: The timer to cancel.
 * @return: (int) 1 if succeed, 0 otherwise.
 */ 
int cancelTimer(struct TIMER *timer);

/**
 * @brief: Cancel all timers.
 * @param: (FIFO32 *)fifo: Cancel the timers using the same buffer.
 * @warning: This will ONLY cancel timers with ownerStat = APPTIMER!
 */ 
void cancelAllTimers(struct FIFOBuffer32* fifo);

extern struct TIMER_MANAGER timerManager;

#endif

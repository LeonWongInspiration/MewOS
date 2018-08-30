/** Programmable Interval Timer (PIT) Management
 * 
 * @author: Leon Wong
 * @build: 201808310554
 * @brief: This file provides functions about PIT and timer.
 * @usage: This file should be both included and compiled.
 * 
 */ 

#if (!(defined(TIMER_H)))
#define TIMER_H 1

#include "asm_funcs.h"
#include "PIC.h"

/// Port help to control PIT
const static int PIT_CTRL = 0x0043;

/// Port help to tell PIT the frequency for timer.
const static int PIT_CNT0 = 0x0040;

/**
 * @brief: Init PIT with frequency 0x2e9c (0x2e9c = 11932 -> 100Hz)
 */ 
void initPIT();

/**
 * @brief: Handles IRQs from Timer.
 * @param: (int *)esp: ESP register.
 * @seealso: inthandler20
 */ 
void timerInterruptHandler(int *esp);

#endif

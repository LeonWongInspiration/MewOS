/** MewOS Console Window
 * 
 * @author: Leon Wong
 * @build: 201809061824
 * @brief: This file makes a console window which runs commands.
 * @usage: This file should be both included and compiled.
 * 
 */

#ifndef CONSOLE_H
#define CONSOLE_H 1

#include "Sheet.h"
#include "Timer.h"
#include "Task.h"
#include "FIFOBuffer.h"
#include "VisualFuncs.h"
#include "asm_funcs.h"
#include "Keyboard.h"
#include "Mouse.h"

/**
 * @brief: Manage the task of console.
 * @param: (SHEET *)sheeet: The sheet of the console window.
 */ 
void consoleTask(struct SHEET *sheet);

#endif

/** MewOS Console Window
 * 
 * @author: Leon Wong
 * @build: 201809081627
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
#include "FAT.h"
#include "Structs.h"
#include "Window.h"
#include "Memorie.h"

#include "include/stdio.h"
#include "include/string.h"

// ------ Below are console management ------ //
/**
 * @brief: Manage the task of console.
 * @param: (SHEET *)sheeet: The sheet of the console window.
 * @param: (uint)totalMemory: The size of the memory.
 */ 
void consoleTask(struct SHEET *sheet, unsigned int totalMemory);

/**
 * @brief: Show a char onto the console.
 * @param: (CONSOLE *)cons: The console to show char on.
 * @param: (int)chr: The character to show.
 * @param: (char)move: Whether you want to move the cursor after showing the char.
 */ 
void consolePutChar(struct CONSOLE *cons, int chr, char move);

/**
 * @brief: Create a new line on the console.
 * @param: (CONSOLE *)cons: The console to show a new line.
 */ 
void consoleNewLine(struct CONSOLE *cons);

/**
 * @brief: Show a string onto the console.
 * @param: (CONSOLE *)cons: The console to show string on.
 * @param: (char *)s: The string to show.
 * @warning: The string should be a C-Style string who ends with a '\0'!
 */ 
void consoleWrite(struct CONSOLE *cons, char *s);

/**
 * @brief: Show a string according to its length.
 * @param: (CONSOLE *)cons: The console to show string.
 * @param: (char *)s: The string to show.
 * @param: (int)l: The length you want to show on the console.
 */ 
void consoleWriteLen(struct CONSOLE *cons, char *s, int l);

/**
 * @brief: This function receives a cmd and some params to run cmds.
 * @param: (char *)cmd: The cmd to run.
 * @param: (CONSOLE *)cons: The console we put the output to.
 * @param: (int *)fat: The FAT table.
 * @param: (uint)totalMemory: The size of the memory.
 */ 
void consoleRunCmd(char *cmd, struct CONSOLE *cons, int *fat, unsigned int totalMemory);

// ------ Below are commands we run in the console ------ //
/**
 * @brief: "mem" cmd shows info about memory onto the console.
 * @param: (CONSOLE *)cons: The console we run mem cmd.
 * @param: (uint)totalMemory: The size of the memory.
 */ 
void cmdMEM(struct CONSOLE *cons, unsigned int totalMemory);

/**
 * @brief: "clear" cmd clears the outputs in the console.
 * @param: (CONSOLE *)cons: The console.
 */ 
void cmdCLEAR(struct CONSOLE *cons);

/**
 * @brief: "ls" cmd shows the files in the directory and their info.
 * @param: (CONSOLE *)cons: The console.
 */ 
void cmdLS(struct CONSOLE *cons);

/**
 * @brief: "cat" cmd shows a file as document on the console.
 * @param: (CONSOLE *)cons: The console.
 * @param: (int *fat): The FAT table.
 * @param: (char *)cmd: The param of cat cmd.
 */ 
void cmdCAT(struct CONSOLE *cons, int *fat, char *cmd);

/**
 * @brief: "call" runs a third-party executable file in the console.
 * @param: (CONSOLE *)cons: The console.
 * @param: (int *fat): The fat table.
 * @param: (char *)cmd: The command to run.
 * @return: (int)0 if failed, 1 otherwise.
 */ 
int cmdCALL(struct CONSOLE *cons, int *fat, char *cmd);

/**
 * @brief: MewOS API call.
 * @param: (int)registers.
 * @return: (int *): The address of the task for some API calls.
 */ 
int *mew_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);

/**
 * @brief: Handle Stack Exception.
 * @param: (int *)esp: ESP register.
 * @return: (int *)The esp of the task.
 */ 
int *stackExceptionHandler(int *esp);

/**
 * @brief: Handle General Protected Exception.
 * @param: (int *)esp: ESP register.
 * @return: (int *)The esp of the task.
 */ 
int *generalProtectedExceptionHandler(int *esp);

/**
 * @brief: Draw a line on the window.
 * @param: (SHEEt *)SHT: The sheet to draw a line on.
 * @param: (int)x0: X0 position of the line.
 * @param: (int)y0: Y0 position of the line.
 * @param: (int)x1: X1 position of the line.
 * @param: (int)y1: Y1 position of the line.
 * @param: (int)col: Color of the line.
 */ 
void mew_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);

#endif

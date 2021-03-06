/** Programmable Interrupt Controller (PIC) Management
 * 
 * @author: Leon Wong
 * @build: 201808310603
 * @brief: This file includes some functions controlling PIC.
 * @usage: This file shoule be both included and compiled.
 * 
 */ 

#ifndef PIC_H
#define PIC_H 1

#include "asm_funcs.h"
#include "BootInfo.h"
#include "VisualFuncs.h"
#include "Fonts.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"
#include "Console.h"

/// Relative addresses
const static int PIC0_ICW1 = 0x0020; /// ICW: Initial Control Word
const static int PIC0_OCW2 = 0x0020;
const static int PIC0_IMR = 0x0021;
const static int PIC0_ICW2 = 0x0021;
const static int PIC0_ICW3 = 0x0021;
const static int PIC0_ICW4 = 0x0021;
const static int PIC1_ICW1 = 0x00a0;
const static int PIC1_OCW2 = 0x00a0;
const static int PIC1_IMR = 0x00a1;
const static int PIC1_ICW2 = 0x00a1;
const static int PIC1_ICW3 = 0x00a1;
const static int PIC1_ICW4 = 0x00a1;

/**
 * @brief: This function inits PIC.
 */ 
void initPIC();

/**
 * @brief: Handle interrupts when stack throws exceptions.
 * @param: (int *)esp: ESP register.
 * @return: (int *)The esp0 of the stack who throws the exception.
 */ 
int *inthandler0c(int *esp);

/**
 * @brief: Handle interrupts for general protected exceptions.
 * @param: (int *)eap: ESP register.
 * @return (int *): The ESP of the task who throws the exception.
 */ 
int *inthandler0d(int *esp);

/**
 * @brief: Handle interrupts from timer.
 * @param: (int *)esp: ESP register.
 * @seealso: timerInterruptHandler.
 */ 
void inthandler20(int *esp);

/**
 * @brief: Receive interrupts from keyboard and call functions in keyboard.c to handle it.
 * @param: int *esp: ESP Register.
 * @seealso: keyboardInterruptHandler.
 */ 
void inthandler21(int *esp);

/**
 * @brief: Handle interrupts from mouse PS/2.
 * @param: int *esp: ESP Register.
 * @seealso: mouseInterruptHandler.
 */ 
void inthandler2c(int *esp);

/**
 * @brief: For some models, initing PIC may create a IRQ7 interrupt, this function deals this.
 * @param: int *eap: ESP Register.
 */ 
void inthandler27(int *esp);

#endif

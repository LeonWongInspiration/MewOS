/** Programmable Interrupt Controller (PIC) Management
 * 
 * @author: Leon Wong
 * @build: 201808270234
 * @brief: This file includes some functions controlling PIC.
 * @usage: This file shoule be both included and compiled.
 * 
 */ 

#if (!defined(PIC_H))
#define PIC_H 1

#include "asm_funcs.h"
#include "BootInfo.h"
#include "VisualFuncs.h"
#include "Fonts.h"

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
 * @brief: Handle interrupts from keyboard PS/2.
 * @param: int *esp: ESP Register.
 */ 
void inthandler21(int *esp);

/**
 * @brief: Handle interrupts from mouse PS/2.
 * @param: int *esp: ESP Register.
 */ 
void inthandler2c(int *esp);

/**
 * @brief: For some models, initing PIC may create a IRQ7 interrupt, this function deals this.
 * @param: int *eap: ESP Register.
 */ 
void inthandler27(int *esp);

#endif

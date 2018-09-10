/** Interrupt Descriptor Table (IDT) Management
 * 
 * @author: Leon Wong
 * @build: 201808310608
 * @brief: This file includes some functions about IDT.
 * @usage: This file should be both included and compiled.
 * 
 */

#ifndef IDT_H
#define IDT_H 1

#include "asm_funcs.h"
#include "Structs.h"

/// Access Right for IDT, which means Allow Interrupts
const static int AR_INTGATE32 = 0x008e;

/// IDT Address got from asm files.
const static void* IDTAddress = (void *) 0x0026f800;

/**
 * @brief: init IDT.
 * @seealso: setIDT.
 */ 
void initIDT();

/**
 * @brief: Set IDT.
 * @param: (GATE_DESCRIPTOR *)gd: The gate descriptor to register.
 * @param: (int)offset: Offset in the GD table.
 * @param: (int)selector: = Segment: difine what to run.
 * @param: (int)ar: Access right.
 */ 
void setIDT(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#endif

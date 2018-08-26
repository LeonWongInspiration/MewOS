/** Interrupt Descriptor Table (IDT) Management
 * 
 * @author: Leon Wong
 * @build: 201808270304
 * @brief: This file includes some functions about IDT.
 * @usage: This file should be both included and compiled.
 * 
 */

#if (!defined(IDT_H))
#define IDT_H 1

#include "asm_funcs.h"

/// Access Right for IDT, which means Allow Interrupts
const static int AR_INTGATE32 = 0x008e;

/**
 * 8 byte IDT info.
 */ 
typedef struct GATE_DESCRIPTOR {
	short offset_low;
    short selector;
	char dw_count;
    char access_right;
	short offset_high;
} GATE_DESCRIPTOR;

/// IDT Address got from asm files.
const static void* IDTAddress = (void *) 0x0026f800;

/**
 * @brief: init IDT.
 * @seealso: setIDT.
 */ 
void initIDT();

/**
 * @brief: Set IDT.
 */ 
void setIDT(GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

#endif

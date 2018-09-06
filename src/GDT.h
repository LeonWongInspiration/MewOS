/** Global (Segment) Descriptor Table (GDT) Management
 * 
 * @author: Leon Wong
 * @build: 201808270308
 * @brief: This file includes some functions about GDT.
 * @usage: This file should be both included and compiled.
 * 
 */

#ifndef GDT_H
#define GDT_H 1

#include "asm_funcs.h"
#include "Structs.h"

/// Address of all C system program.
const static int ADR_C_ALL = 0x00280000;

/// Limit System Segment for C system files.
const static int LIMIT_C_ALL = 0x0007ffff;

/// Access Right
const static int AR_DATA32_RW = 0x4092;

/// Access Right
const static int AR_CODE32_ER = 0x409a;

/// GDT Address got from asm files.
const static void* GDTAddress = (void *) 0x00270000;

/**
 * @brief: init GDT.
 * @seealso: setGDT.
 */ 
void initGDT();

/**
 * @brief: Set GDT.
 * @param: (SEGMENT_DESCRIPTOR *)sd: The GD to register.
 * @param: (uint) limit: The limit of segments (upper).
 * @param: (int)base: The limit of segments (lower).
 * @param: (int)ar: Access right.
 */ 
void setGDT(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);

#endif

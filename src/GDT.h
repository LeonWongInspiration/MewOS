/** Global (Segment) Descriptor Table (GDT) Management
 * 
 * @author: Leon Wong
 * @build: 201808270308
 * @brief: This file includes some functions about GDT.
 * @usage: This file should be both included and compiled.
 * 
 */

#if (!defined(GDT_H))
#define GDT_H 1

#include "asm_funcs.h"

/// Address of all C system program.
const static int ADR_C_ALL = 0x00280000;

/// Limit System Segment for C system files.
const static int LIMIT_C_ALL = 0x0007ffff;

/// Access Right
const static int AR_DATA32_RW = 0x4092;

/// Access Right
const static int AR_CODE32_ER = 0x409a;

/**
 * 8 byte GDT info
 */ 
typedef struct SEGMENT_DESCRIPTOR{
    short limit_low;
    short base_low;
    char base_mid;
    char access_right;
    char limit_high;
    char base_high;
} SEGMENT_DESCRIPTOR;

/// GDT Address got from asm files.
const static void* GDTAddress = (void *) 0x00270000;

/**
 * @brief: init GDT.
 * @seealso: setGDT.
 */ 
void initGDT();

/**
 * @brief: Set GDT.
 */ 
void setGDT(SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);

#endif

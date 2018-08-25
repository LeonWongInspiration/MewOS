/** Global (Segment) Descriptor Table (GDT) Management
 * 
 * @author: Leon Wong
 * @build: 201808260303
 * @brief: This file includes some functions about GDT.
 * @usage: This file should be both included and compiled.
 * 
 */

#if (!defined(GDT_H))
#define GDT_H 1

#include "asm_funcs.h"

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
const static void* GDTAddress = 0x00270000;

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

/** Memory Management
 * 
 * @author: Leon Wong
 * @build: 201808300421
 * @brief: This file contains functions of memory management. To avoid corruption with memory.h of C, we name it memorie instead.
 * @usage: This file should be both included and compiled.
 *  
 */ 

#if (!defined(MEMORIE_H))
#define MEMORIE_H 1

#include "asm_funcs.h"

/// If the 18th bit of eflags can be set to 1, the CPU is 486 or higher, 386 otherwise.
const static int EFLAGS_AC_BIT = 0x00040000;

/// Flag to disable cache
const static int CR0_CACHE_DISABLE = 0x60000000;

/**
 * @brief: Test if the CPU is 386 or 486&higher and config the memory accordingly.
 * @param: (uint) start: The start address of memory to test.
 * @param: (uint) end: The end address of memory to test.
 * @return (uint) Result: the available memory space.
 * @seealso: memtest_sub.
 */ 
unsigned int memtest(unsigned int start, unsigned int end);

#endif

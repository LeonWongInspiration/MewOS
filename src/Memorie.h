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

/// The memory address of the memory management program.
const static int MEMORY_MANAGER_ADDR = 0x003c0000;

//------ Below are about memory management ------//

/// Nodes of memory
#define MEMORY_FREE_NODE_NUMBER 4090 /// This is about 32KB.

/// A node saving available memory info.
typedef struct MEMORY_FREE_NODE {
    unsigned int address; /// Address of the memory.
    unsigned int size; /// Size of the memory.
} MEMORY_FREE_NODE;

/// A table containing available memory info.
typedef struct MEMORY_FREE_TABLE {
    int frees; /// Number of available infos.
    int maxFrees; /// Number of max frees.
    int lostSize; /// Size of unnoted memory.
    int losts; /// Number of losts.
    MEMORY_FREE_NODE table[MEMORY_FREE_NODE_NUMBER];
} MEMORY_FREE_TABLE;

/**
 * @brief: Init a memory table.
 * @param: (MEMORY_FREE_TABLE *)memoryFreeTable: The table to init.
 */ 
void memoryFreeTableInit(MEMORY_FREE_TABLE *memoryFreeTable);

/**
 * @brief: Given a table, sum up free memory spaces.
 * @param: (MEMORY_FREE_TABLE *)memoryFreeTable: The table to calculate.
 * @return (uint): Total space of available memory.
 */ 
unsigned int getAvailableMemorySpace(MEMORY_FREE_TABLE *memoryFreeTable);

/**
 * @brief: Alloc space for memory.
 * @param: (MEMORY_FREE_TABLE *)memoryFreeTable: The table to alloc.
 * @param: (uint) size: The size to get allocated.
 * @return (uint): The address of allocated memory, 0 if failed.
 * @seealso: allocMemoryForSize_Page
 */ 
unsigned int allocMemoryForSize(MEMORY_FREE_TABLE *man, unsigned int size);

/**
 * @brief: Free a node's memory with size and address.
 * @param: (MEMORY_FREE_TABLE *)memoryFreeTable: The table to free.
 * @param: (uint)addr: Address of the memory to free.
 * @param: (uint)size: The size of the memory.
 * @return (int) 0 if succeed, -1 otherwise.
 * @seealso: freeMemoryWithAddrAndSize_Page
 */ 
int freeMemoryWithAddrAndSize(MEMORY_FREE_TABLE *man, unsigned int addr, unsigned int size);

/**
 * @brief: Alloc memory with adress aligned to 4k (Page)
 * @param: (MEMORY_FREE_TABLE *)memoryFreeTable: The table to alloc.
 * @param: (uint) size: The size to get allocated.
 * @return (uint): The address of allocated memory, 0 if failed.
 * @seealso: allocMemoryForSize
 */ 
unsigned int allocMemoryForSize_Page(MEMORY_FREE_TABLE *man, unsigned int size);

/**
 * @brief: Free memory with address aligned to 4k (Page)
 * @param: (MEMORY_FREE_TABLE *)memoryFreeTable: The table to free.
 * @param: (uint)addr: Address of the memory to free.
 * @param: (uint)size: The size of the memory.
 * @return (int) 0 if succeed, -1 otherwise.
 * @seealso: freeMemoryWithAddrAndSize
 */ 
int freeMemoryWithAddrAndSize_Page(MEMORY_FREE_TABLE *man, unsigned int addr, unsigned int size);

//------ Below are about memory test ------//

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

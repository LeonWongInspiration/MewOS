#include "Memorie.h"

unsigned int memtest(unsigned int start, unsigned int end){
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	// Check whether the CPU is 486 higher.
	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; // AC-bit = 1
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) { // If the CPU is 486+, the AC_BIT can be set to 1.
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; // AC-bit = 0
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE; // Disable cache to check memory.
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE; // Enable cache.
		store_cr0(cr0);
	}

	return i;
}

void memoryFreeTableInit(MEMORY_FREE_TABLE *memoryFreeTable){
    memoryFreeTable->frees = 0;
    memoryFreeTable->losts = 0;
    memoryFreeTable->lostSize = 0;
    memoryFreeTable->maxFrees = 0;
}

unsigned int getAvailableMemorySpace(MEMORY_FREE_TABLE *memoryFreeTable){
    unsigned int i;
    unsigned int sum = 0;
    for (i = 0; i < memoryFreeTable->frees; ++i){
        sum += memoryFreeTable->table[i].size;
    }
    return sum;
}

unsigned int allocMemoryForSize(MEMORY_FREE_TABLE *man, unsigned int size){
    unsigned int i, a;
	for (i = 0; i < man->frees; i++) {
		if (man->table[i].size >= size) {
            // If there's a memory big enough.
			a = man->table[i].address;
			man->table[i].address += size;
			man->table[i].size -= size;
			if (man->table[i].size == 0) {
                // If a node's size is 0, del it.
				man->frees--;
				for (; i < man->frees; i++) {
					man->table[i] = man->table[i + 1];
				}
			}
			return a;
		}
	}
    return 0;
}

int freeMemoryWithAddrAndSize(MEMORY_FREE_TABLE *man, unsigned int addr, unsigned int size){
    int i, j;
    // For the table is sorted, we only need to find the least addr
    // which is bigger than the given one.
	for (i = 0; i < man->frees; i++) {
		if (man->table[i].address > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		// There are spaces before.
		if (man->table[i - 1].address + man->table[i - 1].size == addr) {
            // Merge them
			man->table[i - 1].size += size;
			if (i < man->frees) {
				// There are spaces after.
				if (addr + size == man->table[i].address) {
					// Merge them.
					man->table[i - 1].size += man->table[i].size;
                    // Del useless nodes.
					man->frees--;
					for (; i < man->frees; i++) {
						man->table[i] = man->table[i + 1];
					}
				}
			}
			return 0; // Success
		}
	}
	// No spaces before.
	if (i < man->frees) {
		// Spaces available after.
		if (addr + size == man->table[i].address) {
			// Merge them
			man->table[i].address = addr;
			man->table[i].size += size;
			return 0; // Success
		}
	}
	// No spaces arround
	if (man->frees < MEMORY_FREE_NODE_NUMBER) {
        // Create a node
		for (j = man->frees; j > i; j--) {
			man->table[j] = man->table[j - 1];
		}
		man->frees++;
		if (man->maxFrees < man->frees) {
			man->maxFrees = man->frees; // Update the length of the table.
		}
		man->table[i].address = addr;
		man->table[i].size = size;
		return 0; // Success
	}
	// The table is full!
	man->losts++;
	man->lostSize += size;
	return -1; // Failed. :(
}

unsigned int allocMemoryForSize_Page(MEMORY_FREE_TABLE *man, unsigned int size){
    return allocMemoryForSize(man, (size + 0xfff) & 0xfffff000);
}

int freeMemoryWithAddrAndSize_Page(MEMORY_FREE_TABLE *man, unsigned int addr, unsigned int size){
    return freeMemoryWithAddrAndSize(man, addr, (size + 0xfff) & 0xfffff000);
}

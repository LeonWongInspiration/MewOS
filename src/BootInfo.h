/** Boot Info Struct Defination
 * 
 * @author: Leon Wong
 * @build: 201808250330
 * @brief: This file is the defination of BootInfo struct, which saves the boot info from the memory.
 * @usage: This file should be included and there's no need to compile it.
 * 
 */

#if (!defined(BOOT_INFO_H))
#define BOOT_INFO_H 1

const static int ADR_BOOTINFO = 0x00000ff0;

struct BootInfo {
	char cyls; // Cylinders
	char leds; // Led mode
	char vmode; // Visual mode
	char reserve;
	short scrnx; // Resolution X
	short scrny; // Resolution Y
	char *vram; // Address of VRAM
};

typedef struct BootInfo BootInfo;

#endif

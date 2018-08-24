#include "VisualFuncs.h"
#include "BootInfo.h"
#include "Fonts.h"
#include "asm_funcs.h"

void MewOSMain(){
    BootInfo *binfo = (BootInfo *) 0x0ff0;

	init_palette();
	init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
	putfonts8_asc(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF, "MewOS 0.0.2.0");
	putfonts8_asc(binfo->vram, binfo->scrnx, 31, 31, COL8_000000, "hello, world!");
	putfonts8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "Fonts demo");

	while (1){
		io_hlt();
	}
}

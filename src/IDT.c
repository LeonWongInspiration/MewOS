#include "IDT.h"

void initIDT(){
    GATE_DESCRIPTOR *idt = (GATE_DESCRIPTOR *)IDTAddress;
    int i;
    for (i = 0; i < 256; ++i){
        setIDT(idt + i, 0, 0, 0);
    }
    load_idtr(0x7ff, (int)IDTAddress);

	setIDT(idt + 0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);
	setIDT(idt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);
	setIDT(idt + 0x27, (int) asm_inthandler27, 2 * 8, AR_INTGATE32);
	setIDT(idt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);

	// ------ Register MewOS System Call API ------ //
	setIDT(idt + 0x40, (int) asm_mew_api, 2 * 8, AR_INTGATE32);
}

void setIDT(GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
	gd->offset_low = offset & 0xffff;
	gd->selector = selector;
	gd->dw_count = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high = (offset >> 16) & 0xffff;    
}
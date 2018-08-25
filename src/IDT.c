#include "IDT.h"

void initIDT(){
    GATE_DESCRIPTOR *idt = (GATE_DESCRIPTOR *)IDTAddress;
    int i;
    for (i = 0; i < 256; ++i){
        setIDT(idt + i, 0, 0, 0);
    }
    load_idtr(0x7ff, (int)IDTAddress);
}

void setIDT(GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
	gd->offset_low = offset & 0xffff;
	gd->selector = selector;
	gd->dw_count = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high = (offset >> 16) & 0xffff;    
}
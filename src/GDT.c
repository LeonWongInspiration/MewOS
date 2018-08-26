#include "GDT.h"

void initGDT(){
    SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR *)GDTAddress;
    int i;
    for (i = 0; i < 8192; ++i){
        setGDT(gdt + i, 0, 0, 0);
    }
	setGDT(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
	setGDT(gdt + 2, LIMIT_C_ALL, ADR_C_ALL, AR_CODE32_ER);
	load_gdtr(0xffff, (int)GDTAddress);
}

void setGDT(SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){
    if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low = limit & 0xffff;
	sd->base_low = base & 0xffff;
	sd->base_mid = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high = (base >> 24) & 0xff;
}
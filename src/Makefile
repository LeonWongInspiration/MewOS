TOOLPATH = ../tools/
INCLUDE_PATH = ../src/include/

MAKE = make -r
NASK = $(TOOLPATH)nask
CC1 = $(TOOLPATH)cc1 -I$(INCPATH) -Os -Wall -quiet
EDIMG = $(TOOLPATH)edimg
GAS2NASK = $(TOOLPATH)gas2nask -a
OBJ2BIM  = $(TOOLPATH)obj2bim
BIM2HRB  = $(TOOLPATH)bim2hrb
RULEFILE = $(TOOLPATH)mewos.rul
IPL_NAME = ipl10
COPY = copy
DEL = del

default :
	make img

$(IPL_NAME).bin : $(IPL_NAME).nas Makefile
	$(NASK) $(IPL_NAME).nas $(IPL_NAME).bin $(IPL_NAME).lst

MewOS.bin : MewOS.nas Makefile
	$(NASK) MewOS.nas MewOS.bin MewOS.lst

C_Entry.gas : C_Entry.c Makefile
	$(CC1) -o C_Entry.gas C_Entry.c

C_Entry.nas : C_Entry.gas Makefile
	$(GAS2NASK) C_Entry.gas C_Entry.nas

C_Entry.obj : C_Entry.nas Makefile
	$(NASK) C_Entry.nas C_Entry.obj C_Entry.lst

asm_funcs.obj : asm_funcs.nas Makefile
	$(NASK) asm_funcs.nas asm_funcs.obj asm_funcs.lst

C_Entry.bim : C_Entry.obj asm_funcs.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:C_Entry.bim stack:3136k \
	 map:C_Entry.map C_Entry.obj asm_funcs.obj
	
C_Entry.hrb : C_Entry.bim Makefile
	$(BIM2HRB) C_Entry.bim C_Entry.hrb 0

MewOS.sys : MewOS.bin C_Entry.hrb Makefile
	$(COPY) /B MewOS.bin+C_Entry.hrb MewOS.sys

MewOS.img : $(IPL_NAME).bin MewOS.sys Makefile
	$(EDIMG) imgin:../tools/fdimg0at.tek \
	wbinimg src:$(IPL_NAME).bin len:512 from:0 to:0 \
	copy from:MewOS.sys to:@: \
	imgout:MewOS.img

img :
	$(MAKE) MewOS.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) C_Entry.nas
	-$(DEL) C_Entry.map
	-$(DEL) C_Entry.bim
	-$(DEL) C_Entry.hrb
	-$(DEL) MewOS.sys

src_clean :
	-$(DEL) MewOS.img
	make clean
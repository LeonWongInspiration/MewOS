TOOLPATH = ../tools/
INCLUDE_PATH = ../src/include/

OBJ_FILES = C_Entry.obj asm_funcs.obj MewFont.obj Fonts.obj \
 VisualFuncs.obj Mouse.obj GDT.obj IDT.obj PIC.obj \
 Keyboard.obj FIFOBuffer.obj Memorie.obj Sheet.obj \
 Window.obj Timer.obj Widgets.obj Task.obj Console.obj \
 FAT.obj

MAKE = make -r
NASK = $(TOOLPATH)nask
CC1 = $(TOOLPATH)cc1 -I$(INCPATH) -Os -Wall -quiet
EDIMG = $(TOOLPATH)edimg
GAS2NASK = $(TOOLPATH)gas2nask -a
BIN2OBJ = $(TOOLPATH)bin2obj.exe
MAKEFONT = $(TOOLPATH)makefont.exe
OBJ2BIM = $(TOOLPATH)obj2bim
BIM2HRB = $(TOOLPATH)bim2hrb
RULEFILE = $(TOOLPATH)mewos.rul
HARIRULE = $(TOOLPATH)harirul.rul
IPL_NAME = ipl10
COPY = copy
DEL = del

default :
	make img

# Compile asm files for binary files

$(IPL_NAME).bin : $(IPL_NAME).nas Makefile
	$(NASK) $(IPL_NAME).nas $(IPL_NAME).bin $(IPL_NAME).lst

MewOS.bin : MewOS.nas Makefile
	$(NASK) MewOS.nas MewOS.bin MewOS.lst

# Declaration for C files

MewFont.bin : MewFont.txt Makefile
	$(MAKEFONT) MewFont.txt MewFont.bin

MewFont.obj : MewFont.bin Makefile
	$(BIN2OBJ) MewFont.bin MewFont.obj _MewFont

%.gas : %.c Makefile
	$(CC1) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# Now we have made all c files into obj files
# Link them together

C_Entry.bim : $(OBJ_FILES) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:C_Entry.bim stack:3136k \
	map:C_Entry.map $(OBJ_FILES)
	
C_Entry.hrb : C_Entry.bim Makefile
	$(BIM2HRB) C_Entry.bim C_Entry.hrb 0

winhelo.bim : winhelo.obj API.obj Makefile
	$(OBJ2BIM) @$(HARIRULE) out:winhelo.bim stack:1k map:winhelo.map \
	winhelo.obj API.obj

winhelo.hrb : winhelo.bim Makefile
	$(BIM2HRB) winhelo.bim winhelo.hrb 0

MewOS.sys : MewOS.bin C_Entry.hrb Makefile
	$(COPY) /B MewOS.bin+C_Entry.hrb MewOS.sys

MewOS.img : $(IPL_NAME).bin MewOS.sys winhelo.hrb Makefile
	$(EDIMG) imgin:../tools/fdimg0at.tek \
	wbinimg src:$(IPL_NAME).bin len:512 from:0 to:0 \
	copy from:MewOS.sys to:@: \
	copy from:$(IPL_NAME).nas to:@: \
	copy from:winhelo.hrb to:@: \
	imgout:MewOS.img

img :
	$(MAKE) MewOS.img

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) Fonts.nas
	-$(DEL) VisualFuncs.nas
	-$(DEL) Mouse.nas
	-$(DEL) C_Entry.nas
	-$(DEL) GDT.nas
	-$(DEL) IDT.nas
	-$(DEL) PIC.nas
	-$(DEL) C_Entry.map
	-$(DEL) C_Entry.bim
	-$(DEL) C_Entry.hrb
	-$(DEL) winhelo.map
	-$(DEL) winhelo.bim
	-$(DEL) winhelo.hrb
	-$(DEL) MewOS.sys

src_clean :
	-$(DEL) MewOS.img
	make clean
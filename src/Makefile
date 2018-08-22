TOOLPATH = ../tools/
MAKE = make -r
NASK = $(TOOLPATH)/nask
EDIMG = $(TOOLPATH)/edimg
COPY = cp
DEL = rm

default :
	make img

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

MewOS.img : ipl.bin Makefile
	$(EDIMG) imgin:../tools/fdimg0at.tek \
	wbinimg src:ipl.bin len:512 from:0 to:0 imgout:MewOS.img

img :
	$(MAKE) MewOS.img

clean :
	-$(DEL) ipl.bin
	-$(DEL) ipl.lst

src_clean :
	-$(DEL) MewOS.img
	make clean
TOOLPATH = ../tools/
MAKE = make -r
NASK = $(TOOLPATH)nask
EDIMG = $(TOOLPATH)edimg
IPL_NAME = ipl10
COPY = cp
DEL = rm

default :
	make img

$(IPL_NAME).bin : $(IPL_NAME).nas Makefile
	$(NASK) $(IPL_NAME).nas $(IPL_NAME).bin $(IPL_NAME).lst

MewOS.sys : MewOS.nas Makefile
	$(NASK) MewOS.nas MewOS.sys MewOS.lst

MewOS.img : $(IPL_NAME).bin MewOS.sys Makefile
	$(EDIMG) imgin:../tools/fdimg0at.tek \
	wbinimg src:$(IPL_NAME).bin len:512 from:0 to:0 \
	copy from:MewOS.sys to:@: \
	imgout:MewOS.img

img :
	$(MAKE) MewOS.img

clean :
	-$(DEL) $(IPL_NAME).bin
	-$(DEL) $(IPL_NAME).lst
	-$(DEL) MewOS.sys
	-$(DEL) MewOS.lst

src_clean :
	-$(DEL) MewOS.img
	make clean
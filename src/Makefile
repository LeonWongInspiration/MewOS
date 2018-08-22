default :
	make img

ipl.bin : ipl.nas Makefile
	../tools/nask ipl.nas ipl.bin ipl.lst

MewOS.img : ipl.bin Makefile
	../tools/edimg imgin:../tools/fdimg0at.tek \
	wbinimg src:ipl.bin len:512 from:0 to:0 imgout:MewOS.img

img :
	make -r MewOS.img

clean :
	rm ipl.bin
	rm ipl.lst

src_clean :
	rm MewOS.img
	make clean
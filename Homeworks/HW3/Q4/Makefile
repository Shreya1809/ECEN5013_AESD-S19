#Reference: Kernel Module Lecture Slide for Host
#obj-m +=shreya_timerMod.o
#all:
#	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
#clean:
#	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean 	

#Reference MELP compiling Kernel Modules
LINUXDIR := ~/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220

obj-m := data_structMod.o

all:
	make ARCH=arm CROSS_COMPILE=~/buildroot/output/host/usr/bin/arm-linux- -C $(LINUXDIR) M=$(shell pwd)
clean:
	make -C $(LINUXDIR) M=$(shell pwd) clean
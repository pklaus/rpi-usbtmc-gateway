obj-m := usbtmc.o


usbtmc.ko: usbtmc.c
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules


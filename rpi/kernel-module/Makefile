obj-m := usbtmc.o


usbtmc.ko: usbtmc.c
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	-rm usbtmc.ko usbtmc.o usbtmc.mod.c Module.symvers usbtmc.mod.o modules.order

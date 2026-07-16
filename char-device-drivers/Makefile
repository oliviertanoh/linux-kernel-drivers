obj-m += chardev.o

KDIR := /home/otanoh/repo/linux

all :
	make -C $(KDIR) M=$(PWD) modules

clean :
	make -C $(KDIR) M=$(PWD) clean
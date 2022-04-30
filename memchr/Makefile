CONFIG_MODULE_SIG = n
TARGET_MODULE := memchr_drv

obj-m := $(TARGET_MODULE).o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)


all: client
	$(MAKE) -C $(KDIR) M=$(PWD) modules
load:
	sudo insmod $(TARGET_MODULE).ko
unload:
	sudo rmmod $(TARGET_MODULE) || true >/dev/null
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) client out

client: client.c
	$(CC) -o $@ $^
PREFIX = linux_memchr
POSTFIX = .txt
plot:all
	$(MAKE) unload
	$(MAKE) load
	for i in 1 2 3 4 5 6 7 8 9 10; do\
		sudo taskset 0x01 ./client > data/${PREFIX}/${PREFIX}$$i${POSTFIX};\
	done
	$(MAKE) unload
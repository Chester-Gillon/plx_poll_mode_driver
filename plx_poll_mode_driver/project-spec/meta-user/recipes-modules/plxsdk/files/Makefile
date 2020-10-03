obj-m := Plx9056.o
Plx9056-y := ApiFunc.o Dispatch.o Driver.o Eep_9000.o ModuleVersion.o PciFunc.o SuppFunc.o PlxChipApi.o \
             PlxChipFn.o PlxInterrupt.o

MY_CFLAGS += -g -DDEBUG -DPLX_LITTLE_ENDIAN -DPLX_CHIP=9056 -DPLX_DRIVER -DPLX_LINUX_DRIVER -DPLX_64BIT -DPLX_DEBUG
ccflags-y += ${MY_CFLAGS}

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

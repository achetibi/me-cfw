all:
	cp $(TARGET).prx ../vrmanager/modules/$(TARGET).prx
	psp-packer -i $(TARGET).prx
	psp-build-exports -s $(PRX_EXPORTS)
	bin2c $(TARGET).prx $(TARGET).h $(TARGET)
	
clean:
	rm -f *.o $(TARGET).prx $(TARGET).h $(TARGET).S

TARGET = kpspident
OBJS = main.o pspdecrypt.o libpsardumper.o exports.o

INCDIR = ../include
CFLAGS = -Os -G0
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LIBDIR = ../lib
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspnand_driver -lpspumdman_driver -lpspwlan -lpspnet_adhoc -lpspnet_adhocctl -lpspnet -lpsputility -lpspsemaphore -lpspmesgd_driver -lpspnwman_driver -lpsputilsforkernel -lpspsystemctrl_kernel -lpspkernel -lpspreg

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
release: all
	psp-packer -d $(TARGET_FW) -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET)_bin.h $(TARGET)
	sed -i "/int/d" $(TARGET)_bin.h
	rm -f *.elf *.o

clean: reset
reset:
	rm -f *.elf *.o $(TARGET).prx $(TARGET)_bin.h

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION=$(TARGET_FW)


TARGET = usbdevice
OBJS = main.o exports.o

INCDIR = ../include
CFLAGS = -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1


LIBDIR = ../lib
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsystemctrl_kernel -lpspumd_driver

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

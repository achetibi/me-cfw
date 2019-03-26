release: all
	psp-build-exports -s exports.exp
	psp-packer -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET)_driver_bin.c $(TARGET)
	sed -i -e 's/((aligned(16)))/((aligned(64)))/' -e 's/static//g' $(TARGET)_driver_bin.c
#	sed -i 's/static//g' $(TARGET)_driver_bin.c
	rm -f *.elf *.o

clean: reset
reset:
	rm -f *.elf *.o $(TARGET).prx $(TARGET)_driver_bin.c

TARGET = plutonium
OBJS = main.o

CFLAGS = -Os -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

PSP_FW_VERSION = 150

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

INCDIR = ../../include

LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpsppower_driver -lpspsystemctrl_kernel -lpspmodulemgr_kernel
LIBDIR = ../../lib

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

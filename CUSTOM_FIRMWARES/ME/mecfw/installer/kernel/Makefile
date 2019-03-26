all:
	psp-build-exports -s $(PRX_EXPORTS)
	psp-packer -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET).h $(TARGET)
	sed -i 's/((aligned(16)))/((aligned(64)))/g' $(TARGET).h
	rm -f *.elf *.o

clean: reset
reset:
	rm -f *.elf *.o *.S $(TARGET).prx $(TARGET).h

TARGET = bridge
OBJS = main.o

INCDIR =
CFLAGS = -Os -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LIBDIR =
LDFLAGS = -mno-crt0 -nostartfiles
LIBS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

release: all
	psp-packer -d $(TARGET_FW) -t 0x4C94DAF0 -x -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET)_bin.h $(TARGET)
	sed -i "/int/d" $(TARGET)_bin.h
	rm -f *.elf *.o

clean: reset
reset:
	rm -f *.elf *.o $(TARGET).prx $(TARGET)_bin.h

ifndef $(TARGET_FW)
TARGET_FW=150
endif

TARGET = inferno
OBJS = main.o iodrv_funcs.o umd.o isoread.o inferno_patch_offset.o isocache.o imports.o

PSP_FW_VERSION=$(TARGET_FW)

INCDIR = ../include
CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -D_PSP_FW_VERSION=$(TARGET_FW)

LDFLAGS = -mno-crt0 -nostartfiles
LIBDIR = ../lib
LIBS = -lpspsystemctrl_kernel -lpspsemaphore

PRX_EXPORTS = exports$(TARGET_FW).exp

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

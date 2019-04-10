release: all
	psp-packer -t 0x4C9494F0 -x -d $(PSP_FW_VERSION) -i $(TARGET).prx
	bin2c $(TARGET).prx $(DATADIR)/$(TARGET)_bin.h $(TARGET)
	sed -i "/int/d" $(DATADIR)/$(TARGET)_bin.h
	rm -f *.elf *.o *.prx

clean: reset
reset:
	rm -f *.elf *.o $(TARGET).prx $(DATADIR)/$(TARGET)_bin.h

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION=$(TARGET_FW)

TARGET = tmctrl
OBJS = main.o flashemu.o exports$(TARGET_FW).o
#printk.o

INCDIR = ../../include
DATADIR=../installer/data
CFLAGS = -O2 -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division -fno-builtin
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports$(TARGET_FW).exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LDFLAGS += -mno-crt0 -nostartfiles
LIBDIR = ../../lib
LIBS = -lpspsystemctrl_kernel

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

release: all
	psp-packer -d $(TARGET_FW) -t 0x4C94DAF0 -x -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET)_bin.h $(TARGET)
	sed -i "/int/d" $(TARGET)_bin.h
	rm -f *.elf *.o

clean: reset
reset:
	rm -f *.elf *.o $(TARGET).prx $(TARGET)_bin.h

TARGET = pulsar
OBJS = galaxy.o galaxy_driver.o csoread.o daxread.o exports.o stub.o

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION=$(TARGET_FW)

INCDIR = ../include
CFLAGS = -Os -G0 -Wall -fno-pic -mno-check-zero-division
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LDFLAGS += -mno-crt0 -nostartfiles
LIBDIR = ../lib
LIBS = -lpsppower -lpspsystemctrl_kernel


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

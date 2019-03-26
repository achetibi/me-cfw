all:
	psp-packer -x -i $(TARGET).prx
	psp-build-exports -s $(PRX_EXPORTS)

TARGET = bridge
OBJS = main.o imports.o

INCDIR = ../../../include
CFLAGS = -Os -G0 -Wall -fno-pic

OBJS += $(IMPORTS)

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 620

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR =
LDFLAGS = -mno-crt0 -nostartfiles
LIBS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

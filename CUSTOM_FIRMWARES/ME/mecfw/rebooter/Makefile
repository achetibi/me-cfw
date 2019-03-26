release: all
	psp-packer -d $(TARGET_FW) -t 0x4C94DAF0 -x -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET)_bin.h $(TARGET)
#	sed -i "/int/d" $(TARGET)_bin.h
	sed -i "/int/c #if _PSP_FW_VERSION == $(TARGET_FW)" $(TARGET)_bin.h
	sed -i "/#endif/i #endif" $(TARGET)_bin.h
	rm -f *.elf *.o

clean: reset
reset:
	rm -f *.elf *.o $(TARGET).prx $(TARGET)_bin.h

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

ifndef $(PERMANENT)
PERMANENT = 0
endif

PSP_FW_VERSION=$(TARGET_FW)


ifeq ($(PSP_MODEL),0)
ifeq ($(PERMANENT),1)
TARGET = reboot_01g_p
else
TARGET = reboot_01g
endif
endif

ifeq ($(PSP_MODEL),1)
ifeq ($(PERMANENT),1)
TARGET = reboot_02g_p
else
TARGET = reboot_02g
endif
endif

#ifeq ($(PSP_MODEL),2)
#ifeq ($(PERMANENT),1)
#TARGET = reboot_03g_p
#else
#TARGET = reboot_03g
#endif
#endif

ifeq ($(PSP_MODEL),4)
ifeq ($(PERMANENT),1)
TARGET = reboot_05g_p
else
TARGET = reboot_05g
endif
endif

#TARGET = reboot63x

OBJS = main.o utils.o exports.o

BUILD_PRX=1
PRX_EXPORTS=exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

INCDIR = ../include
CFLAGS = -Os -G0 -Wall -DPSP_MODEL=$(PSP_MODEL) -DPERMANENT=$(PERMANENT)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LIBS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

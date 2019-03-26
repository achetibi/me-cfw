release: all
	psp-packer -d $(TARGET_FW) -t 0x4C94ACF0 -x -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET)_bin.h $(TARGET)
	sed -i "/int/d" $(TARGET)_bin.h
	rm -f *.elf *.o iso/*.o sfile/*.o

clean: reset
reset:
	rm -f *.elf *.o iso/*.o sfile/*.o $(TARGET).prx $(TARGET)_bin.h

ifndef $(PERMANENT)
PERMANENT = 0
endif
	
ifeq ($(PSP_MODEL),0)
ifeq ($(PERMANENT),1)
TARGET = systemctrl_01g_p
else
TARGET = systemctrl_01g
endif
endif

ifeq ($(PSP_MODEL),1)
ifeq ($(PERMANENT),1)
TARGET = systemctrl_02g_p
else
TARGET = systemctrl_02g
endif
endif
	
ifeq ($(PSP_MODEL),2)
ifeq ($(PERMANENT),1)
TARGET = systemctrl_03g_p
else
TARGET = systemctrl_03g
endif
endif

ifeq ($(PSP_MODEL),4)
ifeq ($(PERMANENT),1)
TARGET = systemctrl_05g_p
else
TARGET = systemctrl_05g
endif
endif

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION=$(TARGET_FW)

OBJS = main.o loadcore.o modulemgr.o utility.o syspatch.o nid_resolver.o malloc.o conf.o decryptcore_patch.o power.o exports.o
OBJS += ctrl_lib.o kubridge.o system_export.o ctrl_info.o ms_mod.o hook.o mempatch.o
OBJS += iso/isofs_driver.o iso/csoread.o iso/isoread.o iso/daxread.o iso/umd9660_driver.o iso/umd_patch.o umd9660_m33driver.o
OBJS += sfile/loadcore.o sfile/sysmem.o sfile/interruptmgr.o sfile/loadexec.o sfile/modulemgr.o sfile/sysmem_jump.o

INCDIR = ../include
CFLAGS = -Os -G0 -Wall  -fno-pic
# -mno-check-zero-division

CFLAGS += -DPSP_MODEL=$(PSP_MODEL) -DPERMANENT=$(PERMANENT)

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -D_PSP_FW_VERSION=$(TARGET_FW)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LIBDIR = ../lib
LIBS = -lpspsemaphore -lzm
LDFLAGS = -nostdlib -nodefaultlibs -e module_bootstart

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

#all:
#	psp-build-exports -s $(PRX_EXPORTS)
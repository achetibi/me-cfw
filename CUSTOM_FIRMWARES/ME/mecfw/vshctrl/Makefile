release: all
	psp-packer -d $(TARGET_FW) -x -t 0x4C94DAF0 -i $(TARGET).prx
	bin2c $(TARGET).prx $(BINFILE).h vshctrl
	sed -i "/int/d" $(BINFILE).h
	rm -f *.elf *.o

clean: reset
reset:
	rm -f *.elf *.o sfile/*.o $(TARGET).prx $(BINFILE).h

ifndef HEN
TARGET = vshctrl_02g
BINFILE = vshctrl_bin
else
TARGET = vshctrl_03g
BINFILE = vshctrl_lite_bin
endif

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION=$(TARGET_FW)

OBJS = main.o io_patch.o virtualpbpmgr.o isocache.o usbctrl.o vshctrl.o exports.o
OBJS += sfile/sysmem.o 
#sfile/systemctrl_vshctrl.o sfile/pspUsbDevice_driver.o

INCDIR = ../include
CFLAGS = -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

ifdef HEN
CFLAGS += -DHEN
endif

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LDFLAGS += -mno-crt0 -nostartfiles
LIBDIR = ../lib
LIBS = -lpsppower_driver -lpspusb_driver -lpsprtc_driver -lpspumd_driver -lpspusbdevice_driver -lpspsystemctrl_kernel
# -lpspopenpsid

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak



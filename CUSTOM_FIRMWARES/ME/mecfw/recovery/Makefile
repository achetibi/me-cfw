release: all
	psp-packer -d $(TARGET_FW) -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET)_bin.h $(TARGET)
	sed -i "/int/d" $(TARGET)_bin.h
	rm -f *.elf *.o sfiles/*.o

clean: reset
reset:
	rm -f *.elf *.o sfile/*.o $(TARGET).prx $(TARGET)_bin.h

TARGET = recovery
OBJS = main.o menu.o action.o plugin.o lib.o mydebug.o registry.o  kernel/RecoveryBridge.o
OBJS += text.o
OBJS +=  sfiles/vshbridge.o
#sfiles/systemctrl_user.o

ifndef $(TARGET_FW)
TARGET_FW = 600
endif

PSP_FW_VERSION=$(TARGET_FW)

BUILD_PRX = 1

INCDIR = ../include
CFLAGS = -Os -G0 -Wall 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -D_PSP_FW_VERSION=$(PSP_FW_VERSION)

LIBDIR = ../lib
LDFLAGS =
LIBS = -lpspreg -lpspusb -lpspusbstor -lpspusbdevice -lpsppower -lpspsystemctrl_user
#  -lpspaudiocodec -lstdc++ -lpsputility  


#EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = recovery

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

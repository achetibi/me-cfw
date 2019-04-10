release: all
	psp-packer -x -d $(PSP_FW_VERSION) -i $(TARGET).prx
	bin2c $(TARGET).prx $(DATADIR)/$(TARGET)_bin.h $(TARGET)
	sed -i "/int/d" $(DATADIR)/$(TARGET)_bin.h
	rm -f *.elf *.o *.prx sfiles/*.o

clean: reset
reset:
	rm -f *.elf *.o sfiles/*.o $(TARGET).prx $(DATADIR)/$(TARGET)_bin.h

TARGET = resurrection

OBJS = main.o menu.o lib.o mydebug.o action.o unpack.o nand.o
OBJS += sfiles/scePower.o sfiles/DcManager.o sfiles/pspPSAR.o
OBJS += sfiles/pspDecrypt.o sfiles/pspIplUpdate.o

PSP_FW_VERSION = $(TARGET_FW)

BUILD_PRX = 1

INCDIR = ../../include
DATADIR=../installer/data
CFLAGS = -Os -G0 -Wall 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -D_PSP_FW_VERSION=$(PSP_FW_VERSION)

LIBDIR = ../../lib
LDFLAGS =
LIBS = -lpsppower -lpspsystemctrl_user -lpspkubridge

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

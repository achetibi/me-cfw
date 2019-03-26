release: all
	psp-packer -d $(TARGET_FW) -t 0x457BDAF0 -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET)_bin.h $(TARGET)
	sed -i "/int/d" $(TARGET)_bin.h
	rm -f *.elf *.o sfile/*.o

clean: reset
reset:
	rm -f *.elf *.o sfile/*.o $(TARGET).prx $(TARGET)_bin.h

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION=$(TARGET_FW)

TARGET = satellite

OBJS = main.o blit.o ui.o menu.o clock.o cache.o text.o pluginmgr.o exports.o 
OBJS+= sfile/stubkk.o sfile/scepaf.o


BUILD_PRX=1

INCDIR = ../include
CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -D_PSP_FW_VERSION=$(PSP_FW_VERSION)

PRX_EXPORTS = exports.exp

LIBDIR = ../lib
LDFLAGS =
LIBS = -lpsppower -lpspkubridge -lpspsystemctrl_user -lpspvshctrl

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

all:
	psp-packer -i $(TARGET).prx
	bin2c $(TARGET).prx $(TARGET).h $(TARGET)_DC500
	bin2c lang/vunbricker_en.txt vunbricker_en.h vunbricker_en

clean:
	rm *.o $(TARGET).prx $(TARGET).h

TARGET = resurrection
OBJS = crt0.o main.o trans.o common.o vlfutils.o pspinfo.o nand.o install.o ../kpspident/kpspident.o imports.o

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = -mno-crt0 -nostdlib -nodefaultlibs
LIBS = -lpsppower -lpsprtc -lvlfgui -lvlfgu -lvlfutils -lvlflibc -lpspreg -lpspsystemctrl_user -lpspwlan

PSP_FW_VERSION = 271
BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

release: all
	python2.7 ../installer/tool/mksfo.py $(PSP_FW_VERSION) PARAM$(PSP_FW_VERSION).SFO
	pack-pbp ME.PBP PARAM$(PSP_FW_VERSION).SFO ICON0.PNG NULL NULL NULL NULL $(TARGET).prx NULL
	psp-packer -i ME.PBP
#	mksfo 'UPDATE 639LME' PARAM.SFO
#	prxEncrypter $(TARGET).prx
#	pack-pbp EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL data.psp NULL
	rm -f *.elf *.prx *.o data.psp
	
clean: reset
reset:
	rm -f *.elf *.o *.PBP *.SFO

TARGET = plutonium_updater
OBJS = main.o imports.o

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION=$(TARGET_FW)

INCDIR = ../include
CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

#EXTRA_TARGETS = ME.PBP
BUILD_PRX = 1

LIBDIR = ../lib
LIBS = -lpsppower -lpspsystemctrl_user -lpspkubridge
#  -lpspvshbridge

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

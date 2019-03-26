release: all
	python2.7 ../tool/mksfo.py $(PSP_FW_VERSION) PARAM$(PSP_FW_VERSION).SFO
	pack-pbp EBOOT.PBP PARAM$(PSP_FW_VERSION).SFO ICON0_GREEN.PNG NULL NULL NULL NULL $(TARGET).prx NULL
	bin2c EBOOT.PBP plutonium_update_bin.c pupd
	sed -i 's/static//g' plutonium_update_bin.c
	rm -f *.elf *.prx *.o *.SFO

clean: reset
reset:
	rm -f *.elf *.o *.SFO *.prx *.PBP plutonium_update_bin.c

TARGET = plutonium_updater
OBJS = main.o ../kernel/bridge.o ../Plutonium_Driver/Plutonium.o ../Plutonium_Driver/plutonium_driver_bin.c

INCDIR = ../../include
LIBDIR = ../../lib

CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

#EXTRA_TARGETS = EBOOT.PBP
PSP_FW_VERSION = 150
BUILD_PRX = 1

LIBS = -lpspvshbridge -lpsppower -lpspkubridge

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

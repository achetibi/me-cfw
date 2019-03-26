
#ifeq ($(NORMAL),1)
release: all
	python2.7 ../tool/mksfo.py $(PSP_FW_VERSION) PARAM$(PSP_FW_VERSION).SFO
	pack-pbp EBOOT.PBP PARAM$(PSP_FW_VERSION).SFO ICON0.PNG NULL NULL NULL NULL plutonium_updater.prx NULL
	psp-packer -i EBOOT.PBP
	rm -f *.elf *.prx *.o *.sfo
#else
#EXTRA_TARGETS = EBOOT.PBP
#PSP_EBOOT_TITLE = uptest
#endif

clean: reset
reset:
	rm -f *.elf *.o *.PBP *.SFO

TARGET = plutonium_updater
OBJS = main.o ../kernel/bridge.o ../plutonium/plutonium_update_bin.c ../u235/u235_bin.c pspIplUpdate.o ../Downloader/downloader_bin.o

CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

INCDIR = ../../include
#EXTRA_TARGETS = EBOOT.PBP
PSP_FW_VERSION = 150
BUILD_PRX = 1

LIBDIR = ../../lib
LIBS = -lpspvshbridge -lpsppower -lpspsystemctrl_user -lpspkubridge

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

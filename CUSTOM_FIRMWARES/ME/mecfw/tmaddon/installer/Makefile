release: all
	rm -f *.elf *.o *.SFO $(TARGET).prx sfiles/*.o

clean: reset
reset:
	rm -f *.elf *.o *.SFO $(TARGET).prx sfiles/*.o

TARGET = installer

OBJS = main.o menu.o lib.o action.o iplloader.o unpack.o
OBJS += sfiles/pspPSAR.o sfiles/pspDecrypt.o sfiles/DcManager.o

PSP_FW_VERSION = 371

BUILD_PRX = 1

INCDIR = ../../include
CFLAGS = -Os -G0 -Wall 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_TITLE = Time Machine Addon

LIBDIR = ../../lib
LDFLAGS =
LIBS = -lpsppower -lpspkubridge -lpspsystemctrl_user

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

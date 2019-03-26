TARGET = permanent
OBJS = main.o crypto.o kirk_engine.o imports.o bridge/KBridge.o

EXTRA_CLEAN = pspbtjnf_01g.h pspbtjnf_02g.h pspbtjnf_03g.h pspbtjnf_04g.h pspbtjnf_05g.h

INCDIR = ../../include
CFLAGS = -Os -G0 -Wall -mno-gpopt
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LDFLAGS =
LIBS = -lz -lpsppower -lpsputility
LIBDIR = ../../lib

PSP_FW_VERSION = 620

BUILD_PRX = 1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 6.20 LME Permanent Patch
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

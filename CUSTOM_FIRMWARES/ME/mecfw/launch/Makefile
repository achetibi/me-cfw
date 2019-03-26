all:
	rm -f *.o *.prx *.elf *.sfo


ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION = $(TARGET_FW)

TARGET = $(TARGET_FW)launch
OBJS = main.o kernel_patch.o ../kxploit/kxploit.o ../kxploit/sfile/imports.o
LIBS =
LIBDIR =
CFLAGS = -Os -G0 -Wall -D_PSP_FW_VERSION=$(TARGET_FW)
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)
LDFLAGS =
ENCRYPT=1
INCDIR = ../include

BUILD_PRX = 1
PSP_EBOOT_TITLE += LME Launcher for $(TARGET_FW)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include ../lib/build_encrypt.mak

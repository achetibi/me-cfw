all:
ifeq ($(LITE),0)
	mv EBOOT.PBP EBOOT_LITE.PBP
else
	mv EBOOT.PBP EBOOT_FULL.PBP
endif

TARGET = kxploit_tool
OBJS = main.o sfile/imports.o kxploit.o

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION = $(TARGET_FW)

LIBS = -lpspkubridge -lpspsystemctrl_user
LIBDIR = ../lib
CFLAGS = -Os -G0 -Wall -DLITE=$(LITE) -D_PSP_FW_VERSION=$(TARGET_FW)
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)
LDFLAGS =
ENCRYPT=1

BUILD_PRX = 1

EXTRA_TARGETS = EBOOT.PBP
EXTRA_CLEAN = *.PBP

ifeq ($(LITE),0)
PSP_EBOOT_TITLE += LME Installer for $(TARGET_FW)
PSP_EBOOT_ICON = ../light_installer/ICON0.PNG
else
PSP_EBOOT_TITLE += Update Launcher for $(TARGET_FW)
PSP_EBOOT_ICON = ../installer/unpacker/ICON0.PNG
endif

PSPSDK=$(shell psp-config --pspsdk-path)
include ../lib/build_encrypt.mak

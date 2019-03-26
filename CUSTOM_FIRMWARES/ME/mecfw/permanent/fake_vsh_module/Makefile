ifndef $(TARGET_FW)
TARGET_FW = 150
endif

PSP_FW_VERSION = $(TARGET_FW)

TARGET = vshmodule
OBJS = main.o kernel_patch.o imports.o ../../kxploit/kxploit.o ../../kxploit/sfile/imports.o
LIBS =
LIBDIR =
CFLAGS = -Os -G0 -Wall -D_PSP_FW_VERSION=$(TARGET_FW)
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)
LDFLAGS =
INCDIR = ../../include
BUILD_PRX = 1
PSP_EBOOT_TITLE += LME Launcher for $(TARGET_FW)

EXTRA_TARGETS = EBOOT.PBP

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

all:
	psp-packer -d $(TARGET_FW) -t 0x4C94ACF0 -i $(TARGET).prx
	mv $(TARGET).prx ../dist/kd/$(TARGET).prx
	cp LICENSE ../dist/docs/miniupnc-license

TARGET = pspnet_miniupnc

LIBRARYOBJS = \
library/connecthostport.o \
library/igd_desc_parse.o \
library/minisoap.o \
library/miniupnpc.o \
library/minissdpc.o \
library/miniwget.o \
library/minixml.o \
library/portlistingparse.o \
library/receivedata.o \
library/upnpcommands.o \
library/upnperrors.o \
library/upnpreplyparse.o

LIBS = 

OBJS = main.o ../memab/ATPRO.o $(LIBRARYOBJS)

#CFLAGS = -Os -G0 -Wall -fno-pic
CFLAGS = -O0 -G0 -Wall -fno-pic

TARGET_FW = 371

ifeq ($(CONFIG_620), 1)
TARGET_FW = 620
CFLAGS += -DCONFIG_620=1
endif

ifeq ($(CONFIG_63X), 1)
TARGET_FW = 639
CFLAGS += -DCONFIG_63X=1
endif

ifeq ($(CONFIG_660), 1)
TARGET_FW = 660
CFLAGS += -DCONFIG_660=1
endif

ifeq ($(RELEASE), 1)
CFLAGS += -DRELEASE=1
else
CFLAGS += -DDEBUG=1
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_PSPSDK_LIBC = 1

LDFLAGS = -mno-crt0 -nostartfiles

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

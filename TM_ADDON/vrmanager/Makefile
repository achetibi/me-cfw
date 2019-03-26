all:

clean:
	@rm -f *.o
	@rm -f modules/kpspident.prx

TARGET = VResurrection_Manager
OBJS = crt0.o main.o trans.o common.o vlfutils.o install.o battery.o ../kpspident/kpspident.o

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ../lib
LDFLAGS = -mno-crt0 -nostdlib -nodefaultlibs
LIBS = -lpsppower -lpsprtc -lvlfgui -lvlfgu -lvlfutils -lvlflibc -lpspreg -lpspsystemctrl_user

PSP_FW_VERSION = 271
BUILD_PRX = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_ICON = icon/icon0.png
PSP_EBOOT_TITLE = Despertar del Cementerio

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
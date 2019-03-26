PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

ifndef $(PERMANENT)
PERMANENT = 0
endif

ifeq ($(PSP_MODEL),0)
ifeq ($(PERMANENT),1)
TARGET = rebootex_01g_p
else
TARGET = rebootex_01g
endif
endif

ifeq ($(PSP_MODEL),1)
ifeq ($(PERMANENT),1)
TARGET = rebootex_02g_p
else
TARGET = rebootex_02g
endif
endif

#ifeq ($(PSP_MODEL),2)
#ifeq ($(PERMANENT),1)
#TARGET = rebootex_03g_p
#else
#TARGET = rebootex_03g
#endif
#endif

ifeq ($(PSP_MODEL),4)
ifeq ($(PERMANENT),1)
TARGET = rebootex_05g_p
else
TARGET = rebootex_05g
endif
endif

CFLAGS=$(INCLUDES) -W -Wall -G0 -Os -DPSP_MODEL=$(PSP_MODEL) -D_PSP_FW_VERSION=$(TARGET_FW) -DPERMANENT=$(PERMANENT)

all:	a.bin

a.bin: 
	psp-gcc $(CFLAGS) -fno-pic -mno-abicalls -w -S main.c -o main.s
	psp-as main.s -o main.o
	psp-ld -T linkfile.l main.o -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf h.bin
	gzip -f -n h.bin
	bin2c h.bin.gz $(TARGET).h rebootex
#	sed -i "/int/d" $(TARGET).h
	sed -i "/int/c #if _PSP_FW_VERSION == $(TARGET_FW)" $(TARGET).h
	sed -i "/#endif/i #endif" $(TARGET).h
	rm -rf *~ *.o *.elf *.s *.gz

clean: reset
reset:
	rm -f *.elf *.o *.s *.gz *.bin $(TARGET).h $(TARGET)_p.h
	

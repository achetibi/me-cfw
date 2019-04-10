PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include
DATADIR=../../installer/data

ifeq ($(PSP_MODEL), 0)
TARGET = ipl_01g
else
TARGET = ipl_02g
endif

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

CFLAGS=$(INCLUDES) -W -Wall -G0 -Os -DPSP_MODEL=$(PSP_MODEL) -D_PSP_FW_VERSION=$(TARGET_FW)

all: a.bin reset

a.bin: 
	psp-gcc $(CFLAGS) -fno-pic -mno-abicalls -S main.c -o main.s
	psp-as main.s -o main.o
	psp-ld -T linkfile.l main.o -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf $(TARGET).bin
	bin2c $(TARGET).bin $(DATADIR)/$(TARGET)_bin.h $(TARGET)
	sed -i "/int/d" $(DATADIR)/$(TARGET)_bin.h
	rm -rf *~ *.o *.elf *.s *.bin

clean: reset
	rm -f ipl_0?g.bin $(DATADIR)/ipl_0?g_bin.h


reset:
	rm -rf *~ *.o *.elf *.s


PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include -I ../../include

ifeq ($(PSP_MODEL),1)
TARGET = patch_02g
else
TARGET = patch_01g
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

clean: reset
	rm -f patch_0?g.bin


reset:
	rm -rf *~ *.o *.elf *.s


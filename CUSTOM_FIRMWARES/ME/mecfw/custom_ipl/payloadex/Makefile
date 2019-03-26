PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include -I ../../include

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

ifeq ($(PSP_MODEL),1)
TARGET = nand_payloadex_02g
else
TARGET = nand_payloadex_01g
endif

CFLAGS=$(INCLUDES) -W -Wall -G0 -Os -DPSP_MODEL=$(PSP_MODEL) -D_PSP_FW_VERSION=$(TARGET_FW)

all: a.bin reset

a.bin: 
	psp-gcc $(CFLAGS) -fno-pic -mno-abicalls -S main.c -o main.s
	psp-as main.s -o main.o
	psp-ld -T linkfile.l main.o -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf h.bin
	bin2c h.bin $(TARGET).h nand_payloadex

clean: reset
	rm -f nand_payloadex_0?g.h *.bin

reset:
	rm -rf *~ *.o *.elf  *.s


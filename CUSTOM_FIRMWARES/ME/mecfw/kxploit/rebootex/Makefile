PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

CFLAGS=$(INCLUDES) -W -Wall -G0 -D_PSP_FW_VERSION=$(TARGET_FW)

all:	pp.bin
pp.bin: 
	psp-gcc $(CFLAGS) -fno-pic -mno-abicalls -S main.c -o main.s
	psp-as main.s -o main.o
	psp-ld -T linkfile.l main.o -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf h.bin
	bin2c h.bin rebootex.h rebootex
	rm *.o *.elf *.bin

clean:
	rm -f *.o *.elf *.prx *.bin *.s rebootex.h

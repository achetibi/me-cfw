PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

ifeq ($(PSP_MODEL),0)
TARGET = nand_payloadex_01g
else
TARGET = nand_payloadex_02g
endif

CFLAGS=$(INCLUDES) -W -Wall -G0 -Os -DPSP_MODEL=$(PSP_MODEL) -D_PSP_FW_VERSION=$(TARGET_FW)

%.o : %.c

.c.s:
	psp-gcc $(CFLAGS) -fno-pic -mno-abicalls -fno-builtin -Wno-unused-parameter -S $< -o $@

.s.o:  
	psp-as $< -o $@
	
.PRECIOUS: %.s


all:a.bin reset

IO_FILES = fat_lib/memstkro.o fat_lib/dosfs.o fat_lib/hostemu.o

a.bin: main.o $(IO_FILES)
	psp-ld -T linkfile.l main.o $(IO_FILES) -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf h.bin
	bin2c h.bin $(TARGET).h nand_payloadex
	rm -rf *~ *.o *.elf  *.s *.bin

clean: reset
	rm -f nand_payloadex_0?g.h *.bin fat_lib/*.o fat_lib/*.s

reset:
	rm -rf *~ *.o *.elf *.s



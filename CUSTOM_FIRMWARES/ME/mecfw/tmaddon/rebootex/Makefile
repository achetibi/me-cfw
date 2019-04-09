PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include -I ../../include

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

ifeq ($(PSP_MODEL),0)
TARGET = rebootex_01g
endif

ifeq ($(PSP_MODEL),1)
TARGET = rebootex_02g
endif

CFLAGS=$(INCLUDES) -W -Wall -G0 -Os -DPSP_MODEL=$(PSP_MODEL) -D_PSP_FW_VERSION=$(TARGET_FW)

IO_FILES = fat_lib/memstkro.o fat_lib/dosfs.o fat_lib/hostemu.o
SYSTEM_CTRL = sysreg.o syscon.o

%.o : %.c

.c.s:
	psp-gcc $(CFLAGS) -fno-pic -mno-abicalls -S $< -o $@

.s.o:  
	psp-as $< -o $@
	
.PRECIOUS: %.s

all:	a.bin reset

a.bin: main.o $(IO_FILES) $(SYSTEM_CTRL)
	psp-ld -T linkfile.l main.o $(IO_FILES) $(SYSTEM_CTRL) -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf h.bin
	gzip -f h.bin
	bin2c h.bin.gz $(TARGET).h $(TARGET)
	sed -i "/int/c #if _PSP_FW_VERSION == $(TARGET_FW)" $(TARGET).h
	sed -i "/#endif/i #endif" $(TARGET).h

reset:
	rm -rf *~ *.o *.elf *.s ./fat_lib/*.s ./fat_lib/*.o

clean: reset
	rm -f rebootex_0?g.h
	

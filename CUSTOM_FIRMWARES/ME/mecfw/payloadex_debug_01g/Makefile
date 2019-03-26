PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

CFLAGS=$(INCLUDES) -W -Wall -G0 -Os -DPSP_MODEL=$(PSP_MODEL) -D_PSP_FW_VERSION=$(TARGET_FW)

all:	a.bin

a.bin: 
	C:/pspsdk/bin/psp-gcc $(CFLAGS) -fno-pic -mno-abicalls -S main.c -o main.s
	C:/pspsdk/bin/psp-as main.s -o main.o
	C:/pspsdk/bin/psp-ld -T linkfile.l main.o -o main.elf
	C:/pspsdk/bin/psp-strip -s main.elf
	C:/pspsdk/bin/psp-objcopy -O binary main.elf h01g_$(TARGET_FW).bin

clean:
	rm -rf *~ *.o *.elf *.bin *.bin

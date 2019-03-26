PSPSDK=$(shell psp-config --pspsdk-path)

TARGET=libpspvshbridge.a
BASE_FILE= ./*.S

all: $(TARGET)
	cp ./$(TARGET) ../../$(TARGET)

OBJS = sceVshBridge_0000.o sceVshBridge_0001.o sceVshBridge_0002.o sceVshBridge_0003.o sceVshBridge_0004.o sceVshBridge_0005.o sceVshBridge_0006.o sceVshBridge_0007.o sceVshBridge_0008.o sceVshBridge_0009.o sceVshBridge_0010.o sceVshBridge_0011.o sceVshBridge_0012.o sceVshBridge_0013.o sceVshBridge_0014.o sceVshBridge_0015.o sceVshBridge_0016.o sceVshBridge_0017.o sceVshBridge_0018.o sceVshBridge_0019.o sceVshBridge_0020.o sceVshBridge_0021.o 

clean:
	rm -f *.o *.a

CC=psp-gcc
INCDIR = 
CFLAGS = -Os -G0 -Wall -fno-builtin-printf -I$(PSPSDK)/include
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
LDFLAGS=-nodefaultlibs

LIBDIR =

$(TARGET): $(OBJS)
	rm -f $(TARGET)
	psp-ar q $(TARGET) $(OBJS)
	psp-ranlib $(TARGET)


$(OBJS): $(BASE_FILE)
	$(CC) $(CFLAGS) -DF_$* $< -c -o $@


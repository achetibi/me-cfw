PSPSDK=$(shell psp-config --pspsdk-path)

#TARGET=libpspvshctrl.a
BASE_FILE= ./*.S

all: $(TARGET)
	cp ./$(TARGET) ../../$(TARGET)

#OBJS = KUBridge_0000.o

clean:
	rm -f ../../libpspvshctrl.a
	rm -f *.o *.a *.S

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


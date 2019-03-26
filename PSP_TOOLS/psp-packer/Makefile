CC			= i686-w64-mingw32-gcc
CFLAGS		= -Wall -Os -Iinclude
TARGET		= psp-packer
OBJS		= main.o
LDFLAGS		= -lz
PREFIX      = /usr/local

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -static -o $@ $(OBJS) $(LDFLAGS)

all: $(TARGET)

install:
	@mv $(TARGET) $(PREFIX)/bin

uninstall:
	@cd $(PREFIX)/bin; rm -f $(TARGET)

clean:
	@rm -f *.o $(TARGET) *.exe *.exe.stackdump

CC			= i686-w64-mingw32-gcc
CFLAGS		= -O0 -g -ansi -pedantic -Iinclude -Llib
TARGET		= psp-decompiler
OBJS		= allegrex.o analyser.o decoder.o switches.o subroutines.o liveness.o \
              structures.o cfg.o dataflow.o operations.o ssa.o graph.o outgraph.o \
              outcode.o output.o prx.o nids.o hash.o lists.o alloc.o utils.o      \
              constants.o relocs.o module.o main.o
LDFLAGS		= -lexpat
PREFIX      = /usr/local

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

install:
	@mv $(TARGET) $(PREFIX)/bin

uninstall:
	@cd $(PREFIX)/bin; rm -f $(TARGET)

clean:
	@rm -f *.o $(TARGET) *.exe *.exe.stackdump
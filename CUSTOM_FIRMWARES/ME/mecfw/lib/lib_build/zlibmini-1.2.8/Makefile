PSPSDK=$(shell psp-config --pspsdk-path)
PSPDIR=$(shell psp-config --psp-prefix)
TARGET_LIB = libzm.a
OBJS = adler32.o inffast.o inflate.o inftrees.o uncompr.o zutil.o

CFLAGS = -O2 -G0

include $(PSPSDK)/lib/build.mak

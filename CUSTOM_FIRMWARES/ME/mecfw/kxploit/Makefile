
ifndef $(TARGET_FW)
TARGET_FW = 150
endif

ifndef $(LITE)
LITE = 0
endif

release:
	make -C sysmini		TARGET_FW=$(TARGET_FW)
	make -C rebootex	TARGET_FW=$(TARGET_FW)
	bin2c ../light_installer/ME.PBP ./pbp.h update_pbp
	make -f Makefile.mak TARGET_FW=$(TARGET_FW) LITE=$(LITE)
	-rm -f *.prx *.SFO *.o *.elf pbp.h

clean:
	make clean -C sysmini
	make clean -C rebootex
	make clean -f Makefile.mak
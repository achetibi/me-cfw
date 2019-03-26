PSPDEV = $(shell psp-config --pspsdk-path)

all: lib

lib: 
	psp-build-exports -k ../../systemctrl/exports.exp
	psp-build-exports -k ../../vshctrl/exports.exp
	mv KUBridge.S kubridge/KUBridge.S
	mv SystemCtrlForKernel.S systemctrl_kernel/SystemCtrlForKernel.S
	mv SystemCtrlForUser.S systemctrl_user/SystemCtrlForUser.S
	mv VshCtrlLib.S vshctrl/VshCtrlLib.S
	python2.7 make_lib.py
	make -C zlibmini-1.2.8
	mv zlibmini-1.2.8/libzm.a ../libzm.a
	make -C zlib-1.2.8 -f Makefile.PSP
	mv zlib-1.2.8/libz.a ../libz.a

clean:
	make clean -C kubridge
	make clean -C sceVshBridge
	make clean -C systemctrl_kernel
	make clean -C systemctrl_user
	make clean -C vshctrl
	make clean -C zlibmini-1.2.8
	make clean -C zlib-1.2.8 -f Makefile.PSP
	rm -f ../libz.a
	rm -f ../libzm.a

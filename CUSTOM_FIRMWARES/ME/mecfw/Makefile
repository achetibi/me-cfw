BUILD = 2.3

ifeq ($(TARGET_FW), 639)
BUILD = 10.3
endif

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

all: common cfw
lite: common lcfw
double: common cfw lcfw

build_lib:
	make -C lib/lib_build

cfw:
	make -C vshctrl			TARGET_FW=$(TARGET_FW)
	make -C custom_ipl		TARGET_FW=$(TARGET_FW)
	make -C installer		TARGET_FW=$(TARGET_FW)
ifeq ($(TARGET_FW), $(filter $(TARGET_FW), 660 661))
	make -C tmaddon			TARGET_FW=$(TARGET_FW)
endif

lcfw:
#	make -C rebootex		TARGET_FW=$(TARGET_FW) PSP_MODEL=2
	make -C rebootex		TARGET_FW=$(TARGET_FW) PSP_MODEL=4
ifeq ($(TARGET_FW), 620)
#	make -C rebootex		TARGET_FW=$(TARGET_FW) PSP_MODEL=2 PERMANENT=1
	make -C rebootex		TARGET_FW=$(TARGET_FW) PSP_MODEL=4 PERMANENT=1
endif
	make -C systemctrl		TARGET_FW=$(TARGET_FW) PSP_MODEL=2
	make -C systemctrl		TARGET_FW=$(TARGET_FW) PSP_MODEL=4
ifeq ($(TARGET_FW), 620)
	make -C systemctrl		TARGET_FW=$(TARGET_FW) PSP_MODEL=2 PERMANENT=1
	make -C systemctrl 		TARGET_FW=$(TARGET_FW) PSP_MODEL=4 PERMANENT=1
endif
	make -C vshctrl 		TARGET_FW=$(TARGET_FW) HEN=1
	make -C rebootex_lite	TARGET_FW=$(TARGET_FW) PSP_MODEL=0
	make -C rebootex_lite 	TARGET_FW=$(TARGET_FW) PSP_MODEL=1
#	make -C rebootex_lite 	TARGET_FW=$(TARGET_FW) PSP_MODEL=2
	make -C rebootex_lite 	TARGET_FW=$(TARGET_FW) PSP_MODEL=4
ifeq ($(TARGET_FW), 620)
	make -C rebootex_lite	TARGET_FW=$(TARGET_FW) PSP_MODEL=0 PERMANENT=1
	make -C rebootex_lite 	TARGET_FW=$(TARGET_FW) PSP_MODEL=1 PERMANENT=1
#	make -C rebootex_lite 	TARGET_FW=$(TARGET_FW) PSP_MODEL=2 PERMANENT=1
	make -C rebootex_lite 	TARGET_FW=$(TARGET_FW) PSP_MODEL=4 PERMANENT=1
endif
	make -C rebooter		TARGET_FW=$(TARGET_FW) PSP_MODEL=0
	make -C rebooter		TARGET_FW=$(TARGET_FW) PSP_MODEL=1
#	make -C rebooter		TARGET_FW=$(TARGET_FW) PSP_MODEL=2
	make -C rebooter		TARGET_FW=$(TARGET_FW) PSP_MODEL=4
ifeq ($(TARGET_FW), 620)
	make -C rebooter		TARGET_FW=$(TARGET_FW) PSP_MODEL=0 PERMANENT=1
	make -C rebooter		TARGET_FW=$(TARGET_FW) PSP_MODEL=1 PERMANENT=1
#	make -C rebooter		TARGET_FW=$(TARGET_FW) PSP_MODEL=2 PERMANENT=1
	make -C rebooter		TARGET_FW=$(TARGET_FW) PSP_MODEL=4 PERMANENT=1
endif
	make -C light_installer	TARGET_FW=$(TARGET_FW)
	make -C kxploit			TARGET_FW=$(TARGET_FW) LITE=0
	make -C kxploit			TARGET_FW=$(TARGET_FW) LITE=1
	make -C launch			TARGET_FW=$(TARGET_FW)
ifeq ($(TARGET_FW), 620)
	make -C permanent		TARGET_FW=$(TARGET_FW)
endif

common:
	make -C rebootex		TARGET_FW=$(TARGET_FW) PSP_MODEL=0
	make -C rebootex		TARGET_FW=$(TARGET_FW) PSP_MODEL=1
ifeq ($(TARGET_FW), 620)
	make -C rebootex		TARGET_FW=$(TARGET_FW) PSP_MODEL=0 PERMANENT=1
	make -C rebootex		TARGET_FW=$(TARGET_FW) PSP_MODEL=1 PERMANENT=1
endif
	make -C systemctrl		TARGET_FW=$(TARGET_FW) PSP_MODEL=0
	make -C systemctrl		TARGET_FW=$(TARGET_FW) PSP_MODEL=1
ifeq ($(TARGET_FW), 620)
	make -C systemctrl		TARGET_FW=$(TARGET_FW) PSP_MODEL=0 PERMANENT=1
	make -C systemctrl		TARGET_FW=$(TARGET_FW) PSP_MODEL=1 PERMANENT=1
endif
	make -C horoscope		TARGET_FW=$(TARGET_FW)
	make -C idcanager		TARGET_FW=$(TARGET_FW)
	make -C galaxy			TARGET_FW=$(TARGET_FW)
	make -C march33			TARGET_FW=$(TARGET_FW)
	make -C inferno			TARGET_FW=$(TARGET_FW)
	make -C recovery/kernel	TARGET_FW=$(TARGET_FW)
	make -C recovery		TARGET_FW=$(TARGET_FW)
#	make -C vshmenu_src		TARGET_FW=$(TARGET_FW)
	make -C vshmenu_new		TARGET_FW=$(TARGET_FW)
	make -C usbdevice		TARGET_FW=$(TARGET_FW)
	make -C dax9660			TARGET_FW=$(TARGET_FW)
	make -C pspbtjnf		TARGET_FW=$(TARGET_FW)

clean:
	make clean -C lib/lib_build
	make clean -C dax9660
	make clean -C documents
	make clean -C galaxy
	make clean -C horoscope
	make clean -C idcanager
	make clean -C custom_ipl
	make clean -C installer
#ifeq ($(TARGET_FW), $(filter $(TARGET_FW), 660 661))
	make clean -C tmaddon
#endif
	make clean -C kxploit LITE=0
	make clean -C kxploit LITE=1
	make clean -C launch
	make clean -C light_installer
	make clean -C march33
	make clean -C inferno
	make clean -C pspbtjnf
	make clean -C rebooter PSP_MODEL=0
	make clean -C rebooter PSP_MODEL=1
#	make clean -C rebooter PSP_MODEL=2
	make clean -C rebooter PSP_MODEL=4
#ifeq ($(TARGET_FW), 620)
	make clean -C rebooter PSP_MODEL=0 PERMANENT=1
	make clean -C rebooter PSP_MODEL=1 PERMANENT=1
#	make clean -C rebooter PSP_MODEL=2 PERMANENT=1
	make clean -C rebooter PSP_MODEL=4 PERMANENT=1
#endif
	make clean -C rebootex PSP_MODEL=0
	make clean -C rebootex PSP_MODEL=1
#	make clean -C rebootex PSP_MODEL=2
	make clean -C rebootex PSP_MODEL=4
#ifeq ($(TARGET_FW), 620)	
	make clean -C rebootex PSP_MODEL=0 PERMANENT=1
	make clean -C rebootex PSP_MODEL=1 PERMANENT=1
#	make clean -C rebootex PSP_MODEL=2 PERMANENT=1
	make clean -C rebootex PSP_MODEL=4 PERMANENT=1
#endif
	make clean -C rebootex_lite PSP_MODEL=0
	make clean -C rebootex_lite PSP_MODEL=1
#	make clean -C rebootex_lite PSP_MODEL=2
	make clean -C rebootex_lite PSP_MODEL=4
#ifeq ($(TARGET_FW), 620)
	make clean -C rebootex_lite PSP_MODEL=0 PERMANENT=1
	make clean -C rebootex_lite PSP_MODEL=1 PERMANENT=1
#	make clean -C rebootex_lite PSP_MODEL=2 PERMANENT=1
	make clean -C rebootex_lite PSP_MODEL=4 PERMANENT=1
#endif
	make clean -C recovery
	make clean -C recovery/kernel
	make clean -C systemctrl PSP_MODEL=0
	make clean -C systemctrl PSP_MODEL=1
	make clean -C systemctrl PSP_MODEL=2
	make clean -C systemctrl PSP_MODEL=4
#ifeq ($(TARGET_FW), 620)
	make clean -C systemctrl PSP_MODEL=0 PERMANENT=1
	make clean -C systemctrl PSP_MODEL=1 PERMANENT=1
	make clean -C systemctrl PSP_MODEL=2 PERMANENT=1
	make clean -C systemctrl PSP_MODEL=4 PERMANENT=1
#endif
	make clean -C usbdevice
	make clean -C vshctrl
	make clean -C vshctrl HEN=1	
	make clean -C vshmenu_new
#ifeq ($(TARGET_FW), 620)
	make clean -C permanent
#endif
	rm -rf dist
#	@rm -f EBOOT.PBP

distribute_tmaddon:
ifeq ($(TARGET_FW), $(filter $(TARGET_FW), 660 661))
	mkdir -p dist/release_timemachine/PSP/GAME/TIMEMACHINE_ADDON
	cp tmaddon/installer/EBOOT.PBP dist/release_timemachine/PSP/GAME/TIMEMACHINE_ADDON
	cp tmaddon/libpsardumper/libpsardumper.prx dist/release_timemachine/PSP/GAME/TIMEMACHINE_ADDON
	cp tmaddon/pspdecrypt/pspdecrypt.prx dist/release_timemachine/PSP/GAME/TIMEMACHINE_ADDON
	cd dist; zip -r -9 "../../release_$(TARGET_FW)timemachine.zip" "release_timemachine"; cd ..
endif

reset_dist:
	-mkdir dist
	rm -rf dist/*

distribute_lite:
	mkdir -p dist/release_$(TARGET_FW)lme/PSP/GAME/installer
	mkdir -p dist/release_$(TARGET_FW)lme/PSP/GAME/launcher
ifeq ($(TARGET_FW), 620)
	mkdir -p dist/release_$(TARGET_FW)lme/PSP/GAME/permanent
endif
	make -C documents VER=lite TARGET_FW=$(TARGET_FW)
	cp -r inferno dist/release_$(TARGET_FW)lme; make clean -C dist/release_$(TARGET_FW)lme/inferno
	cp -r documents/leda dist/release_$(TARGET_FW)lme
	cp -r recovery/translate dist/release_$(TARGET_FW)lme
	cp vshmenu_new/translate/*.txt dist/release_$(TARGET_FW)lme/translate
	cp kxploit/EBOOT_LITE.PBP dist/release_$(TARGET_FW)lme/PSP/GAME/installer/EBOOT.PBP
	cp launch/EBOOT.PBP dist/release_$(TARGET_FW)lme/PSP/GAME/launcher/EBOOT.PBP
ifeq ($(TARGET_FW), 620)
	cp permanent/fake_vsh_module/vshmodule.prx dist/release_$(TARGET_FW)lme/PSP/GAME/permanent/vshmodule.prx
	cp permanent/installer/bridge/bridge.prx dist/release_$(TARGET_FW)lme/PSP/GAME/permanent/bridge.prx
	cp permanent/installer/EBOOT.PBP dist/release_$(TARGET_FW)lme/PSP/GAME/permanent/EBOOT.PBP
endif
	cp documents/$(TARGET_FW)_lme*en.txt dist/release_$(TARGET_FW)lme/Readme.txt
	cp documents/$(TARGET_FW)_lme*jp.txt dist/release_$(TARGET_FW)lme/Readme_jp.txt
	cp documents/info.txt dist/release_$(TARGET_FW)lme/info.txt
	cd dist; zip -r -9 "../../release_$(TARGET_FW)lme$(BUILD).zip" "release_$(TARGET_FW)lme"; cd ..
	cp light_installer/ME.PBP dist/lme.dat

distribute: reset_dist
	mkdir -p dist/release_$(TARGET_FW)me/PSP/GAME/UPDATE
	make -C documents VER=full TARGET_FW=$(TARGET_FW)
	cp -r inferno dist/release_$(TARGET_FW)me; make clean -C dist/release_$(TARGET_FW)me/inferno
	cp -r documents/leda dist/release_$(TARGET_FW)me
	cp -r recovery/translate dist/release_$(TARGET_FW)me
	cp vshmenu_new/translate/*.txt dist/release_$(TARGET_FW)me/translate
	cp installer/EBOOT.PBP dist/release_$(TARGET_FW)me/PSP/GAME/UPDATE/EBOOT.PBP
	cp documents/$(TARGET_FW)_me*en.txt dist/release_$(TARGET_FW)me/Readme.txt
	cp documents/$(TARGET_FW)_me*jp.txt dist/release_$(TARGET_FW)me/Readme_jp.txt
	cp documents/info.txt dist/release_$(TARGET_FW)me/info.txt
	cd dist; zip -r -9 "../../release_$(TARGET_FW)me$(BUILD).zip" "release_$(TARGET_FW)me"; cd ..
	cp installer/EBOOT.PBP dist/me.dat
	
distribute_signed: reset_dist
	mkdir -p "dist/release_$(TARGET_FW) (OFW Version)/PSP/GAME/me_installer"
	make -C documents VER=full TARGET_FW=$(TARGET_FW)
	cp -r inferno "dist/release_$(TARGET_FW) (OFW Version)"; make clean -C "dist/release_$(TARGET_FW) (OFW Version)/inferno"
	cp -r documents/leda "dist/release_$(TARGET_FW) (OFW Version)"
	cp -r recovery/translate "dist/release_$(TARGET_FW) (OFW Version)"
	cp vshmenu_new/translate/*.txt "dist/release_$(TARGET_FW) (OFW Version)/translate"
	cp kxploit/EBOOT_FULL.PBP "dist/release_$(TARGET_FW) (OFW Version)/PSP/GAME/me_installer/EBOOT.PBP"
	cp installer/EBOOT.PBP "dist/release_$(TARGET_FW) (OFW Version)/PSP/GAME/me_installer/ME.PBP"
	cp documents/$(TARGET_FW)_me*en.txt "dist/release_$(TARGET_FW) (OFW Version)/Readme.txt"
	cp documents/$(TARGET_FW)_me*jp.txt "dist/release_$(TARGET_FW) (OFW Version)/Readme_jp.txt"
	cp documents/info.txt "dist/release_$(TARGET_FW) (OFW Version)/info.txt"
	cd dist; zip -r -9 "../../release_$(TARGET_FW)me$(BUILD) (OFW Version).zip" "release_$(TARGET_FW) (OFW Version)"; cd ..
	cp installer/EBOOT.PBP dist/me.dat

distribute_all: build_lib double distribute distribute_signed distribute_lite distribute_tmaddon
all:
	make -C installer/bridge
	make -C installer
	make -C fake_vsh_module TARGET_FW=$(TARGET_FW)

ifndef $(TARGET_FW)
TARGET_FW = 150
endif

clean:
	make -C installer/bridge clean
	make -C installer clean
	make -C fake_vsh_module clean
	@rm -f bridge.prx vshmodule.prx EBOOT.PBP

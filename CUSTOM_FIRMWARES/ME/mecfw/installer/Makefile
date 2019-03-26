
ifndef $(TARGET_FW)
TARGET_FW = 150
endif

all: 
	make -C u235				PSP_FW_VERSION=$(TARGET_FW)
	make -C kernel				PSP_FW_VERSION=$(TARGET_FW)
	make -C Plutonium_Driver	PSP_FW_VERSION=$(TARGET_FW)
	make -C plutonium			PSP_FW_VERSION=$(TARGET_FW)
	make -C Downloader			PSP_FW_VERSION=$(TARGET_FW)
	make -C unpacker			PSP_FW_VERSION=$(TARGET_FW)
	mv unpacker/EBOOT.PBP EBOOT.PBP

clean:
	make clean -C u235
	make clean -C kernel
	make clean -C Plutonium_Driver
	make clean -C plutonium
	make clean -C Downloader
	make clean -C unpacker
	@rm -f EBOOT.PBP

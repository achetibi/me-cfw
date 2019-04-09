PSPDEV=$(shell psp-config --pspsdk-path)

MODULE_PATH = ..

all:	a.bin

a.bin: 
	psp-combine ipl_header_01g.bin ../mainbinex/patch_01g.bin ipl_block_01g.bin
	psp-combine ipl_header_02g.bin ../mainbinex/patch_02g.bin ipl_block_02g.bin
	psp-combine -l 0x24000 ipl_header_02g.bin ../mainbinex/patch_02g.bin ipl_block_02g.bin
	bin2c ipl_block_01g.bin $(MODULE_PATH)/ipl_block_01g.h ipl_block_01g
	bin2c ipl_block_02g.bin $(MODULE_PATH)/ipl_block_02g.h ipl_block_02g
	bin2c ipl_block_02g.bin $(MODULE_PATH)/ipl_block_large.h ipl_block_large
	sed -i "s/((aligned(16)))/((aligned(64)))/" $(MODULE_PATH)/ipl_block_large.h
	sed -i "/int/d" $(MODULE_PATH)/ipl_block_01g.h
	sed -i "/int/d" $(MODULE_PATH)/ipl_block_02g.h

clean:
	rm -f $(MODULE_PATH)/ipl_block_01g.h  $(MODULE_PATH)/ipl_block_02g.h $(MODULE_PATH)/ipl_block_large.h
	rm -f ipl_block_01g.bin  ipl_block_02g.bin

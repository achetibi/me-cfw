
MODULE_PATH = ../../installer/data

all:
	psp-combine ../mainbinex/patch_01g.bin $(TARGET_FW)/dec_ipl_01g_patched.bin ipl_01g.bin
	psp-combine ../mainbinex/patch_02g.bin $(TARGET_FW)/dec_ipl_02g_patched.bin ipl_02g.bin
	bin2c ipl_01g.bin $(MODULE_PATH)/ipl_01g_bin.h ipl_01g
    bin2c ipl_02g.bin $(MODULE_PATH)/ipl_02g_bin.h ipl_02g

clean:
	rm -f $(MODULE_PATH)/ipl_01g_bin.h $(MODULE_PATH)/ipl_02g_bin.h
	rm -f ipl_01g.bin ipl_02g.bin
PSP-BTCNF=psp-btcnf
PSP-COMBINE=psp-combine
PSP-DAXCR=psp-daxcr
PSP-PACKER=psp-packer
PSP-ENCRYPTER=psp-encrypter

all:
	make -C $(PSP-BTCNF)
	make -C $(PSP-COMBINE)
	make -C $(PSP-DAXCR)
	make -C $(PSP-PACKER)
	make -C $(PSP-ENCRYPTER)

install:
	make -C $(PSP-BTCNF) install
	make -C $(PSP-COMBINE) install
	make -C $(PSP-DAXCR) install
	make -C $(PSP-PACKER) install
	make -C $(PSP-ENCRYPTER) install

uninstall:
	make -C $(PSP-BTCNF) uninstall
	make -C $(PSP-COMBINE) uninstall
	make -C $(PSP-DAXCR) uninstall
	make -C $(PSP-PACKER) uninstall
	make -C $(PSP-ENCRYPTER) uninstall

clean:
	make -C $(PSP-BTCNF) clean
	make -C $(PSP-COMBINE) clean
	make -C $(PSP-DAXCR) clean
	make -C $(PSP-PACKER) clean
	make -C $(PSP-ENCRYPTER) clean

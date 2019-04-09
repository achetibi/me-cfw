all:
	make -C dcman TARGET_FW=$(TARGET_FW)
	make -C pspdecrypt TARGET_FW=$(TARGET_FW)
	make -C libpsardumper TARGET_FW=$(TARGET_FW)
	make -C resurrection TARGET_FW=$(TARGET_FW)
	make -C rebootex TARGET_FW=$(TARGET_FW) PSP_MODEL=0
	make -C rebootex TARGET_FW=$(TARGET_FW) PSP_MODEL=1
	make -C tmctrl TARGET_FW=$(TARGET_FW)
	make -C installer TARGET_FW=$(TARGET_FW)

clean:
	make -C dcman clean
	make -C pspdecrypt clean
	make -C libpsardumper clean
	make -C resurrection clean
	make -C rebootex clean
	make -C tmctrl clean
	make -C installer clean
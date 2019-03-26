PSP-TOOLS=PSP_TOOLS
TM-ADDON=TM_ADDON

all: psp-tools tm-addon

psp-tools:
	make -C $(PSP-TOOLS)
	
tm-addon:
	make -C $(TM-ADDON)


install:
	make -C $(PSP-TOOLS) install

uninstall:
	make -C $(PSP-TOOLS) uninstall

clean:
	make -C $(PSP-TOOLS) clean
	make -C $(TM-ADDON) clean

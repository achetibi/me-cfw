	.set noreorder

#include "pspstub.s"
	
	STUB_START "scePower",0x40090000,0x00010005
	STUB_FUNC  0x0442D852,scePowerRequestColdReset
	STUB_END

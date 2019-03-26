	.set noreorder

#include "pspstub.s"

	STUB_START "Bridge",0x40090000,0x00010005
	STUB_FUNC  0xE2A5D1EE,sceSysregGetTachyonVersion
	STUB_END

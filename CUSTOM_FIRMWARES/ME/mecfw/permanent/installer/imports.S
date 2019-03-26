	.set noreorder

#include "pspstub.s"

	STUB_START "SystemCtrlForUser",0x40090000,0x00010005
	STUB_FUNC  0xB47C9D77,sctrlSEGetVersion
	STUB_END

	STUB_START "KUBridge",0x40090000,0x00020005
	STUB_FUNC  0x4C25EA72,kuKernelLoadModule
	STUB_FUNC  0x24331850,kuKernelGetModel
	STUB_END

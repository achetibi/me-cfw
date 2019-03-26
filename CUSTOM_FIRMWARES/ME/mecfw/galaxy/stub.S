	.set noreorder

#include "pspstub.s"
/*
	STUB_START "SystemCtrlForKernel", 0x00090000, 0x00020005
	STUB_FUNC 0xAC56B90B, sctrlSEGetUmdFile
	STUB_FUNC 0xF9584CAD, sctrlKernelMalloc
	STUB_END
*/

	STUB_START "SysMemForKernel", 0x00010011, 0x00010005
	STUB_FUNC 0xC7E57B9C, sceKernelSetQTGP3
	STUB_END

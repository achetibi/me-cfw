	.set noreorder

#include "pspstub.s"

	STUB_START "SystemCtrlForKernel", 0x00090000, 0x001B0005
	STUB_FUNC 0x2794CCF4, sctrlKernelExitVSH
	STUB_FUNC 0x1C90BECB, sctrlHENSetStartModuleHandler
	STUB_FUNC 0xF9584CAD, sctrlKernelMalloc
	STUB_FUNC 0xA65E8BC4, sctrlKernelFree
	STUB_FUNC 0x826668E9, sctrlHENPatchSyscall
	STUB_FUNC 0x159AF5CC, sctrlHENFindFunction
	STUB_FUNC 0x2D10FB28, sctrlKernelLoadExecVSHWithApitype
	STUB_FUNC 0x16C3B7EE, sctrlSEGetConfig
	STUB_FUNC 0xB47C9D77, sctrlSEGetVersion
	STUB_FUNC 0x2F157BAF, SystemCtrlForKernel_2F157BAF
	STUB_FUNC 0x260CA420, isofs_init
	STUB_FUNC 0xB078D9A0, isofs_exit
	STUB_FUNC 0xE82E932B, isofs_open
	STUB_FUNC 0x324FB7B1, isofs_read
	STUB_FUNC 0xCAE6A8E1, isofs_close
	STUB_FUNC 0x3BC8E648, isofs_lseek
	STUB_FUNC 0xDC974FF8, isofs_getstat
	STUB_FUNC 0x7E6F2BBA, isofs_fastinit
	STUB_FUNC 0x98012538, SetSpeed
	STUB_FUNC 0x5CB025F0, sctrlSESetBootConfFileIndex
	STUB_FUNC 0xB64186D0, sctrlSESetUmdFile
	STUB_FUNC 0xAC56B90B, sctrlSEGetUmdFile
	STUB_FUNC 0x85B520C6, sctrlSEMountUmdFromFile
	STUB_FUNC 0x512E0CD8, sctrlSEUmountUmd
	STUB_FUNC 0x31C6160D, sctrlSESetDiscType
	STUB_FUNC 0xFFEFA034, sctrlSESetDiscOut
	STUB_FUNC 0xEB74FE45, sctrlKernelSetUserLevel
	STUB_END

/*
	STUB_START "SystemCtrlForKernel", 0x00090000, 0x00190005
	STUB_FUNC 0x1C90BECB, sctrlHENSetStartModuleHandler
	STUB_END
*/

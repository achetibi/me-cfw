	.set noreorder

#include "pspstub.s"

	STUB_START "LflashFatfmt",0x00090000,0x00010005
	STUB_FUNC  0xB7A424A4,sceLflashFatfmtStartFatfmt
	STUB_END

	STUB_START "LoadCoreForKernel",0x00010000,0x00020005
	STUB_FUNC  0xD8779AC6,sceKernelIcacheClearAll
	STUB_FUNC  0xCF8A41B1,sceKernelFindModuleByName
	STUB_END

	STUB_START "UtilsForKernel",0x00090000,0x00010005
	STUB_FUNC  0x79D1C3FA,sceKernelDcacheWritebackAll
	STUB_END

	STUB_START "sceSysreg_driver",0x00010011,0x00030005
	STUB_FUNC  0x4F46EEDE,sceSysregGetFuseId
	STUB_FUNC  0x8F4F4E96,sceSysregGetFuseConfig
	STUB_FUNC  0xE2A5D1EE,sceSysregGetTachyonVersion
	STUB_END

	STUB_START "sceSyscon_driver",0x00010011,0x00020005
	STUB_FUNC  0x7EC5A957,sceSysconGetBaryonVersion
	STUB_FUNC  0xE7E87741,sceSysconGetPommelVersion
	STUB_END

	STUB_START "sceNand_driver",0x00010011,0x00010005
	STUB_FUNC  0xC478C1DE,sceNandReadPagesRawAll
	STUB_END

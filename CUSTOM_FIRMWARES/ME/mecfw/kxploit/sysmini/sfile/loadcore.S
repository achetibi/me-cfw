	.set noreorder

#include "pspstub.s"

#if _PSP_FW_VERSION == 620
	STUB_START "LoadCoreForKernel", 0x00010011, 0x00020005
	STUB_FUNC 0xBEF0A05E, sceKernelFindModuleByName
	STUB_FUNC 0x452C5524, sceKernelFindModuleByAddress
	STUB_END

#elif _PSP_FW_VERSION == 639
	STUB_START "LoadCoreForKernel", 0x00010011, 0x00020005
	STUB_FUNC 0xEF8A0BEA, sceKernelFindModuleByName
	STUB_FUNC 0x312CA47E, sceKernelFindModuleByAddress
	STUB_END

#elif _PSP_FW_VERSION == 660
	STUB_START "LoadCoreForKernel", 0x00010011, 0x00020005
	STUB_FUNC 0xF6B1BF0F, sceKernelFindModuleByName
	STUB_FUNC 0xBC99C625, sceKernelFindModuleByAddress
	STUB_END

#elif _PSP_FW_VERSION == 661
	STUB_START "LoadCoreForKernel", 0x00010011, 0x00020005
	STUB_FUNC 0xF6B1BF0F, sceKernelFindModuleByName
	STUB_FUNC 0xBC99C625, sceKernelFindModuleByAddress
	STUB_END

#else
#error loadcore.s
#endif
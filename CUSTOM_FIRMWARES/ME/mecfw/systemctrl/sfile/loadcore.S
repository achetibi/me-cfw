	.set noreorder

#include "pspimport.s"

#if _PSP_FW_VERSION == 620
	IMPORT_START	"LoadCoreForKernel",0x00010000
	IMPORT_FUNC	"LoadCoreForKernel",0xBEF0A05E,sceKernelFindModuleByName
	IMPORT_FUNC	"LoadCoreForKernel",0x452C5524,sceKernelFindModuleByAddress
	IMPORT_FUNC	"LoadCoreForKernel",0x77E8F516,sceKernelFindModuleByUID

#elif _PSP_FW_VERSION == 639
	IMPORT_START	"LoadCoreForKernel",0x00010000
	IMPORT_FUNC	"LoadCoreForKernel",0xEF8A0BEA,sceKernelFindModuleByName
	IMPORT_FUNC	"LoadCoreForKernel",0x312CA47E,sceKernelFindModuleByAddress
	IMPORT_FUNC	"LoadCoreForKernel",0xED53894F,sceKernelFindModuleByUID

#elif _PSP_FW_VERSION == 660
	IMPORT_START	"LoadCoreForKernel",0x00010000
	IMPORT_FUNC	"LoadCoreForKernel",0xF6B1BF0F,sceKernelFindModuleByName
	IMPORT_FUNC	"LoadCoreForKernel",0xBC99C625,sceKernelFindModuleByAddress
	IMPORT_FUNC	"LoadCoreForKernel",0x40972E6E,sceKernelFindModuleByUID

#elif _PSP_FW_VERSION == 661
	IMPORT_START	"LoadCoreForKernel",0x00010000
	IMPORT_FUNC	"LoadCoreForKernel",0xF6B1BF0F,sceKernelFindModuleByName
	IMPORT_FUNC	"LoadCoreForKernel",0xBC99C625,sceKernelFindModuleByAddress
	IMPORT_FUNC	"LoadCoreForKernel",0x40972E6E,sceKernelFindModuleByUID

#else
#error loadcore.S
#endif
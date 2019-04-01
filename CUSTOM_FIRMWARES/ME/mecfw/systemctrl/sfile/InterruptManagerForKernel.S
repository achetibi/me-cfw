	.set noreorder

#include "pspimport.s"

#if _PSP_FW_VERSION == 620
	IMPORT_START	"InterruptManagerForKernel", 0x00010000
	IMPORT_FUNC "InterruptManagerForKernel", 0xAC9306F0, sceKernelQuerySystemCall

#elif _PSP_FW_VERSION == 639
	IMPORT_START	"InterruptManagerForKernel", 0x00010000
	IMPORT_FUNC "InterruptManagerForKernel", 0x399FF74C, sceKernelQuerySystemCall

#elif _PSP_FW_VERSION == 660
	IMPORT_START	"InterruptManagerForKernel", 0x00010000
	IMPORT_FUNC "InterruptManagerForKernel", 0xF153B371, sceKernelQuerySystemCall
	
#elif _PSP_FW_VERSION == 661
	IMPORT_START	"InterruptManagerForKernel", 0x00010000
	IMPORT_FUNC "InterruptManagerForKernel", 0xF153B371, sceKernelQuerySystemCall

#else
#error InterruptManagerForKernel.S
#endif
	.set noreorder

#include "pspstub.s"

#if _PSP_FW_VERSION == 620
	STUB_START "InterruptManagerForKernel", 0x00010011, 0x00020005
	STUB_FUNC 0xEC51802D, sceKernelCallSubIntrHandler
	STUB_FUNC 0xAC9306F0, sceKernelQuerySystemCall
	STUB_END

#elif _PSP_FW_VERSION == 639
	STUB_START "InterruptManagerForKernel", 0x00010011, 0x00020005
	STUB_FUNC 0xD88DD3B4, sceKernelCallSubIntrHandler
	STUB_FUNC 0x399FF74C, sceKernelQuerySystemCall
	STUB_END

#elif _PSP_FW_VERSION == 660
	STUB_START "InterruptManagerForKernel", 0x00010011, 0x00020005
	STUB_FUNC 0x0C5F7AE3, sceKernelCallSubIntrHandler
	STUB_FUNC 0xF153B371, sceKernelQuerySystemCall
	STUB_END

#else
#error interruptmgr.S
#endif
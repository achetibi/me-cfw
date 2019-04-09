	.set noreorder

#include "pspstub.s"

#if _PSP_FW_VERSION == 620
	STUB_START "SysMemForKernel", 0x00010011, 0x00010005
	STUB_FUNC 0x864EBFD7, sceKernelGetModel
	STUB_END

#elif  _PSP_FW_VERSION == 639
	STUB_START "SysMemForKernel", 0x00010011, 0x00010005
	STUB_FUNC 0x458A70B5, sceKernelGetModel
	STUB_END

#elif _PSP_FW_VERSION == 660
	STUB_START "SysMemForKernel", 0x00010011, 0x00010005
	STUB_FUNC 0x07C586A1, sceKernelGetModel
	STUB_END

#elif _PSP_FW_VERSION == 661
	STUB_START "SysMemForKernel", 0x00010011, 0x00010005
	STUB_FUNC 0x07C586A1, sceKernelGetModel
	STUB_END

#else
#error sysmem.s
#endif

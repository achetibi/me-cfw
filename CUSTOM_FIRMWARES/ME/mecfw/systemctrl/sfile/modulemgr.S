	.set noreorder

#include "pspstub.s"

#if _PSP_FW_VERSION == 620
	STUB_START "ModuleMgrForKernel", 0x00010011, 0x000030005
	STUB_FUNC 0xB691CB9F, ModuleMgrForKernel_6DE9FF11 #sceKernelLoadModuleWithApitype2 
	STUB_FUNC 0xE3CCC6EA, sceKernelLoadModule
	STUB_FUNC 0xDF8FFFAB, sceKernelStartModule
	STUB_END

#elif _PSP_FW_VERSION == 639
	STUB_START "ModuleMgrForKernel", 0x00010011, 0x000030005
	STUB_FUNC 0x32292450, ModuleMgrForKernel_6DE9FF11 #sceKernelLoadModuleWithApitype2 
	STUB_FUNC 0xFFB9B760, sceKernelLoadModule
	STUB_FUNC 0xE6BF3960, sceKernelStartModule
	STUB_END

#elif _PSP_FW_VERSION == 660
	STUB_START "ModuleMgrForKernel", 0x00010011, 0x000030005
	STUB_FUNC 0x2B7FC10D, ModuleMgrForKernel_6DE9FF11 #sceKernelLoadModuleWithApitype2 
	STUB_FUNC 0x939E4270, sceKernelLoadModule
	STUB_FUNC 0x3FF74DF1, sceKernelStartModule
	STUB_END

#elif _PSP_FW_VERSION == 661
	STUB_START "ModuleMgrForKernel", 0x00010011, 0x000030005
	STUB_FUNC 0x2B7FC10D, ModuleMgrForKernel_6DE9FF11 #sceKernelLoadModuleWithApitype2 
	STUB_FUNC 0x939E4270, sceKernelLoadModule
	STUB_FUNC 0x3FF74DF1, sceKernelStartModule
	STUB_END

#else
#error modulemgr.s
#endif
	.set noreorder

#include "pspstub.s"

#if _PSP_FW_VERSION == 620
	STUB_START "sceVshBridge", 0x40010011, 0x00050005
	STUB_FUNC 0x74DA9D25, vshLflashFatfmtStartFatfmt
	STUB_FUNC 0x11278E88, vshKernelExitVSHVSH
	STUB_FUNC 0x63E40313, vshKernelGetModel
	STUB_FUNC 0x4ECCCDBC, vshKernelLoadExecVSHMs2
	STUB_FUNC 0xC9626587, vshKernelLoadModuleBufferVSH
	STUB_END

#elif _PSP_FW_VERSION == 639
	STUB_START "sceVshBridge", 0x40010011, 0x00050005
	STUB_FUNC 0x74DA9D25, vshLflashFatfmtStartFatfmt
	STUB_FUNC 0x6F9DF304, vshKernelExitVSHVSH
	STUB_FUNC 0xAD90BEE5, vshKernelGetModel
	STUB_FUNC 0x59BBA567, vshKernelLoadExecVSHMs2
	STUB_FUNC 0xC9626587, vshKernelLoadModuleBufferVSH
	STUB_END

#elif _PSP_FW_VERSION == 660
	STUB_START "sceVshBridge", 0x40010011, 0x00050005
	STUB_FUNC 0x74DA9D25, vshLflashFatfmtStartFatfmt
	STUB_FUNC 0xD47041CA, vshKernelExitVSHVSH
	STUB_FUNC 0x21C243FE, vshKernelGetModel
	STUB_FUNC 0x21D4D038, vshKernelLoadExecVSHMs2
	STUB_FUNC 0xC9626587, vshKernelLoadModuleBufferVSH
	STUB_END

#elif _PSP_FW_VERSION == 661
    STUB_START "sceVshBridge", 0x40010011, 0x00050005
    STUB_FUNC 0x74DA9D25, vshLflashFatfmtStartFatfmt
    STUB_FUNC 0xD47041CA, vshKernelExitVSHVSH
    STUB_FUNC 0x21C243FE, vshKernelGetModel
    STUB_FUNC 0x21D4D038, vshKernelLoadExecVSHMs2
    STUB_FUNC 0xC9626587, vshKernelLoadModuleBufferVSH
    STUB_END

#else
#error vshbridge.S
#endif
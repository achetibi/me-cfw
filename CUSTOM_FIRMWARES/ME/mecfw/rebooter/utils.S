	.set noreorder

#include "pspstub.s"

	STUB_START "UtilsForKernel", 0x00090011, 0x00010005
	STUB_FUNC 0x78934841, _sceKernelGzipDecompress
	STUB_END
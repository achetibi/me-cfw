	.set noreorder

#include "pspstub.s"

#if _PSP_FW_VERSION == 620
	STUB_START "SysMemForKernel", 0x00010011, 0x000C0005
	STUB_FUNC 0xC6A782DA, sceKernelCreateHeap
	STUB_FUNC 0xA60B6316, sceKernelHeapTotalFreeSize
	STUB_FUNC 0x96BFE779, sceKernelAllocHeapMemory
	STUB_FUNC 0xC2A8C491, sceKernelFreeHeapMemory
	STUB_FUNC 0x9BAA857E, sceKernelDeleteHeap
	STUB_FUNC 0x957A966D, sceKernelGetSystemStatus
	STUB_FUNC 0x31DFE03F, sceKernelSetDdrMemoryProtection
	STUB_FUNC 0x864EBFD7, sceKernelGetModel
	STUB_FUNC 0xE5E18A99, sceKernelAllocPartitionMemory
	STUB_FUNC 0xFEB5C72B, sceKernelGetBlockHeadAddr
	STUB_FUNC 0xFE9BC18B, sceKernelDevkitVersion
	STUB_FUNC 0xB00D1EC2, sceKernelSetQTGP3
	STUB_END
	
#elif _PSP_FW_VERSION == 639
	STUB_START "SysMemForKernel", 0x00010011, 0x000C0005
	STUB_FUNC 0xAF85EB1B, sceKernelCreateHeap
	STUB_FUNC 0x61F1BD6F, sceKernelHeapTotalFreeSize
	STUB_FUNC 0x6D161EE2, sceKernelAllocHeapMemory
	STUB_FUNC 0xDB836ADB, sceKernelFreeHeapMemory
	STUB_FUNC 0xF9475C1A, sceKernelDeleteHeap
	STUB_FUNC 0xD9A4D69E, sceKernelGetSystemStatus
	STUB_FUNC 0x00E9A04A, sceKernelSetDdrMemoryProtection
	STUB_FUNC 0x458A70B5, sceKernelGetModel
	STUB_FUNC 0x4621A9CC, sceKernelAllocPartitionMemory
	STUB_FUNC 0x52B54B93, sceKernelGetBlockHeadAddr
	STUB_FUNC 0x5E8DCA05, sceKernelDevkitVersion
	STUB_FUNC 0x9F154FA1, sceKernelSetQTGP3
	STUB_END
	
#elif _PSP_FW_VERSION == 660
	STUB_START "SysMemForKernel", 0x00010011, 0x000C0005
	STUB_FUNC 0x58148F07, sceKernelCreateHeap
	STUB_FUNC 0x01810023, sceKernelHeapTotalFreeSize
	STUB_FUNC 0x23D81675, sceKernelAllocHeapMemory
	STUB_FUNC 0x87C2AB85, sceKernelFreeHeapMemory
	STUB_FUNC 0xDD6512D0, sceKernelDeleteHeap
	STUB_FUNC 0x36C503A9, sceKernelGetSystemStatus
	STUB_FUNC 0x83B5226D, sceKernelSetDdrMemoryProtection
	STUB_FUNC 0x07C586A1, sceKernelGetModel
	STUB_FUNC 0x7158CE7E, sceKernelAllocPartitionMemory
	STUB_FUNC 0xF12A62F7, sceKernelGetBlockHeadAddr
	STUB_FUNC 0xC886B169, sceKernelDevkitVersion
	STUB_FUNC 0xFC639A2B, sceKernelSetQTGP3
	STUB_END

#elif _PSP_FW_VERSION == 661
	STUB_START "SysMemForKernel", 0x00010011, 0x000C0005
	STUB_FUNC 0x58148F07, sceKernelCreateHeap
	STUB_FUNC 0x01810023, sceKernelHeapTotalFreeSize
	STUB_FUNC 0x23D81675, sceKernelAllocHeapMemory
	STUB_FUNC 0x87C2AB85, sceKernelFreeHeapMemory
	STUB_FUNC 0xDD6512D0, sceKernelDeleteHeap
	STUB_FUNC 0x36C503A9, sceKernelGetSystemStatus
	STUB_FUNC 0x83B5226D, sceKernelSetDdrMemoryProtection
	STUB_FUNC 0x07C586A1, sceKernelGetModel
	STUB_FUNC 0x7158CE7E, sceKernelAllocPartitionMemory
	STUB_FUNC 0xF12A62F7, sceKernelGetBlockHeadAddr
	STUB_FUNC 0xC886B169, sceKernelDevkitVersion
	STUB_FUNC 0xFC639A2B, sceKernelSetQTGP3
	STUB_END

#else
#error sysmem.s
#endif
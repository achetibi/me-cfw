#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <pspsysmem_kernel.h>

#include "malloc.h"

static SceUID heapid = -1;

int mallocinit()
{
	int intr = sceKernelInitKeyConfig();
	int size;

	if(intr == PSP_INIT_KEYCONFIG_VSH)
	{
		size = (1024*14);
	}
	else if(intr == PSP_INIT_KEYCONFIG_GAME)
	{
//		if(sceKernelInitApitype() == 0x123)//np9660
//		if( sceKernelBootFrom() != PSP_BOOT_DISC )
//			return 0;

		size = (1024*44);//0xB000
	}
	else
	{
		return 0;
	}

	heapid =sceKernelCreateHeap(1, size ,1, "");

	return (heapid < 0) ? heapid : 0;
}

void *sctrlKernelMalloc(size_t size)
{
//	int i=sceKernelHeapTotalFreeSize( heapid );
//	Kprintf("free: %d\n",i);

	return sceKernelAllocHeapMemory(heapid, size); 
}

int sctrlKernelFree(void *p)
{
//	int i=sceKernelHeapTotalFreeSize( heapid );
//	Kprintf("freex: %d\n",i);

	return sceKernelFreeHeapMemory(heapid, p);
}

int mallocterminate()
{
	return sceKernelDeleteHeap(heapid);
}

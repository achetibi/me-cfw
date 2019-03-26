#include <pspsdk.h>
#include <pspkernel.h>

#include "main.h"

u32 memp2 = 24;
u32 memp9 = 24;

#if PSP_MODEL != 0
static u8 memp8 = 4;
#endif

int mempatch_enabled = 0;

void ReleaseExtraMemory(u32 forced)
{
	if(!sctrlIsHomebrewsRunLevel() && !forced && !mempatch_enabled)
	{
		return;
	}

	unsigned int i = 0;
	for(; i < 0x40; i += 4)
	{
		_sw(0xFFFFFFFF, 0xBC000040 + i);
	}
}

#if PSP_MODEL != 0
static void sctrlPatchPartition(MemPart *part)
{
	u32 *meminfo;
	u32 offset, size;

	meminfo = part->meminfo;
	offset = part->offset;
	size = part->size;

	if(meminfo == NULL)
	{
		return;
	}

	if (offset != 0)
	{
		meminfo[1] = (offset << 20) + 0x88800000;
	}

	meminfo[2] = size << 20;
	((u32*)(meminfo[4]))[5] = (size << 21) | 0xFC;
}

void sctrlPrepatchPartitions(void)
{
	MemPart p8, p11;

	if(!sctrlIsHomebrewsRunLevel())
	{
		return;
	}

	p8.meminfo = (u32 *)sctrlKernelGetPartition(8);
	p11.meminfo = (u32 *)sctrlKernelGetPartition(11);

	memp8 = p8.size = 1;
	
	if(p11.meminfo != NULL)
	{
		p8.offset = 56 - 4 - p8.size;
	}
	else
	{
		p8.offset = 56 - p8.size;
	}

	sctrlPatchPartition(&p8);

	p11.size = 4;
	p11.offset = 56 - 4;

	sctrlPatchPartition(&p11);
}
#endif

int sctrlHENSetMemory(u32 p2, u32 p9)
{
#if PSP_MODEL != 0
	if(p2 == 0 || (p2 + p9) > MAX_HIGH_MEMSIZE)
	{
		return 0x80000107;
	}

	memp2 = p2;
	memp9 = p9;

	return 0;
#else
	return 0x80000107;
#endif
}

int SystemCtrlForKernel_B86E36D1()
{
#if PSP_MODEL != 0
	MemPart p2, p9;
	int maxsize;
	
	p2.meminfo = (u32 *)sctrlKernelGetPartition(2);
	p9.meminfo = (u32 *)sctrlKernelGetPartition(9);

	if(memp2 == 24 && memp9 == 24)
	{
		p2.size = MAX_HIGH_MEMSIZE;
		p9.size = 0;
	}
	else
	{
		p2.size = memp2;
		p9.size = memp9;
	}

	if((u32 *)sctrlKernelGetPartition(11) != NULL)
	{
		maxsize = 56 - 4 - memp8;
	}
	else
	{
		maxsize = 56 - memp8;
	}

	if (p2.size + p9.size > maxsize)
	{
		int len;
		len = p2.size + p9.size - maxsize;

		if(p9.size > len)
		{
			p9.size -= len;
		}
		else
		{
			p2.size -= len - p9.size; 
			p9.size = 0;
		}
	}

	sctrlHENSetMemory(24, 24);

	p2.offset = 0;
	sctrlPatchPartition(&p2);

	p9.offset = p2.size;
	sctrlPatchPartition(&p9);

	mempatch_enabled = 1;

	ReleaseExtraMemory(0);
#endif

	return 0;
}

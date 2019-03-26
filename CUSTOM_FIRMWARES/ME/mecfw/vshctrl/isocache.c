#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman.h>
#include <psploadcore.h>
#include <pspctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "systemctrl_me.h"

#include "virtualpbpmgr.h"
#include "main.h"

static u8 cachechanged = 0;

static VirtualPbp *cache = NULL;
static u8 referenced[32];


//#define ISOCACHE_PATH "ms0:/PSP/SYSTEM/isocaches.bin"

static const char *cache_path[2] = {
	"ms0:/PSP/SYSTEM/isocaches.bin",
	"ef0:/PSP/SYSTEM/isocaches.bin",
};

int ReadCache(int dev)
{
	SceUID fd;
	int i;

	if (!cache) {
		cache = (VirtualPbp *)sctrlKernelMalloc(32*sizeof(VirtualPbp));//sctrlMalloc
	}

	memset(cache, 0, sizeof(VirtualPbp)*32);
	memset(referenced, 0, sizeof(referenced));

	for (i = 0; i < 0x10; i++)
	{
		fd = sceIoOpen( cache_path[dev] /* ISOCACHE_PATH */ , PSP_O_RDONLY, 0);

		if (fd >= 0)
		{
			sceIoRead(fd, cache, sizeof(VirtualPbp)*32);
			sceIoClose(fd);
			return 0;
		}
	}

	return -1;
}

int SaveCache(int dev )
{
	SceUID fd;
	int i;

	if (!cache) return -1;

	for (i = 0; i < 32; i++)
	{
		if (cache[i].isofile[0] != 0 && !referenced[i])
		{
			cachechanged = 1;
			memset(&cache[i], 0, sizeof(VirtualPbp));
		}
	}

	if (cachechanged)
	{
		cachechanged = 0;

		for (i = 0; i < 0x10; i++)
		{
			fd = sceIoOpen( cache_path[dev] /* ISOCACHE_PATH */ , PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0511);
			if (fd >= 0)
			{
				sceIoWrite(fd, cache, sizeof(VirtualPbp)*32);
				sceIoClose(fd);
				return 0;
			}
		}

		return -1;
		
	}

	return 0;
}

int IsCached(char *isofile, ScePspDateTime *mtime, VirtualPbp *res)
{
	int i;

	for (i = 0; i < 32; i++)
	{
		if (cache[i].isofile[0] != 0)
		{
			if (strcmp(cache[i].isofile, isofile) == 0)
			{
				if (memcmp(mtime, &cache[i].mtime, sizeof(ScePspDateTime)) == 0)
				{
					memcpy(res, &cache[i], sizeof(VirtualPbp));
					referenced[i] = 1;
					return 1;
				}
			}
		}
	}

	return 0;
}

int Cache(VirtualPbp *pbp)
{
	int i;

	for (i = 0; i < 32; i++)
	{
		//cache
		if (cache[i].isofile[0] == 0)
		{
			referenced[i] = 1;
			memcpy(&cache[i], pbp, sizeof(VirtualPbp));
			cachechanged = 1;
			return 1;
		}
	}

	return 0;
}


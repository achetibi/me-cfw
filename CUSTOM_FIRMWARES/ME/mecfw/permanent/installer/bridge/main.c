#include <pspkernel.h>
#include <pspreg.h>
#include <stdio.h>
#include <string.h>
#include <psprtc.h>
#include <psputilsforkernel.h>
#include "systemctrl_me.h"

PSP_MODULE_INFO("PermanentBridge", 0x1000, 1, 0);

#define MAKE_CALL(f) (0x0C000000 | (((u32)(f) >> 2) & 0x03ffffff)) 

static u8 vshmain_kirkheader[272];
static int vshmain_kirkheader_copied = 0;
static int (*sceUtilsBufferCopyWithRange)(u32 a0, u32 a1, u32 a2, u32 a3, u32 t0);

static int sceUtilsBufferCopyWithRangePatched(u32 a0, u32 a1, u32 a2, u32 a3, u32 t0)
{
	int ret;

	if(!vshmain_kirkheader_copied) {
		memcpy(vshmain_kirkheader, (void*)a2, sizeof(vshmain_kirkheader));
		vshmain_kirkheader_copied = 1;
	}

	ret = (*sceUtilsBufferCopyWithRange)(a0, a1, a2, a3, t0);

	return ret;
}

void patch_mesgled(SceModule2 *mod)
{
	sceUtilsBufferCopyWithRange = (void*)(sctrlHENFindFunction("sceMemlmd", "semaphore", 0x4C537C72));
	_sw(MAKE_CALL(sceUtilsBufferCopyWithRangePatched), mod->text_addr+0x000009FC);
}

void sync_cache(void)
{
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();
}

int get_kirk_header(void *buf)
{
	if(vshmain_kirkheader_copied) {
		memcpy(buf, vshmain_kirkheader, sizeof(vshmain_kirkheader));

		return 0;
	}

	return -1;
}

int module_start(SceSize argc, void *argp)
{
	patch_mesgled(sceKernelFindModuleByName("sceMesgLed"));
	sync_cache();

	return 0;
}

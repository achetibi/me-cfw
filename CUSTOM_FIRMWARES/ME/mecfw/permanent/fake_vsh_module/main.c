#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspiofilemgr.h>
#include <psputility.h>
#include <psputility_htmlviewer.h>
#include <psploadexec.h>
#include <psploadexec_kernel.h>
#include <psputils.h>
#include <psputilsforkernel.h>
#include <pspsysmem.h>
#include <psppower.h>
#include <string.h>

#include "main.h"
#include "kubridge.h"
#include "../ppatch_config.h"

PSP_MODULE_INFO("hen_launcher", 0x800, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH | PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(0);

extern int scePowerRequestColdReset(int unk);
static u8 buf[1024] __attribute__((aligned(64)));
u8 rebootex_buff[0x4000];
int recovery_flag = 0;

void sync_cache(void)
{
	/*
	 * Beware there is a bug in PSPSDK. 
	 * sceKernelIcacheInvalidateAll in pspsdk is imported as UtilsForKernel by default which cannot be used for an user PRX (returns 0x8002013A)
	 * Because of this the original kxploit by some1 used a delay by 1 second to flush i-cache. But sometimes it still fails.
	 * import.S contains sceKernelIcacheInvalidateAll as workaround
	 */
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();
}

void init_flash(void)
{
	sceIoUnassign("flash0:");
	sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", 0, NULL, 0);
	sceIoUnassign("flash1:");
	sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", 0, NULL, 0);
}

int copy_file(const char *src, const char *dst)
{
	SceUID fd = -1, fdw = -1;
	int ret;

	ret = sceIoOpen(src, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto error;
	}

	fd = ret;

	ret = sceIoOpen(dst, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (ret < 0) {
		goto error;
	}

	fdw = ret;
	ret = sizeof(buf);
	ret = sceIoRead(fd, buf, ret);

	while (ret > 0) {
		ret = sceIoWrite(fdw, buf, ret);

		if (ret < 0) {
			goto error;
		}

		ret = sceIoRead(fd, buf, ret);
	}

	if (ret < 0) {
		goto error;
	}

	sceIoClose(fd);
	sceIoClose(fdw);

	return 0;

error:
	sceIoClose(fd);
	sceIoClose(fdw);

	return ret;
}

int write_file(const char *path, unsigned char *buf, int size)
{
	SceUID fd;
	int ret;

	fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0) {
		goto error;
	}

	ret = sceIoWrite(fd, buf, size);

	if (ret < 0) {
		goto error;
	}

	sceIoClose(fd);

	return 0;
error:
	if (fd >= 0)
		sceIoClose(fd);

	return -1;
}

static void uninstall_fake_vsh(void)
{
	SceIoStat stat;	

	init_flash();
	if(sceIoGetstat(VSHORIG, &stat) == 0) {
		sceIoRemove(VSHMAIN);
		copy_file(VSHORIG, VSHMAIN);
		sceIoRemove(VSHORIG);
	}
}

int main(int argc, char * argv[])
{
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);

	pspDebugScreenInit();
	if((pad.Buttons & (PSP_CTRL_CIRCLE | PSP_CTRL_CROSS | PSP_CTRL_SELECT | PSP_CTRL_START)) == (PSP_CTRL_CIRCLE | PSP_CTRL_CROSS | PSP_CTRL_SELECT | PSP_CTRL_START)) {
		uninstall_fake_vsh();
		sceKernelDelayThread(100000);
		scePowerRequestColdReset(0);

		return 0;
	}
	else if (pad.Buttons & PSP_CTRL_RTRIGGER) {
		recovery_flag = 1;
	}

	doKernelExploit(NULL);

	int mid = sceKernelLoadModule("flash0:/kd/rebooter.prx",0,0);
	if( mid < 0 ) {
		sceKernelDelayThread(10000000);
		sceKernelExitGame();
	}

	sceKernelStartModule(mid, 0, NULL, 0, NULL);

	sync_cache();

	memcpy( rebootex_buff , (void *)0x08fc0000 , 0x4000 );

	sceKernelExitGame();
	sceKernelExitDeleteThread(0);

	return 0;
}

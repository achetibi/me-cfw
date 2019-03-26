/*

ちょっと勢いで書いてみました。
liquidzigong氏の6.39 Kxploitサンプルをベース(というか丸パクリ)に、
ゲーム終了時にrebootexを実行するようにしてみました。
多分、この後6.39向けのrebootexとsystemctrl書けばHENができると思いますｗ
・・・時間あればやってみようかなあ。
あ、PSP1000用でゲーム終了時に無線LANのLEDが点灯することを確認しました。

Thanks liquidzigong and some1!
*/

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

PSP_MODULE_INFO("hen_launcher", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(0);

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

int model;
u8 rebootex_buff[0x4000];
int is_exploited = 0;

int main(int argc, char * argv[])
{
	pspDebugScreenInit();
//	printk("6.39 kxploit POC by liquidzigong\n");
//	printk("originally found by some1\n");

	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);
	if (pad.Buttons & PSP_CTRL_RTRIGGER)
	{
		is_exploited = 1;
	}

	doKernelExploit(NULL);

	if(is_exploited) 
	{
		printk("Enter Recovery Menu ....\n");
	}

	int mid = sceKernelLoadModule("flash0:/kd/rebooter.prx",0,0);
	if( mid < 0 )
	{
		printk("Load error -> 0x%08X\n", mid);
		sceKernelDelayThread(10000000);
		sceKernelExitGame();
	}

//	_sw( rebootex_cp, (void *)0x08fb0000 );
//	_sw( func_rebootex , (void *)0x08fb0004 );

	sceKernelStartModule(mid, 0, NULL, 0, NULL);

	sync_cache();

	memcpy( rebootex_buff , (void *)0x08fc0000 , 0x4000 );

	printk("Model %02dg\n", model + 1 );
	printk("Exiting...\n");

	sceKernelDelayThread(1000000);
	sceKernelExitGame();
	sceKernelExitDeleteThread(0);

	return 0;
}
//
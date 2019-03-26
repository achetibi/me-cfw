/*
--- Message from J416 ---

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

#if LITE == 0
#include "pbp.h"
#endif

#include "rebootex/rebootex.h"

PSP_MODULE_INFO("__kxploit__", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(0);

#include "main.h"
#include "kxploit_offs.h"
#include "loadexec_offs.h"

struct SceKernelLoadExecVSHParam param;

int kuKernelGetModel();
int sctrlKernelLoadExecVSHWithApitype(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param);
static int (*func_rebootex)(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

int model;
char filename[256];
int is_exploited = 0;

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

static int rebootex_cp(unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
	memcpy((void *) 0x88FC0000, rebootex, size_rebootex);
	memset((void *) 0x88FB0000, 0, 256);

	_sw(model, 0x88FB0000);

	return func_rebootex(a1, a2, a3, a4, a5);
}

int kernel_permission_call(void)
{
	void (* _sceKernelIcacheInvalidateAll)(void) = (void *)sceKernelIcacheInvalidateAll_Offs;
	void (* _sceKernelDcacheWritebackInvalidateAll)(void) = (void *)sceKernelDcacheWritebackInvalidateAll_Offs;
	u32 (*_sceKernelFindModuleByName)(char *) = (void *)sceKernelFindModuleByName_Offs;
	int (* _sceKernelGetModel)() = (void *)sceKernelGetModel_Offs;

	recovery_sysmem();

	model = _sceKernelGetModel();

	unsigned int addr;
	addr = _sceKernelFindModuleByName("sceLoadExec");
	addr += 108;
    addr = *(unsigned int *) addr;

	const u32 *loadexex_list = ( model == 4 ) ? loadexec_patch_list_05g : loadexec_patch_list;

	MAKE_CALL( addr + loadexex_list[0] ,rebootex_cp);
	_sw( 0x3C0188FC, addr + loadexex_list[1] ); // lui $at, 0x88FC

	
	_sw(0x1000000C, addr + loadexex_list[2] );
	_sw(0, addr + loadexex_list[3] );

	_sceKernelIcacheInvalidateAll();
	_sceKernelDcacheWritebackInvalidateAll();
	func_rebootex = (void *) addr;

	void ( * _sceKernelLoadExecVSHMs1)() = (void *)(addr + loadexec_patch_list[4]);
	void ( * _sceKernelLoadExecVSHMs1_05g)() = (void *)(addr + loadexec_patch_list_05g[4]);
	void ( * _sceKernelLoadExecVSHEf1_05g)() = (void *)(addr + loadexec_patch_list_05g[5]);

	if( model == 4 )
	{
		if( filename[0] == 'e' && filename[1] == 'f' )
		{
			_sceKernelLoadExecVSHEf1_05g( filename /*FILE_PATH*/ ,&param );
		}
		else
		{
			_sceKernelLoadExecVSHMs1_05g( filename /*FILE_PATH*/ ,&param );
		}
	}
	else
	{
		_sceKernelLoadExecVSHMs1( filename /*FILE_PATH*/ ,&param);
	}

	is_exploited = 1;

	return 0;
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);

	if (fd >= 0)
	{
		sceIoWrite(fd, buf, size);
		sceIoClose(fd);
		return 1;
	}

	return -1;
}

int main(int argc, char * argv[])
{
	pspDebugScreenInit();

	if(sceKernelDevkitVersion() != DEVKIT_VER )
	{
		printk("This program require "VER_STR"\n");
		sceKernelDelayThread(5*1000*1000);
		sceKernelExitGame();
	}

	printk("Update Launcher for "VER_STR"\n\n");

	printk("Thanks to liquidzigong for 6.39 kxploit POC\n");
	printk("Thanks to some1/Davee/Proxima/Zecoxao for kxploit\n");
	printk("Thanks to bbtgp for his PrxEncrypter\n");
	printk("Thanks to Draan for his kirk-engine code\n");

	strcpy( filename , argv[0] );	
	char *p = strrchr(filename, '/');
	strcpy(p+1, "ME.PBP");

	printk("\nTarget file:%s\n", filename );

#if LITE == 0
	printk("Unpacking ... ");
	WriteFile( filename , update_pbp , size_update_pbp );
	printk("Done\n\n");
#endif

	memset( &param, 0, sizeof(param));
	param.size=sizeof(param);
	param.args= strlen( filename ) + 1;//sizeof(FILE_PATH);
	param.argp= filename;//FILE_PATH;
	param.key="updater";//

	model = kuKernelGetModel();
//	if( sctrlHENIsSE() == 1 )
	if( model >= 0)
	{
		printk("\n\nLaunching Please wait ...\n");
		sceDisplaySetHoldMode(1);

		int api_type;
		if( filename[0] == 'e' && filename[1] == 'f' )
			api_type = 0x151;
		else
			api_type = 0x140;

		sctrlKernelLoadExecVSHWithApitype( api_type , filename, &param);
	}
	else
	{
		doKernelExploit("\n\nLaunching Please wait ...\n");
	}

	printk("Model %02dg\n", model + 1 );
	printk("Exiting...\n");

	sceKernelDelayThread(1000000);
	sceKernelExitGame();
	sceKernelExitDeleteThread(0);

	return 0;
}

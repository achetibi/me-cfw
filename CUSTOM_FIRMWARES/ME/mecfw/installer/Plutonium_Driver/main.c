/*
*/
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspiofilemgr_kernel.h>

//#include <systemctrl.h>
#include <systemctrl_me.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "../modules/ipl_block_large.h"
//#include "../modules/ipl_block_01g.h"
#include "../../custom_ipl/ipl_block_large.h"
#include "../../custom_ipl/ipl_block_01g.h"

#define NOP			0x00000000
#define JR_RA		0x03E00008
#define J_OPCODE	0x08000000
#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2) & 0x03FFFFFF), a);  _sw(NOP, a+4);
//#define UPDATE_PATH "ms0:/PSP/GAME/UPDATE/EBOOT.PBP"

#if _PSP_FW_VERSION == 620
#define UPDATE_PATH "ms0:/PSP/GAME/UPDATE/620.PBP"

#elif _PSP_FW_VERSION == 639
#define UPDATE_PATH "ms0:/PSP/GAME/UPDATE/639.PBP"
//int ipl_size = 0x20000; // 131072

#elif _PSP_FW_VERSION == 660
#define UPDATE_PATH "ms0:/PSP/GAME/UPDATE/660.PBP"
//int ipl_size = 0x20000; // 131072

#elif _PSP_FW_VERSION == 661
#define UPDATE_PATH "ms0:/PSP/GAME/UPDATE/661.PBP"
//int ipl_size = 0x1F000; // 126976
#endif

int ipl_size = 0x20000; // 131072
int IplUpdate_patched = 0;

int (*sceIplUpdateSetIpl)(void *buf, int size);
int (*scePowerRequestColdReset)(int);
int (*sceIplUpdateVerifyIpl)(void *buf, int size);

int sceIplUpdateSetIplPatched(void *buf, int size);
int sceIplUpdateVerifyIplPatched(void *buf, int size);

//int restore_act();

PSP_MODULE_INFO("Plutonium_Driver", 0x1000, 1, 0);

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

#include <pspdisplay.h>

#if 0
typedef union {
	int rgba;
	struct {
		char r;
		char g;
		char b;
		char a;
	} c;
} color_t;
#endif

void ScreenFlash(int color)
{
	sceDisplaySetFrameBuf((void *)0x44000000, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 1);

	int *p = (int*)0x44000000;
	while (p < (int*)0x44200000)
		*p++ = color;
}

int PlutoniumColdReset()//Plutonium_E30ED0F0
{
	int k1 = pspSdkSetK1(0);
	scePowerRequestColdReset = (void*)sctrlHENFindFunction("scePower_Service", "scePower", 0x0442D852);
	scePowerRequestColdReset(0);
	pspSdkSetK1(k1);
	return 0;
}

int PlutoniumGetModel()//Plutonium_340AC1E4
{
	int k1 = pspSdkSetK1(0);
	int model = sceKernelGetModel();
	pspSdkSetK1(k1);
	return model;
}

int sceIoGetstatPatched(const char *file, SceIoStat *stat)
{
	int k1 = pspSdkSetK1(0);
	if (strcmp(file, "ms0:/PSP/GAME/UPDATE/EBOOT.PBP") == 0) {
		strcpy((char*)file, UPDATE_PATH);
	}
	pspSdkSetK1(k1);
	return sceIoGetstat(file, stat);
}

SceUID sceIoOpenPatched(const char *file, int flags, SceMode mode)
{
	int k1 = pspSdkSetK1(0);

	if (!IplUpdate_patched) {
		sceIplUpdateSetIpl = (void*)sctrlHENFindFunction("TexSeqCounter", "sceIplUpdate", 0xEE7EB563);
		if (sceIplUpdateSetIpl != 0) 
		{
			sctrlHENPatchSyscall(sceIplUpdateSetIpl, sceIplUpdateSetIplPatched);
			sceIplUpdateVerifyIpl = (void*)sctrlHENFindFunction("TexSeqCounter", "sceIplUpdate", 0x0565E6DD);
			sctrlHENPatchSyscall(sceIplUpdateVerifyIpl, sceIplUpdateVerifyIplPatched);
			IplUpdate_patched = 1;
		}
	}
	
	if (strcmp(file, "ms0:/PSP/GAME/UPDATE/EBOOT.PBP") == 0) {
		strcpy((char*)file, UPDATE_PATH );
	}
	pspSdkSetK1(k1);
	return sceIoOpen(file, flags, mode);
}

int uranium235_thread(SceSize args, void *argp)
{
//	restore_act();

	SceKernelLMOption option;	
	sceKernelSetDdrMemoryProtection((void*)0x08400000, 0x00400000, 0xF);

	option.size = sizeof(option);
	option.mpidtext=5;
	option.mpiddata=5;
	option.flags=0;
	option.position=0;
	option.access=1;
	option.creserved[0]=0;
	option.creserved[1]=0;

	SceUID u235 = sceKernelLoadModule("ms0:/PSP/GAME/UPDATE/u235.prx", 0, &option);
	if (u235 >= 0) {
		sceKernelStartModule(u235, 0, NULL, NULL, NULL);
	}

	sceKernelExitDeleteThread(0);
	return 0;
}

int WriteFile(char *filename, void *buffer, u32 size)
{
	SceUID fd = sceIoOpen(filename, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0) return -1;

	int written = sceIoWrite(fd, buffer, size);
	if (sceIoClose(fd) < 0) return -1;

    return written;
}

#if 0
u8 act_buff[0x1500];
int restore_act()
{
	sceIoUnassign("flash2:");
	int 	res = sceIoAssign("flash2:", "lflash0:0,2", "flashfat2:", IOASSIGN_RDWR, NULL, 0);
	if( res >= 0)
	{
		int fd = sceIoOpen("ms0:/PSP/GAME/UPDATE/act.dat", PSP_O_RDONLY, 0777);
		if( fd >= 0)
		{
			int size = sceIoRead(fd ,  act_buff  , 0x1500);
			sceIoClose(fd);

			if(size > 0)
				WriteFile("flash2:/act.dat" ,  act_buff ,  size);
		}
		sceIoUnassign("flash2:");
	}
	return 0;
}
#endif

int scePowerRequestColdResetPatched()
{
	int k1 = pspSdkSetK1(0);

	SceUID uranium235_thid = sceKernelCreateThread("uranium235", uranium235_thread, 0xF, 0x1000, 0, NULL);
	if (uranium235_thid < 0) {
		pspSdkSetK1(k1);
		return -1;
	}
	
	sceKernelStartThread(uranium235_thid, 0, NULL);
	pspSdkSetK1(k1);
	return uranium235_thid;
}

int PlutoniumLoadUranium()
{
	int k1 = pspSdkSetK1(0);
	int r = scePowerRequestColdResetPatched();
	pspSdkSetK1(k1);
	return r;
}
/*
int sceKernelStartModulePatched(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	int k1 = pspSdkSetK1(0);
	SceModule2 *mod = sceKernelFindModuleByName("sceNAND_Updater_Driver");
	if (mod)
	{
		if (strcmp(mod->modname, "sceNAND_Updater_Driver") == 0)
		{
			//WriteFile("ms0:/nand_update.bin" , mod->text_addr , mod->text_size );
			_sh(0xAC60, mod->text_addr+0x0D7E);
			ClearCaches();
		}
	}
	pspSdkSetK1(k1);
	return sceKernelStartModule(modid, argsize, argp, status, option);
}
*/
int plutonium_thread(SceSize args, void *argp)
{
	sctrlHENPatchSyscall(sctrlHENFindFunction("scePower_Service", "scePower", 0x0442D852), scePowerRequestColdResetPatched);

	/*
	if (sceKernelGetModel() == 0 )
	{
		sctrlHENPatchSyscall(sctrlHENFindFunction("sceModuleManager", "ModuleMgrForUser", 0x50F0C1EC), sceKernelStartModulePatched);
	}*/

	sctrlHENPatchSyscall(sctrlHENFindFunction("sceIOFileManager", "IoFileMgrForUser", 0x109F50BC), sceIoOpenPatched);
	sctrlHENPatchSyscall(sctrlHENFindFunction("sceIOFileManager", "IoFileMgrForUser", 0xACE946E8), sceIoGetstatPatched);
	ClearCaches();

	sceKernelDelayThread(70000);
	while (1) {
		SceModule2 *mod = sceKernelFindModuleByName("plutonium_updater");
		if (mod == NULL) break;
		sceKernelStopModule(mod->modid, 0, NULL, NULL, NULL);
		sceKernelUnloadModule(mod->modid);
		sceKernelDelayThread(50000);
	}

	SceUID updater;
	if ( sceKernelDevkitVersion() < 0x06000010 ) {
		updater = ModuleMgrForKernel_6723BBFF( UPDATE_PATH, 0, NULL);
	}
	else {
		SceUID (* _sceKernelLoadModule)() = (void *)ModuleMgrForKernel_6723BBFF;
		updater = _sceKernelLoadModule( PSP_LOADMODULE_APITYPE_MS1 , UPDATE_PATH, 0, NULL);
	}

	ScreenFlash(0x00FF0000);

	if (updater >= 0) {
		sceKernelStartModule(updater, sizeof("ms0:/PSP/GAME/UPDATE/EBOOT.PBP"), "ms0:/PSP/GAME/UPDATE/EBOOT.PBP", 0, NULL);
	}
	else {
		ScreenFlash(0x000000FF);
	}

	sceKernelExitDeleteThread(0);
	return 0;
}

int PlutoniumStartUpdater()
{
	int k1 = pspSdkSetK1(0);

	SceUID plutonium_thid = sceKernelCreateThread("plutonium", plutonium_thread, 0xF, 0x1000, 0, NULL);
	if (plutonium_thid >= 0) {
		sceKernelStartThread(plutonium_thid, 0, NULL);
	}

	pspSdkSetK1(k1);
	return plutonium_thid;
}


int sceIplUpdateSetIplPatched(void *buf, int size)
{
	int k1 = pspSdkSetK1(0);

//	ipl_size = 0x20000 + 0x4000;
	ipl_size = size_ipl_block_large;

	memcpy( ipl_block_large + 0x4000, buf, ipl_size - 0x4000);

	if( sceKernelGetModel() == 0 ) {
		memcpy( ipl_block_large , ipl_block_01g , 0x4000 );
	}

	ClearCaches();

	sceIplUpdateSetIpl = (void *)sctrlHENFindFunction("TexSeqCounter" , "sceIplUpdate", 0xEE7EB563);
	int ret = sceIplUpdateSetIpl(ipl_block_large , ipl_size);

	pspSdkSetK1(k1);
	return ret;
}

int sceIplUpdateVerifyIplPatched(void *buf, int size)
{
	int k1 = pspSdkSetK1(0);
	sceIplUpdateVerifyIpl = (void*)sctrlHENFindFunction("TexSeqCounter", "sceIplUpdate", 0x0565E6DD);
	int ret = sceIplUpdateVerifyIpl( ipl_block_large , ipl_size );
	pspSdkSetK1(k1);
	return ret;
}

int module_start(SceSize args, void *argp)
{
	if (argp == NULL)
		return 0;

	u32 value = *(u32 *)argp;
	
	u32 func = sctrlHENFindFunction("scePower_Service", "scePower", 0x2085D15D);

	if (func && value) {	
		_sw(0x03E00008, func);
		_sw(0x24020062, func+4);
		ClearCaches();
	}

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}


#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psputilsforkernel.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "rebootex_config.h"

PSP_MODULE_INFO("SystemControl", 0x3007, 2, 5);
PSP_MAIN_THREAD_ATTR(0);
u32 module_sdk_version = CFW_VERSION;


int k150_flag = 0;

u32 size_systemctrl = 0;
char *systemctrl = NULL;

void PatchMemlmd();
void PatchLoadCore();
void PatchModuleMgr();


void ClearCaches()
{
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();
}

#if 0
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
#endif


#if _PSP_FW_VERSION == 620
#define INTEREPT_PATCH_ADDR0	0x00000E94//OK
#define INTEREPT_PATCH_ADDR1	0x00000DE8//OK

#elif _PSP_FW_VERSION == 639
#define INTEREPT_PATCH_ADDR0	0x00000E94
#define INTEREPT_PATCH_ADDR1	0x00000DE8

#elif _PSP_FW_VERSION == 660
#define INTEREPT_PATCH_ADDR0	0x00000E98
#define INTEREPT_PATCH_ADDR1	0x00000DEC

#elif _PSP_FW_VERSION == 661
#define INTEREPT_PATCH_ADDR0	0x00000E98
#define INTEREPT_PATCH_ADDR1	0x00000DEC

#else
#error intereptman
#endif

static void PatchInterruptMgr()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceInterruptManager");
	u32 text_addr = mod->text_addr;

	// Allow execution of syscalls in kmode
	_sw( 0x408F7000 ,	text_addr + INTEREPT_PATCH_ADDR0	);
	_sw( 0 			,	text_addr + INTEREPT_PATCH_ADDR0 + 4);
	//mct0		$t7, $EPC
	//eret		0x42000018
}

#if _PSP_FW_VERSION == 620
static const u16 sysmem_patch_list[] ={
	0x00009B4C,//OK
	0x00009C6C,//OK
	0x00009D04,//OK
	0x00009DB4,//OK
	0x00009E88,//OK

	0x00009F2C,//
	0x00009FD0,//
	0x0000A068,//
};

#elif _PSP_FW_VERSION == 639
static const u16 sysmem_patch_list[] ={
	0x00009A2C,
	0x00009B4C,
	0x00009BE4,
	0x00009C94,
	0x00009D68,

	0x00009E0C,
	0x00009EB0,
	0x00009F48,
	0x00009FF8,
};

#elif _PSP_FW_VERSION == 660
static const u16 sysmem_patch_list[] ={
	0x000098F0,//
	0x00009A10,//
	0x00009AA8,//
	0x00009B58,//
	0x00009C2C,//

	0x00009CD0,//
	0x00009D74,//
	0x00009E0C,//
	0x00009EBC,//
	0x00009F6C,//
};

#elif _PSP_FW_VERSION == 661
static const u16 sysmem_patch_list[] ={
	0x000098F0,//
	0x00009A10,//
	0x00009AA8,//
	0x00009B58,//
	0x00009C2C,//

	0x00009CD0,//
	0x00009D74,//
	0x00009E0C,//
	0x00009EBC,//
	0x00009F6C,//
};

#else
#error sysmem
#endif

//patch to break loop
static void PatchSystemMemoryMgr()
{
	u32 text_addr;
	int i;

	for( i= 0;i<(sizeof(sysmem_patch_list)/sizeof(u16));i++)
	{
		text_addr = 0x88000000 | sysmem_patch_list[i];
		_sh( 0x1000 , text_addr + 2 );
	}
}


int module_bootstart(SceSize args, void *argp)
{	
	size_systemctrl = *(u32 *)0x88FB00F0;
	
	PatchLoadCore();
	PatchModuleMgr();
	PatchMemlmd();
	PatchInterruptMgr();
	PatchSystemMemoryMgr();

	ClearCaches();

	if(size_systemctrl == 0xFFFFFFFF )
		size_systemctrl = 0;

	if(size_systemctrl)
	{
		SceUID sysctrl_mem = sceKernelAllocPartitionMemory( 1 , "systemexbin" , 1 , size_systemctrl , 0);

		if( sysctrl_mem >=0)
		{
			systemctrl = (char *)sceKernelGetBlockHeadAddr( sysctrl_mem );

			memset( systemctrl ,0 , size_systemctrl);
			memcpy( systemctrl , (void *)0x88FB0100 , size_systemctrl );
		}
	}

	struct RebootexParam *rebootex_param = (void *)REBOOTEX_PARAM_OFFSET;

	sctrlSESetUmdFile( rebootex_param->file );//(char *)0x88fB0000
	sctrlSEApplyConfig((SEConfig *)(rebootex_param->config));//0x88FB0050
	sctrlSESetBootConfFileIndex( rebootex_param->reboot_index );//*(int *)0x88fb00C0
	sctrlHENSetMemory( rebootex_param->mem2, rebootex_param->mem9 );

#if PSP_MODEL == 0
	k150_flag = rebootex_param->k150_flag;//*(int *)0x88fb00CC;
#endif

//	memset( (void *)0x88FB0000 ,0 , 0x20000);
//	ClearCaches();
	return 0;
}


int module_stop(void)
{
	return 0;
}


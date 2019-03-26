
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psploadexec_kernel.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <pspreg.h>
#include <pspumd.h>

#include <stdio.h>
#include <string.h>


#include "main.h"
#include "systemctrl_me.h"

//void *block;
static int drivestat = ( PSP_UMD_READY | PSP_UMD_INITED | PSP_UMD_PRESENT);
//int drivestat = PSP_UMD_INITED | PSP_UMD_PRESENT;//SCE_UMD_READY | SCE_UMD_MEDIA_IN;
static SceUID umdcallback = -1;


static void pspUmdCallback(int dstat)
{
	if (umdcallback >= 0)
	{
		sceKernelNotifyCallback(umdcallback, dstat);
	}
}

int sceUmdActivatePatched(const int mode, const char *aliasname)
{
	int k1 = pspSdkSetK1(0);

	sceIoAssign(aliasname, "umd0:", "isofs0:", IOASSIGN_RDONLY, NULL, 0);

	if(!(drivestat & 0x20))
		pspUmdCallback( 0x32);

	drivestat= 0x32;
	//drivestat = SCE_UMD_READABLE | SCE_UMD_MEDIA_IN;	

	pspSdkSetK1(k1);	
	return 0;
}

int sceUmdDeactivatePatched(const int mode, const char *aliasname)
{
	int k1 = pspSdkSetK1(0);
	sceIoUnassign(aliasname);
	drivestat = PSP_UMD_INITED | PSP_UMD_PRESENT;//SCE_UMD_MEDIA_IN | SCE_UMD_READY;
	
	pspUmdCallback(drivestat);
	pspSdkSetK1(k1);	
	return 0;
}

int sceUmdGetDiscInfoPatched(pspUmdInfo *disc_info)
{
	int k1 = pspSdkSetK1(0);
	disc_info->size = 8; 
	disc_info->type = PSP_UMD_TYPE_GAME;

	pspSdkSetK1(k1);	
	return 0;
}

int sceUmdRegisterUMDCallBackPatched(SceUID cbid)
{
	int k1 = pspSdkSetK1(0);
	umdcallback = cbid;
	pspUmdCallback(drivestat);
	pspSdkSetK1(k1);	
	return 0;
}

int sceUmdUnRegisterUMDCallBackPatched(SceUID cbid)
{
	int k1 = pspSdkSetK1(0);
	umdcallback = -1;
	pspSdkSetK1(k1);	
	return 0;
}

int sceUmdGetDriveStatPatched()
{
	int k1 = pspSdkSetK1(0);
	int r = drivestat;
	pspSdkSetK1(k1);	
	return r;
}

//__attribute__((noinline)) 
static u32 FindUmdUserFunction(u32 nid)
{
	return FindProc("sceUmd_driver", "sceUmdUser", nid);
}

static const u32 umd_nid_list[][2] = {
	{ 0xC6183D47, (u32)sceUmdActivatePatched	},
	{ 0xE83742BA, (u32)sceUmdDeactivatePatched	},
	{ 0x340B7686, (u32)sceUmdGetDiscInfoPatched	},
	{ 0xAEE7404D, (u32)sceUmdRegisterUMDCallBackPatched		},
	{ 0xBD2BDE07, (u32)sceUmdUnRegisterUMDCallBackPatched	},
	{ 0x46EBB729, 1	},//sceUmdCheckMedium
	{ 0x8EF08FCE, 0	},//sceUmdWaitDriveStat
	{ 0x56202973, 0	},//sceUmdWaitDriveStatWithTimer
	{ 0x4A9E5E29, 0	},//sceUmdWaitDriveStatCB
	{ 0x6B4A146C, (u32)sceUmdGetDriveStatPatched	},
	{ 0x20628E6F, 0	},//sceUmdGetErrorStat
	{ 0x87533940, 0	},//sceUmdReplaceProhibit
	{ 0xCBE9F02A, 0	},//sceUmdReplacePermit
};

void DoNoUmdPatches()
{
	int i;
	for(i=0;i < sizeof(umd_nid_list)/(sizeof(int) * 2);i++)
	{
		u32 value = umd_nid_list[i][1];
		u32 umd_addr = FindUmdUserFunction( umd_nid_list[i][0] );
		if( value )
		{
			if( value == 1 )
			{
				MAKE_DUMMY_FUNCTION1( umd_addr );
			}
			else
			{
				REDIRECT_FUNCTION( umd_addr, (void *)value );
			}
		}
		else
		{
			MAKE_DUMMY_FUNCTION0( umd_addr );
		}
	}

	ClearCaches();
}

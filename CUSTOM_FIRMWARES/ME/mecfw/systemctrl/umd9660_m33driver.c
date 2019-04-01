#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "systemctrl_me.h"
#include "iso/umd9660_driver.h"
#include "iso/isofs_driver.h"

int umd9660_init();
int umd9660_exit(PspIoDrvArg* arg);
int umd_read_block(void *drvState, u8 *buf, SceInt64 read_size, LbaParams *lba_param);


int discFlag = 1;		//dataA6E8
static int data7DE4[4] = {-1 , -1 , -1 , -1};	//

extern int umd_open;
extern int discType;
extern int umd_file_len;
extern SceUID umd_sema;

int sctrlSEGetDiscType()
{
	int k1 = pspSdkSetK1(0);
	int ret = discType;
	pspSdkSetK1(k1);
	return ret;
}

#if _PSP_FW_VERSION == 620
#define UMD9660_PATCH_ADDR1		0x00002458//OK
#define UMD9660_PATCH_ADDR2		0x000078A0//OK
#define UMDMAN_PATCH_ADDR1		0x0000B7A4//OK
#define UMDMAN_PATCH_ADDR2		0x0000B814//OK
#define UMDMAN_INFO_FUNC_NID	0xB0BB67F5//OK

#elif _PSP_FW_VERSION == 639
#define UMD9660_PATCH_ADDR1		0x00002458
#define UMD9660_PATCH_ADDR2		0x000079CC
#define UMDMAN_PATCH_ADDR1		0x0000B7A4
#define UMDMAN_PATCH_ADDR2		0x0000B814
#define UMDMAN_INFO_FUNC_NID	0x28B41927

#elif _PSP_FW_VERSION == 660
#define UMD9660_PATCH_ADDR1		0x00001E84
#define UMD9660_PATCH_ADDR2		0x00007500
#define UMDMAN_PATCH_ADDR1		0x0000B7AC
#define UMDMAN_PATCH_ADDR2		0x0000B81C
#define UMDMAN_INFO_FUNC_NID	0xD1478023

#elif _PSP_FW_VERSION == 661
#define UMD9660_PATCH_ADDR1		0x00001E84
#define UMD9660_PATCH_ADDR2		0x00007500
#define UMDMAN_PATCH_ADDR1		0x0000B7AC
#define UMDMAN_PATCH_ADDR2		0x0000B81C
#define UMDMAN_INFO_FUNC_NID	0xD1478023

#else
#error umd9660_m33driver.c
#endif

//loc_00004A3C:	
void *sceUmdManGetUmdDiscInfoPatched()
{
	void *(*sceUmdManGetUmdDiscInfo_k)(void) = (void *)sctrlHENFindFunction("sceUmdMan_driver", "sceUmdMan_driver", UMDMAN_INFO_FUNC_NID );
	u32 *info = (u32 *)sceUmdManGetUmdDiscInfo_k();

	sceKernelWaitSema(umd_sema, 1, 0);
	int umd_len = umd_file_len;
	sceKernelSignalSema(umd_sema, 1);

	if(umd_len > 0)
	{ 
		//patch the returned disc info
		info[0x64 / 4] = 0xE0000800;
		info[0x68 / 4] = 0;
		info[0x6C / 4] = 0;
		info[0x70 / 4] = umd_len;
		info[0x74 / 4] = umd_len;
		info[0x80 / 4] = 1;
		info[0x84 / 4] = 1;
	}

	return info;
}

//loc_00004A10
u32 sceGpioPortReadPatched()
{
	u32 *addir = (u32 *)0xBE240004;
	u32 ret = addir[0];

	if(discFlag)
		ret &= 0xFBFFFFFF;

	return ret;
}

u32 patch_addir[6];//dataB0D4
u32 patch_backup[6];//dataB0F0

void DiscOutPatch()
{
	u32 add = (u32)sctrlHENFindFunction("sceLowIO_Driver","sceGpio_driver",0x4250D44A);//sceGpioPortRead

	patch_addir[4] = add;
	patch_backup[4] = *(u32 *)add;

	patch_addir[5]=add + 4;
	patch_backup[5]=*(u32 *)(add + 4);

	REDIRECT_FUNCTION(add, sceGpioPortReadPatched);
}

//; 0x000056D8
void sctrlSESetDiscOut(int out)
{
	int k1 = pspSdkSetK1(0);

	DiscOutPatch();
	ClearCaches();

	discFlag = out;//dataA6E8

	sceKernelCallSubIntrHandler(4,26,0,0);

	sceKernelDelayThread(50000);

	pspSdkSetK1(k1);
}

//sub_00005740
void DoAnyUmd()
{
	umd9660_init();
	sceKernelSetQTGP3(data7DE4);

	SceModule2 *mod = sceKernelFindModuleByName("sceUmd9660_driver");
	if( mod == NULL )
		return;

	u32 text_addr = mod->text_addr;
	u32 patch_where;


	patch_where = text_addr + UMD9660_PATCH_ADDR1;
	patch_addir[0] = patch_where;
	patch_backup[0]= *(u32 *)patch_where;
	MAKE_CALL( patch_where ,  umd_read_block);


	patch_where = text_addr + UMD9660_PATCH_ADDR2;
	patch_addir[1] = patch_where;
	patch_backup[1]= *(u32 *)patch_where;
	MAKE_JUMP( patch_where , sceUmdManGetUmdDiscInfoPatched);


	mod = sceKernelFindModuleByName("sceUmdMan_driver");
	text_addr=mod->text_addr;

	u32 disctype_patch = 0x24040000 | discType;

	patch_where = text_addr + UMDMAN_PATCH_ADDR1;
	patch_addir[2] = patch_where;
	patch_backup[2]= *(u32 *)patch_where;
	_sw( disctype_patch , patch_where );

	patch_where = text_addr + UMDMAN_PATCH_ADDR2;
	patch_addir[3] = patch_where;
	patch_backup[3]= *(u32 *)patch_where;
	_sw( disctype_patch , patch_where );


	if(!patch_addir[4])
	{
		DiscOutPatch();
	}

	discFlag =0;

	ClearCaches();

	sceKernelCallSubIntrHandler(4, 0x1A, 0, 0);//
	sceKernelDelayThread(50000);
	return;
}

static int isof_backup = 0;

int sctrlSEMountUmdFromFile(char *file, int noumd, int isofs)
{
	int k1 = pspSdkSetK1(0);
	int res;

	sctrlSESetUmdFile(file);

	if (!noumd && !isofs)
	{
		DoAnyUmd();
		pspSdkSetK1(k1);
		return 0;
	}

	sceIoDelDrv("umd");

	if ((res = sceIoAddDrv(getumd9660_driver())) < 0)
	{
		pspSdkSetK1(k1);
		return res;
	}

	if (noumd)
	{
		DoNoUmdPatches();
	}

	if (isofs )
	{

		sceIoDelDrv("isofs");
		sceIoAddDrv(getisofs_driver());

		SceModule2 *mod =sceKernelFindModuleByName("sceIsofs_driver");
		if(mod && !isof_backup)
		{	
			isof_backup = 1;

			u32 patch_addr = (u32)search_module_export( mod, NULL, 0x2F064FA6 );//module_reboot_before
//			u32 patch_addr = mod->text_addr + 0x00004364;

			MAKE_DUMMY_FUNCTION0( patch_addr );
			ClearCaches();
		}

		sceIoAssign("disc0:", "umd0:", "isofs0:", IOASSIGN_RDONLY, NULL, 0);
	}

	pspSdkSetK1(k1);
	return 0;
}

int sctrlSEUmountUmd()//Unmount iso
{
	int k1 = pspSdkSetK1(0);
	int i;

	if(umd_open==0)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	umd9660_exit(NULL);
	sctrlSESetUmdFile("");

	sceKernelSetQTGP3(data7DE4);
	discFlag =1;//dataA6E8disc out

	sceKernelCallSubIntrHandler(4,26,0,0);//InterruptManagerForKernel_8BE96A2E
	sceKernelDelayThread(0xC350);

	for(i=0;i<6;i++)
	{
		if(patch_addir[i])
		{

			_sw(patch_backup[i] , patch_addir[i]);

			patch_addir[i]=0;
			patch_backup[i]=0;
		}
	}

	ClearCaches();
	pspSdkSetK1(k1);
	return 0;
}

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psploadexec_kernel.h>
#include <pspmodulemgr_kernel.h>

#include <stdio.h>
#include <string.h>

#include "main.h"

#if _PSP_FW_VERSION == 620
static const struct sctrlLibFunc lib_list = {
	.GetPartitionInfoFuncAddr	= 0x00003E2C,//OK
	.FindDriverFuncAddr			= 0x00002A38,//OK
	.SetUserLevelPatchAddr		= 0x00019E80,//OK
	.SetDevkitPatchAddr			= 0x00011AAC,//OK
	.LoadExecVshFuncAddr		= 0x00002304,//OK
	.LoadExecVshFuncAddr_05g	= 0x00002558,//OK
	.FatfsDeviceFlagAddr		= 0x0000ECEC,//OK
};
#elif _PSP_FW_VERSION == 639
static const struct sctrlLibFunc lib_list = {
	.GetPartitionInfoFuncAddr	= 0x00003E34,
	.FindDriverFuncAddr			= 0x00002A44,
	.SetUserLevelPatchAddr		= 0x00019E80,
	.SetDevkitPatchAddr			= 0x00011998,
	.LoadExecVshFuncAddr		= 0x00002384,
	.LoadExecVshFuncAddr_05g	= 0x000025D8,
	.FatfsDeviceFlagAddr		= 0x0000EDC8,
};
#elif _PSP_FW_VERSION == 660
static const struct sctrlLibFunc lib_list = {
	.GetPartitionInfoFuncAddr	= 0x00003D28,
	.FindDriverFuncAddr			= 0x00002A4C,
	.SetUserLevelPatchAddr		= 0x00019F40,
	.SetDevkitPatchAddr			= 0x0001191C,
	.LoadExecVshFuncAddr		= 0x00002384,
	.LoadExecVshFuncAddr_05g	= 0x000025D8,
	.FatfsDeviceFlagAddr		= 0x0000E74C,
};

#else
#error ctrl_lib
#endif

int sctrlIsHomebrewsRunLevel(void)
{
	int api = sceKernelInitApitype();
	if(api == 0x141 || api == 0x152)
	{
		return 1;
	}

	return 0;
}

int sctrlKernelQuerySystemCall(void *addr)
{
	int k1 = pspSdkSetK1(0);
	int res = sceKernelQuerySystemCall( addr );
	pspSdkSetK1(k1);

	return res;
}

void *sctrlKernelGetPartition(int pid)
{			
	PartitionInfo* (*GetPartition)(int) = (void *)( 0x88000000 + lib_list.GetPartitionInfoFuncAddr );
	return GetPartition( pid );
}

PspIoDrv *sctrlHENFindDriver(char *drvname)
{
	int k1 = pspSdkSetK1(0);
	SceModule2 *mod = sceKernelFindModuleByName("sceIOFileManager");
	PspIoDrv *ret = NULL;

	u32 *(* GetDevice)() =(void *)( mod->text_addr + lib_list.FindDriverFuncAddr );

#if PSP_MODEL == 4
FIND_DRIVER:;
#endif
	u32 *dev = GetDevice(drvname);
	if ( dev != NULL )
	{
		ret = (PspIoDrv *)( dev[1]);
	}
#if PSP_MODEL == 4
	else
	{		
		if( strcmp(drvname, "msstor") == 0 ) 
		{
			drvname = "eflash0a0f";
			goto FIND_DRIVER;
		}
		else if( strcmp(drvname, "msstor0p") == 0 ) 
		{
			drvname = "eflash0a0f0p";
			goto FIND_DRIVER;
		}

	}
#endif	
	pspSdkSetK1(k1);
	return ret;
}

int sctrlKernelSetUserLevel(int level)
{
	int k1 = pspSdkSetK1(0);
	void (* apply_level)(int);
	int res = sceKernelGetUserLevel();
	if( res >= 0)
	{
		SceModule2 *mod =sceKernelFindModuleByName("sceThreadManager");
		u32 *thstruct = (u32 *)( mod->text_addr + lib_list.SetUserLevelPatchAddr );

		u32 *struct_level = (u32 *)( thstruct[0] );
		struct_level[0x14/4] = (level ^ 8) << 28;

		apply_level = (void *)( thstruct[8/4] );
		if( k1 != 0 && apply_level != NULL )
		{
			apply_level( level );
		}
	}

	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelSetDevkitVersion(int version)
{
	int k1 = pspSdkSetK1(0);
	int prev = sceKernelDevkitVersion();
	const u32 patch_addr = ( 0x88000000 + lib_list.SetDevkitPatchAddr);

	_sh( version >> 16 ,	patch_addr		);//high
	_sh( version & 0xFFFF , patch_addr + 8	);//low

	ClearCaches();
	pspSdkSetK1(k1);
	return prev;
}

int sctrlKernelLoadExecVSHWithApitype(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int (* LoadExecVSH)(int, const char *, struct SceKernelLoadExecVSHParam *, int);

	int k1 = pspSdkSetK1(0);
	SceModule2 *mod = sceKernelFindModuleByName("sceLoadExec");

#if PSP_MODEL == 4
	LoadExecVSH = (void *)( mod->text_addr + lib_list.LoadExecVshFuncAddr_05g	);
#else
	LoadExecVSH = (void *)( mod->text_addr + lib_list.LoadExecVshFuncAddr		);
#endif

	int res = LoadExecVSH(apitype, file, param, 0x10000);
	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelMsIsEf()
{
#if PSP_MODEL == 4
	int k1 = pspSdkSetK1(0);
	int res = 0;
	SceModule2 *mod = sceKernelFindModuleByName("sceFATFS_Driver");
	if( mod != NULL )
	{
		int (* DevFlag)() = (void *)( mod->text_addr + lib_list.FatfsDeviceFlagAddr );
		res = DevFlag();
	}
	pspSdkSetK1(k1);
	return res;
#else
	return 0;
#endif
}
//////////////////////// sctrllib //////////////////////////////////

int sctrlKernelLoadExecVSHDisc(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);
	int res = sceKernelLoadExecVSHDisc(file, param);
	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHDiscUpdater(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);
	int res = sceKernelLoadExecVSHDiscUpdater(file, param);
	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHMs1(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);
	int res = sceKernelLoadExecVSHMs1(file, param);
	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHMs2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);	
	int res = sceKernelLoadExecVSHMs2(file, param);
	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);	
	int res = sceKernelLoadExecVSHMs3(file, param);
	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHMs4(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);	
	int res = sceKernelLoadExecVSHMs4(file, param);
	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);
	int res = sceKernelExitVSHVSH(param);
	pspSdkSetK1(k1);
	return res;
}
//////////// ef0: //////////////////
/*
int sctrlKernelLoadExecVSHEf1(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(00);
	int res = sceKernelLoadExecVSHEf1(file, param);
	pspSdkSetK1(k1);
	return res;
}
*/
int sctrlKernelLoadExecVSHEf2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);	
	int res = sceKernelLoadExecVSHEf2(file, param);
	pspSdkSetK1(k1);
	return res;
}
/*
int sctrlKernelLoadExecVSHEf3(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);	
	int res = sceKernelLoadExecVSHEf3(file, param);
	pspSdkSetK1(k1);
	return res;
}

int sctrlKernelLoadExecVSHEf4(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	int k1 = pspSdkSetK1(0);	
	int res = sceKernelLoadExecVSHEf4(file, param);
	pspSdkSetK1(k1);
	return res;
}
*/
//////////// ctrl lib //////////////////////////

int my_strcspn(const char *s1, const char *s2)
{
	const char *p = s1;
	for( ; *s1; s1++) {
		const char *t;
		for(t = s2; *t; t++)
			if(*t == *s1)
				return (int)(s1 - p);
	}
	return (int)(s1 - p);
}

int my_strspn(const char *s1, const char *s2)
{
	const char *p = s1;
	for ( ; *s1; s1++) {
		const char  *t;
		for (t = s2; *t != *s1; t++)
			if(*t == '\0')
				return (int)(s1 - p);
	}
	return (int)(s1 - p);
}

char *my_strtok_r(char *s1, const char *s2 , char **s3 )
{
	char  *pbegin, *pend;
	char *save = *s3;

	pbegin = s1 ? s1 : save;
	if(!pbegin)
		return NULL;

	pbegin += my_strspn(pbegin, s2);	/* strspn‚ð—˜—p */
	if (*pbegin == '\0') {
		*s3 = NULL;
		return (NULL);
	}

	pend = pbegin + my_strcspn(pbegin, s2);	/* strcspn‚ð—˜—p */
	if(*pend != '\0') {
		*pend = '\0';
		pend++;
	}
	
	*s3 = pend;
	return (pbegin);
}


char *my_strtok(char *s1, const char *s2 )
{
	static char *tok_save = NULL;
	return my_strtok_r( s1 , s2 , &tok_save );
}

/*
	sw         $ra, 0($a0)
	sw         $gp, 44($a0)
	sw         $sp, 4($a0)
	sw         $fp, 8($a0)
	sw         $s0, 12($a0)
	sw         $s1, 16($a0)
	sw         $s2, 20($a0)
	sw         $s3, 24($a0)
	sw         $s4, 28($a0)
	sw         $s5, 32($a0)
	sw         $s6, 36($a0)
	sw         $s7, 40($a0)
	addu       $v0, $zr, $zr
	jr         $ra
	sll        $zr, $zr, 0

int my_setjmp(int a0)
{
	__asm__ __volatile__ ("\
	.word 0xAC9F0000;\
	.word 0xAC9C002C;\
	.word 0xAC9D0004;\
	.word 0xAC9E0008;\
	.word 0xAC90000C;\
	.word 0xAC910010;\
	.word 0xAC920014;\
	.word 0xAC930018;\
	.word 0xAC94001C;\
	.word 0xAC950020;\
	.word 0xAC960024;\
	.word 0xAC970028;\
	"::);
	return 0;
}
*/
/*
	lw         $ra, 0($a0)
	lw         $gp, 44($a0)
	lw         $sp, 4($a0)
	lw         $fp, 8($a0)
	lw         $s0, 12($a0)
	lw         $s1, 16($a0)
	lw         $s2, 20($a0)
	lw         $s3, 24($a0)
	lw         $s4, 28($a0)
	lw         $s5, 32($a0)
	lw         $s6, 36($a0)
	lw         $s7, 40($a0)
	addu       $v0, $a1, $zr   0x00A01021 '!...'
	jr         $ra             0x03E00008 '....'
	sll        $zr, $zr, 0  

void my_longjmp(int a0, int a1)
{
	__asm__ __volatile__ ("\
	.word 0x8C9F0000;\
	.word 0x8C9C002C;\
	.word 0x8C9D0004;\
	.word 0x8C9E0008;\
	.word 0x8C90000C;\
	.word 0x8C910010;\
	.word 0x8C920014;\
	.word 0x8C930018;\
	.word 0x8C94001C;\
	.word 0x8C950020;\
	.word 0x8C960024;\
	.word 0x8C970028;\
	.word 0x00A01021;\
	"::);
	return;
}
*/
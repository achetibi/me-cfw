/*
 * 
 * 6.3X rebootex.bin
 *
 */

#include <pspsdk.h>
#include "main.h"
#include "../sysmini/sysmini_bin.h"
#include "rebootex_patch_addr.h"

int rtm_flag = 0;
u32 loadcore_text_addr = 0;
int PSP_MODEL;

#define REG32(a) *(volatile unsigned long *)(a)

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define MIPS_JAL(IMM) (0x0C000000 + (u32)(IMM)&0x3FFFFFF)
#define MIPS_J(IMM) (0x08000000 + (u32)(IMM)&0x3FFFFFF)
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a);
#define MIPS_ADDI(RT,RS,IMM)    (0x24000000|(RS<<21)|(RT<<16)|((u32)(IMM)&0xffff))
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MIPS_ADDU(RD,RS,RT) (0x00000021|(RD<<11)|(RT<<16)|(RS<<21))
#define CHANGE_FUNC(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); _sw(0, a+4);

void Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2, void *t3) __attribute__ ((section (".text.start")));

int (* memlmd_E42AFE2E)(void *buf ,void *check , void *s) = NULL;
int (* memlmd_3F2AC9C6)(void *a0,void *a1) = NULL;
	
struct FunctionList function_list;
struct PatchAddrList patch_list;


static void ClearCaches()
{
	int (* DcacheClear)(void) = (void *)function_list.DcacheClearAddr;
	int (* IcacheClear)(void) = (void *)function_list.IcacheClearAddr;

	DcacheClear();
	IcacheClear();
}

static int sceBootLfatOpen(const char *path)
{
	int (* sceBootLfatOpen_k)(const char *)	= (void *)function_list.BootLfatOpen;
	return sceBootLfatOpen_k( path );
}
static int sceBootLfatRead(void *buffer, int maxsize)
{
	int (* sceBootLfatRead_k)(void* , int)	= (void *)function_list.BootLfatRead;
	return sceBootLfatRead_k( buffer , maxsize);
}
static int sceBootLfatClose(void)
{
	int (* sceBootLfatClose_k )(void)		= (void *)function_list.BootLfatClose;
	return sceBootLfatClose_k();
}
static int sceKernelCheckPspConfig(void *buffer , int size , int flag)
{
	int (* sceKernelCheckPspConfig_k)(void *, int, int ) = (void *)function_list.CheckPspConfig;
	return sceKernelCheckPspConfig_k( buffer, size, flag );
}

void *memcpy(void *dst,void *src,int size)
{
	u8 *p1 = (u8 *)dst;
	u8 *p2 = (u8 *)src;
	while(size--)
	{
		*p1++ = *p2++;
	}
	return p1;
}

void memcpy_b(void *dst, void *src, int len)
{
	u8 *d = (u8 *)dst;
	u8 *s = (u8 *)src;
	while(len--) 
	{
		d[len] = s[len];
	} 
}

int memcmp(void *a0, void *a1, int size)
{
	int i;
	u8 *m1 = (u8 *)a0;
	u8 *m2 = (u8 *)a1;

	for (i = 0; i < size; i++)
	{
		if (m1[i] != m2[i])
			return m2[i] - m1[i];
	}

	return 0;
}

int strcmp(char *s, char *t)
{
	while( *s == *t )
	{
		if( *s == '\0' )
			return 0;

		s++;t++;
	}
	return (*s - *t);
}

void *memset(void *dst,int code,int size)
{
	u8 *p1 = (u8 *)dst;
	while(size--)
	{
		*p1++ = code;
	}
	return p1;
}

int sceBootLfatOpenPatched(char *path)
{

	if(memcmp( path , "/rtm.prx" , sizeof("/rtm.prx")) == 0)
	{
		rtm_flag = 1;
		return 0;
	}


	return sceBootLfatOpen( path );
}

int readedsize=0;

int sceBootLfatReadPatched(void *buff , int max_size )//
{
	if( rtm_flag )
	{
		int load_size = size_sysmini;

		if( load_size > max_size)		
			load_size = max_size;

		memcpy( buff , &(sysmini[ readedsize ]) , load_size );

		size_sysmini -= load_size;
		readedsize += load_size;
		return load_size;
	}

	int ret = sceBootLfatRead( buff , max_size );
	return ret;
}

int sceBootLfatClosePatched(void)//close
{

	if(rtm_flag)
	{
		rtm_flag = 0;
		return 0;
	}

	return sceBootLfatClose();
}


int sceKernelCheckPspConfigPatched(BtcnfHeader *a0 , int size , int flag)
{

	int ret = sceKernelCheckPspConfig(a0, size , flag);
	int i , j;
	int module_cnt;

	BtcnfHeader *header = a0;


	if( header->signature == 0x0F803001)// 0x0F803001if btcnf
	{
		module_cnt = header->nmodules;
//		printf("module_cnt:0x%08X\n",module_cnt);
		if( module_cnt > 0)
		{
			ModuleEntry *module_offset = (ModuleEntry *)((u32)header + (u32)(header->modulestart));
			//u32 modinfo_offset = (u32)header + (u32)(header->modulestart);
			char* modname_start = (char *)((u32)header+ header->modnamestart);
		
			for(i=0; i< module_cnt;i++)
			{	
//				printf("%s\n", modname_start + module_offset[i].stroffset);

				if ( memcmp( modname_start + module_offset[i].stroffset ,"/kd/init.prx", 13) == 0)
				{
					memcpy_b( &(module_offset[i+1]) , &(module_offset[i]) , ret - header->modulestart - 32*i);

					ret += 32 + sizeof("/rtm.prx");

					header->nmodules ++;
					header->modnamestart += 0x20;
					header->modnameend += 0x20;

					ModuleEntry *sp = &(module_offset[i]);

					sp->stroffset = header->modnameend - header->modnamestart;
					sp->flags= 0xEF;//flag

					memcpy( (char *)((u32)header + (u32)(header->modnameend)) , "/rtm.prx" , sizeof("/rtm.prx"));
					header->modnameend += sizeof("/rtm.prx");

					int mode_cnt = header->nmodes;
					ModeEntry * mode_entyr = (ModeEntry *)( (u32)header + (u32)(header->modestart));

					for(j=0;j< mode_cnt;j++)
					{
						mode_entyr[j].maxsearch ++;
						mode_entyr[j].searchstart = 0;
					}
					break;
				}
			}
		}
	}

	return ret;
}


int memlmd_E42AFE2E_patched(PSP_Header *buf, int *check,int *s)//read & decrypt patch
{
	if(buf->oe_tag == 0x55668D96 )
	{
		memcpy(buf,&(buf->main_data), buf->comp_size);
		*s = buf->comp_size;

		MAKE_CALL(loadcore_text_addr + rebootex_patch_list.memlmd_list.DecryptPatchAddr , memlmd_E42AFE2E);
		ClearCaches();
		return 0;
	}
	
	return memlmd_E42AFE2E(buf , check , s);	
}

//sub_0x08FC04A4
int memlmd_3F2AC9C6_patched(void *a0,void *a1)//header check
{
	PSP_Header *head=(PSP_Header *)a0;
	int i;

	for(i=0;i<0x58;i++)
	{
		if(head->scheck[i] != 0)
			return memlmd_3F2AC9C6(a0,a1);
	}

	MAKE_CALL(loadcore_text_addr + rebootex_patch_list.memlmd_list.SigcheckPatchAddr , memlmd_3F2AC9C6 );
	ClearCaches();
	return 0;
}

int PatchLoadCore(void *a0, void *a1, void *a2, int (* module_start)(void *, void *, void *))
{
	u32 text_addr = ((u32)module_start) - rebootex_patch_list.memlmd_list.ModuleOffsetAddr;

	MAKE_CALL(text_addr + rebootex_patch_list.memlmd_list.SigcheckPatchAddr , memlmd_3F2AC9C6_patched );
	MAKE_CALL(text_addr + rebootex_patch_list.memlmd_list.DecryptPatchAddr , memlmd_E42AFE2E_patched );

	loadcore_text_addr = text_addr;
	memlmd_3F2AC9C6 = (void *)(text_addr + rebootex_patch_list.memlmd_list.SigcheckFuncAddr );
	memlmd_E42AFE2E = (void *)(text_addr + rebootex_patch_list.memlmd_list.DecryptFuncAddr );

	ClearCaches();//

	return module_start(a0, a1, a2);
}


void Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2, void *t3)
{

	rtm_flag = 0;
	PSP_MODEL = *(int *)0x88FB0000;

	if( PSP_MODEL == 0)
	{
		function_list	= rebootex_patch_list.function_list_01g;
		patch_list		= rebootex_patch_list.patch_list_01g;
	}
	else
	{
		function_list	= rebootex_patch_list.function_list_02g;
		patch_list		= rebootex_patch_list.patch_list_02g;
	}

	MAKE_CALL( patch_list.BootLfatOpenPatch		, sceBootLfatOpenPatched );
	MAKE_CALL( patch_list.BootLfatReadPatch		, sceBootLfatReadPatched );
	MAKE_CALL( patch_list.BootLfatClosePatch	, sceBootLfatClosePatched);
	MAKE_CALL( patch_list.CheckPspConfigPatch	, sceKernelCheckPspConfigPatched);

	//kdebug patch
	_sw( 0x03E00008 , patch_list.KdebugPatchAddr  );
	_sw( 0x24020001 , patch_list.KdebugPatchAddr + 4 );//addiu  $v0, $zr, 1 

	// Patch ~PSP header check
	_sw(0xafa50000, patch_list.BtHeaderPatchAddr	 );//sw $a0, 0($sp) -> sw $a1, 0($sp)
	_sw(0x20a30000, patch_list.BtHeaderPatchAddr + 4 );//addiu v1, zr,-1 -> addi	$v1, $a1, 0

	///patch sceBootLfatfsMount
	_sw(0, patch_list.LfatMountPatchAddr );

	//patch sceBootLfatSeek size
	_sw(0, patch_list.LfatSeekPatchAddr1 );
	_sw(0, patch_list.LfatSeekPatchAddr2 );

	//MIPS_ADDU( 7 , 17 , 0 )//addu $a3,$s1,$zr
	_sw( 0x02203821 ,	patch_list.LoadCorePatchAddr );
	MAKE_JUMP(			patch_list.LoadCorePatchAddr + 4 , PatchLoadCore );
	_sw( 0x02A0E821 ,	patch_list.LoadCorePatchAddr + 8 );//addu  $sp, $s5, $zr

	//patch error
	_sw(0, patch_list.HashCheckPatchAddr );

	/*
	if (PSP_MODEL == 0)
	{
//fat
		sceBootLfatOpen = (void *)0x88608250; //01g
		sceBootLfatRead = (void *)0x886083C4;//01g
		sceBootLfatClose = (void *)0x88608368;//01g
		sceKernelCheckPspConfig = (void *)0x8860569C;//01g

		MAKE_CALL( 0x88602768 , sceBootLfatOpenPatched);//01g
		MAKE_CALL( 0x886027D8 , sceBootLfatReadPatched);//01g
		MAKE_CALL( 0x88602804 , sceBootLfatClosePatched);//01g

		MAKE_CALL( 0x8860711C  , sceKernelCheckPspConfigPatched);//01g

		_sw( 0x03E00008 , 0x88603848  );// -01g
		_sw( 0x24020001 , 0x8860384C );//addiu  $v0, $zr, 1 

		// Patch ~PSP header check - 01g
		_sw(0xafa50000, 0x886056D0 );//sw $a0, 0($sp) -> sw $a1, 0($sp)
		_sw(0x20a30000, 0x886056D4 );//addiu v1, zr,-1 -> addi	$v1, $a1, 0

		//patch sceBootLfatfsMount - 01g
		_sw(0, 0x88602760 );

		//patch sceBootLfatSeek size -01g
		_sw(0, 0x886027B4 );

		//patch buffer size check -01g
		_sw(0, 0x886027CC );

		_sw( MIPS_ADDU( 7 , 17 , 0 ) , 0x88605590 );////addu $a3,$s1,$zr +
		MAKE_JUMP( 0x88605594 , PatchLoadCore );//- 01g
		_sw( 0x02A0E821 , 0x88605598 );//addu  $sp, $s5, $zr +

		//patch hash error - 01g
		_sw(0, 0x886073B4 );
	}
	else if (PSP_MODEL == 1 || PSP_MODEL == 2 || PSP_MODEL == 3 || PSP_MODEL == 4 || PSP_MODEL == 6 || PSP_MODEL == 8 )
	{
		sceBootLfatOpen = (void *)0x88608320; //6.39
		sceBootLfatRead = (void *)0x88608494;//6.39
		sceBootLfatClose= (void *)0x88608438;//6.39
		sceKernelCheckPspConfig = (void *)0x8860576C;//6.39

		MAKE_CALL( 0x88602838 , sceBootLfatOpenPatched);//6.39
		MAKE_CALL( 0x886028A8 , sceBootLfatReadPatched);//6.39
		MAKE_CALL( 0x886028D4 , sceBootLfatClosePatched);//6.39

		MAKE_CALL( 0x886071EC  , sceKernelCheckPspConfigPatched);//6.39

		_sw( 0x03E00008 , 0x88603918  );// -6.39
		_sw( 0x24020001 , 0x8860391C );//addiu  $v0, $zr, 1 

		// Patch ~PSP header check -6.39
		_sw(0xafa50000, 0x886057A0 );//sw $a0, 0($sp) -> sw $a1, 0($sp)
		_sw(0x20a30000, 0x886057A4 );//addiu v1, zr,-1 -> addi	$v1, $a1, 0

		//patch sceBootLfatfsMount -6.39
		_sw(0, 0x88602830 );

		//patch sceBootLfatSeek size -6.39	
		_sw(0, 0x88602884 );

		//patch buffer size check -6.39
		_sw(0, 0x8860289C );

		_sw( MIPS_ADDU( 7 , 17 , 0 ) , 0x88605660 );////addu $a3,$s1,$zr +
		MAKE_JUMP( 0x88605664 , PatchLoadCore );//-6.39
		_sw( 0x02A0E821 , 0x88605668 );//addu  $sp, $s5, $zr +

		//patch hash error -6.39
		_sw(0, 0x88607484 );
	}
	*/

//	REG32(0xbe240008)=0x80;
	ClearCaches();

	void (* go)() = (void *)0x88600000;
	return go(a0, a1, a2, a3, t0, t1, t2, t3);
}




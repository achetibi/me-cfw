/*
 * 
 * 6.XX payloadex.bin
 *
 */

#include <pspsdk.h>
#include "main.h"
#include "inline.h"

#include "payloadex_patch_addr.h"

static int Main(void *, void *, void *, void *, void *, void *, void *);

int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) __attribute__ ((section (".text.startup")));
int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{
	return Main(a0, a1, a2, a3, t0, t1, t2);
}

int elf_load_flag;
int btcnf_load_flag;
int recovery_flag;
u32 hold_key;

u32 loadcore_text_addr = 0;

int (* Real_Reboot)(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) = (void *)0x88600000;

int (* sceKernelCheckExecFile)(void *buf, int *check) =NULL;
int (* memlmd_E42AFE2E)(void *buf ,int size , void *s) = NULL;
int (* memlmd_3F2AC9C6)(void *a0, int size ) = NULL;

#if PSP_MODEL == 0
//fat
#include "../recovery_btcnf_01g.h"
#define BTCNF_PATH "pspbtcnf.bin"
#elif PSP_MODEL == 1
//slim
#include "../recovery_btcnf_02g.h"
#define BTCNF_PATH "pspbtcnf_02g.bin"
#else
#error PSP_MODEL is not defined
#endif


static void ClearCaches()
{
	int (* DcacheClear)(void) = (void *)payloadex_patch_list.function_list.DcacheClearAddr;
	int (* IcacheClear)(void) = (void *)payloadex_patch_list.function_list.IcacheClearAddr;

	DcacheClear();
	IcacheClear();
}

static int sceBootLfatOpen(const char *path)
{
	int (* sceBootLfatOpen_k)(const char *) = (void *)payloadex_patch_list.function_list.BootLfatOpen;
	return sceBootLfatOpen_k( path );
}

static int sceBootLfatRead(void *buff , int max_size)
{
	int (* sceBootLfatRead_k)(void * , int) = (void *)payloadex_patch_list.function_list.BootLfatRead;
	return sceBootLfatRead_k( buff , max_size);
}

static int sceBootLfatClose(void)
{
	int (* sceBootLfatClose_k)(void) = (void *)payloadex_patch_list.function_list.BootLfatClose;
	return sceBootLfatClose_k();
}

//sub_08FB0374
static void* memcpy(void *dst,void *src,int size)
{
	u8 *p1 = (u8 *)dst;
	u8 *p2 = (u8 *)src;
	while(size--)
	{
		*p1++ = *p2++;
	}
	return dst;
}

/*
//sub_08FB03A4:
void memcpy_b(u8 *d, u8 *s, int len)
{
	while(len--) 
	{
		d[len] = s[len];
	} 
}
*/

//sub_08FB0414
static int memcmp(char *m1, char *m2, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (m1[i] != m2[i])
			return m2[i] - m1[i];
	}

	return 0;
}

static void* memset(void *dst,int code,int size)
{
	u8 *p1 = (u8 *)dst;
	while(size--)
	{
		*p1++ = code;
	}
	return dst;
}

int sceBootLfatOpenPatched(char *path)
{
	if( memcmp( path + 4 , BTCNF_PATH , sizeof( BTCNF_PATH )) == 0)
	{
		if(recovery_flag)
		{
			btcnf_load_flag = 1;
			return 0;
		}
		
		path[9] = 'j';
	}

	return sceBootLfatOpen( path );
}

int sceBootLfatReadPatched(void *buff , int max_size )
{
	if( btcnf_load_flag )
	{
		memcpy( buff , recovery_btcnf , size_recovery_btcnf );
		return size_recovery_btcnf;
	}

	return sceBootLfatRead( buff , max_size );
}

int sceBootLfatClosePatched(void)
{
	if(btcnf_load_flag)
	{
		btcnf_load_flag = 0;
		return 0;
	}

	return sceBootLfatClose();
}

static void scramble(unsigned char *target, int size, unsigned char *seed, int seed_size)
{
	int seed_offset = 0;
	unsigned char *end_buffer = target + size;

	while( target < end_buffer )
	{
		if( seed_offset >= seed_size )
			seed_offset = 0;

		*target ^= seed[seed_offset];
		seed_offset++;
		target++;	
	}
}

static void scramble_simple(unsigned int *target, unsigned int *seed, int size )
{
	unsigned int *end_buffer = target + size/sizeof(int);

	while( target < end_buffer )
	{
		*target ^= *seed;

		seed++;
		target++;
	}
}

int memlmd_E42AFE2E_patched(PSP_Header *buf, int size,int *s)//decrypt patch
{
	if(buf->oe_tag == 0xC6BA41D3 )//0x55668D96
	{
		scramble_simple( (u32 *)(&(buf->main_data)) , (u32 *)buf->key_data1 , 0x10 );
		scramble( &(buf->main_data) , buf->comp_size , &(buf->scheck[0x38]) , 0x20 );
		memcpy(buf,&(buf->main_data), buf->comp_size);
		*s = buf->comp_size;

		MAKE_CALL(loadcore_text_addr + payloadex_patch_list.memlmd_list.DecryptPatchAddr , memlmd_E42AFE2E);
		ClearCaches();
		return 0;
	}
	
	return memlmd_E42AFE2E(buf , size , s);	
}


//sub_0x08FC04A4
//Sigcheck
int memlmd_3F2AC9C6_patched(void *a0,int size)
{
	PSP_Header *head=(PSP_Header *)a0;
	int i;

#if _PSP_FW_VERSION == 660
	for(i=0;i<0x30;i++)
#else
	for(i=0;i<0x38;i++)
#endif
	{
		if(head->scheck[i] != 0)
			return memlmd_3F2AC9C6(a0, size);
	}

	MAKE_CALL(loadcore_text_addr + payloadex_patch_list.memlmd_list.SigcheckPatchAddr , memlmd_3F2AC9C6 );
	ClearCaches();
	return 0;
}

int PatchLoadCore(void *a0, void *a1, void *a2, int (* module_start)(void *, void *, void *))
{
	u32 text_addr = ((u32)module_start) - payloadex_patch_list.memlmd_list.ModuleOffsetAddr;

	MAKE_CALL(text_addr + payloadex_patch_list.memlmd_list.SigcheckPatchAddr , memlmd_3F2AC9C6_patched );
	MAKE_CALL(text_addr + payloadex_patch_list.memlmd_list.DecryptPatchAddr , memlmd_E42AFE2E_patched );

	loadcore_text_addr = text_addr;
	memlmd_3F2AC9C6 = (void *)(text_addr + payloadex_patch_list.memlmd_list.SigcheckFuncAddr );
	memlmd_E42AFE2E = (void *)(text_addr + payloadex_patch_list.memlmd_list.DecryptFuncAddr );

	ClearCaches();
	return module_start(a0, a1, a2);
}

//#define SYSCON_CTRL_LTRG      0x00000200
#define SYSCON_CTRL_RTRG      0x00000400
#define SYSCON_CTRL_HOME      0x00001000

static int Main(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{	

	hold_key = *(u32 *)0x88FB0000;
	memset((void *)0x88FB0000 ,0,0x100);

//	hold_key =~hold_key;

	elf_load_flag = 0;
	btcnf_load_flag = 0;
	recovery_flag = 0;

	if ( hold_key & SYSCON_CTRL_HOME)
	{
		hold_key = ~hold_key;

		if( SYSCON_CTRL_RTRG & hold_key)
			recovery_flag = 1;

		MAKE_CALL( payloadex_patch_list.patch_list.BootLfatOpenPatch	, sceBootLfatOpenPatched );
		MAKE_CALL( payloadex_patch_list.patch_list.BootLfatReadPatch	, sceBootLfatReadPatched );
		MAKE_CALL( payloadex_patch_list.patch_list.BootLfatClosePatch	, sceBootLfatClosePatched);
//		MAKE_CALL( payloadex_patch_list.patch_list.CheckPspConfigPatch	, sceKernelCheckPspConfigPatched);

		_sw( 0x03E00008 , payloadex_patch_list.patch_list.KdebugPatchAddr );
		_sw( 0x24020001 , payloadex_patch_list.patch_list.KdebugPatchAddr + 4);//addiu  $v0, $zr, 1

		// Patch ~PSP header check
		_sw(0xafa50000, payloadex_patch_list.patch_list.BtHeaderPatchAddr );//sw $a0, 0($sp) -> sw $a1, 0($sp)
		_sw(0x20a30000, payloadex_patch_list.patch_list.BtHeaderPatchAddr + 4 );//addiu v1, zr,-1 -> addi	$v1, $a1, 0x0000

		///patch sceBootLfatfsMount
		_sw(0, payloadex_patch_list.patch_list.LfatMountPatchAddr );

		//patch sceBootLfatSeek size
		_sw(0, payloadex_patch_list.patch_list.LfatSeekPatchAddr1 );
		_sw(0, payloadex_patch_list.patch_list.LfatSeekPatchAddr2 );

		//MIPS_ADDU( 7 , 15 , 0 )
		_sw( 0x01E03821 ,	payloadex_patch_list.patch_list.LoadCorePatchAddr );//addu $a3,$t7,$zr
		MAKE_JUMP(			payloadex_patch_list.patch_list.LoadCorePatchAddr + 4 , PatchLoadCore );
		_sw( 0x0280E821 ,	payloadex_patch_list.patch_list.LoadCorePatchAddr + 8 );//addu  $sp, $s4, $zr

		//patch hash error
		_sw(0, payloadex_patch_list.patch_list.HashCheckPatchAddr );
	}

	ClearCaches();

	return Real_Reboot(a0, a1, a2, a3, t0, t1, t2);	
}
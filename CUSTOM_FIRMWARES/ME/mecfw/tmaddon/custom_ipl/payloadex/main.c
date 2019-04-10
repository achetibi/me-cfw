/*
 * 
 * 6.3X payloadex.bin
 *
 */

#include <pspsdk.h>
#include "main.h"

#include "../../../custom_ipl/payloadex/payloadex_patch_addr.h"

int Main(void *, void *, void *, void *, void *, void *, void *);

int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) __attribute__ ((section (".text.start")));
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




static void ClearCaches()
{
	int (* DcacheClear)(void) = (void *)payloadex_patch_list.function_list.DcacheClearAddr;
	int (* IcacheClear)(void) = (void *)payloadex_patch_list.function_list.IcacheClearAddr;

	DcacheClear();
	IcacheClear();
}

void* memcpy(void *dst,void *src,int size)
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

int memcmp(char *m1, char *m2, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (m1[i] != m2[i])
			return m2[i] - m1[i];
	}

	return 0;
}

void* memset(void *dst,int code,int size)
{
	u8 *p1 = (u8 *)dst;
	while(size--)
	{
		*p1++ = code;
	}
	return dst;
}

int strlen(const char *s)
{
  const char* ss;
  for (ss = s; *ss != '\0'; ss++)
    ;
  return ss - s;
}

char *strncpy(char *s1, const char *s2, int n)
{
    char  *p = s1;
    while (n) {
        n--;
        if (!(*s1++ = *s2++)) break;    // '\0'‚ðŒ©‚Â‚¯‚½‚çI—¹ 
    }
    while (n--)
        *s1++ = '\0';                   // Žc‚è‚ð'\0'‚Å–„‚ß‚é 
    return (p);
}
/*
int strncmp(const char *s1, const char *s2 , int n )
{
    while (n && *s1 && *s2) {
        if (*s1 != *s2)         // “™‚µ‚­‚È‚¢ 
            return ((unsigned char)*s1 - (unsigned char)*s2);
        s1++;
        s2++;
        n--;
    }
    if (!n)  return (0);
    if (*s1) return (1);
    return (-1);
}
*/
int strcmp(const char *s1, const char *s2)
{
    while (*s1 == *s2) {
        if (*s1 == '\0')
            return (0);
        s1++;
        s2++;
    }
    return ((int)*s1 - (int)*s2);
}

char *strcpy(char *s1, const char *s2)
{
    char *p = s1;
    while ((*s1++ = *s2++)){}
    return (p);
}

typedef struct {
  int quot;
  int rem;
} div_t;

div_t div(int numer, int denom)
{
  div_t result;
  result.quot = numer / denom;
  result.rem = numer % denom;
  return result;
}
typedef struct {
  long quot;
  long rem;
} ldiv_t;

ldiv_t ldiv(long numer, long denom)
{
  ldiv_t result;
  result.quot = numer / denom;
  result.rem = numer - result.quot * denom;
  return result;
}

static char path_buffer[128];

static int sceBootLfatOpen(const char *path)
{
	memcpy( path_buffer , BASE_PATH , sizeof(BASE_PATH) - 1);
	strcpy( path_buffer + sizeof( BASE_PATH ) - 1 , path );
	return open_ms_file( path_buffer );
//	int (* sceBootLfatOpen_k)(const char *) = (void *)payloadex_patch_list.function_list.BootLfatOpen;
//	return sceBootLfatOpen_k( path );
}

static int sceBootLfatRead(void *buff , int max_size)
{
	return read_ms_file( buff , max_size );
//	int (* sceBootLfatRead_k)(void * , int) = (void *)payloadex_patch_list.function_list.BootLfatRead;
//	return sceBootLfatRead_k( buff , max_size);
}

static int sceBootLfatClose(void)
{
	return 0;
//	int (* sceBootLfatClose_k)(void) = (void *)payloadex_patch_list.function_list.BootLfatClose;
//	return sceBootLfatClose_k();
}

int sceBootLfatfsMountPatch(int a0 , int a1 , int a2)
{
	a0 = a0; a1 = a1; a2 = a2;
	
//	int (* sceBootLfatfsMount)() = (void *)0x886049E8;
//	sceBootLfatfsMount( a0 , a1 , a2);
	return init_ms();
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
		
		path[9] = 'd';
	}
	else if( strcmp("/kd/lfatfs.prx", path) == 0 )
	{
		path = TMCTRL_PATH;
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
	if(btcnf_load_flag)//
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

#if _PSP_FW_VERSION == 660 || _PSP_FW_VERSION == 661
	for(i=0;i<0x30;i++)
#else
	for(i=0;i<0x38;i++)
#endif
	{
		if(head->scheck[i] != 0)
			return memlmd_3F2AC9C6(a0, size);
	}

//	MAKE_CALL(loadcore_text_addr + payloadex_patch_list.memlmd_list.SigcheckPatchAddr , memlmd_3F2AC9C6 );
//	ClearCaches();
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

int Main(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{	
	
	hold_key = *(u32 *)0x88FB0000;
	memset((void *)0x88FB0000 ,0,0x100);

//	hold_key =~hold_key;

	elf_load_flag = 0;
	btcnf_load_flag = 0;
	recovery_flag = 0;

//	if ( hold_key & SYSCON_CTRL_HOME)
	{
		hold_key = ~hold_key;

		if( SYSCON_CTRL_RTRG & hold_key)
			recovery_flag = 1;

		MAKE_CALL( MS_MOUNT_PATCH_ADDR									, sceBootLfatfsMountPatch );

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

		_sw( 0x03E00008 , SIGCHECK_PATCH_ADDR		);
		_sw( 0x00001021 , SIGCHECK_PATCH_ADDR + 4	);

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
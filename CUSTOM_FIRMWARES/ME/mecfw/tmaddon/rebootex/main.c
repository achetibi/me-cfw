/*
 * 
 *  rebootex.bin
 *
 */

#include <pspsdk.h>
#include <rebootex_config.h>

#include "main.h"

#include "../../minimum_edition/rebootex/rebootex_patch_addr.h"

int Main(void *, void *, void *, void *, void *, void *, void *);
int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) __attribute__ ((section (".text.start")));
int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{
	return Main(a0, a1, a2, a3, t0, t1, t2);
}

int (* Real_Reboot)(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) = (void *)0x88600000;

#define REG32(a) *(volatile unsigned long *)(a)


#if PSP_MODEL == 0
#define SYSTEMCTRL_PATH "systemctrl_01g.prx"
#define MS_MOUNT_PATCH_ADDR	0x886027B0
#define SIGCHECK_PATCH_ADDR	0x88601620

#elif PSP_MODEL == 1
#define SYSTEMCTRL_PATH "systemctrl_02g.prx"
#define MS_MOUNT_PATCH_ADDR	0x88602878
#define SIGCHECK_PATCH_ADDR	0x886016B0
#endif

int elf_load_flag;
int btcnf_load_flag;
int rtm_flag;

u32 loadcore_text_addr = 0;

char *systemctrl = (char *)0x88FB0100;
u32 size_systemctrl = 0;

char *on_reboot_after = NULL;
void *on_reboot_buf = NULL;
u32 on_reboot_size = 0;
u32 on_reboot_flag = 0;

int boot_index = 0;

int (* memlmd_E42AFE2E)(void *buf ,int size , void *s) = NULL;
int (* memlmd_3F2AC9C6)(void *a0, int size) = NULL;

static void ClearCaches()
{
	int (* DcacheClear)(void) = (void *)rebootex_patch_list.function_list.DcacheClearAddr;
	int (* IcacheClear)(void) = (void *)rebootex_patch_list.function_list.IcacheClearAddr;

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

void memcpy_b(void *dst, void *src, int len)
{
	u8 *d = (u8 *)dst;
	u8 *s = (u8 *)src;
	while(len--) 
	{
		d[len] = s[len];
	} 
}

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

int strcmp(const char *s, const char *t)
{
	while( *s == *t )
	{
		if( *s == '\0' )
			return 0;

		s++;t++;
	}
	return (*s - *t);
}

void* memset(void *dst, u8 code, int size)
{
	u8 *p1 = (u8 *)dst;
	while(size--)
	{
		*p1++ = code;
	}
	return p1;
}


int strlen(char *str)
{
	char *p;
	for(p=str; *p; ++p)
		;

	return (p - str);
}


char *strncpy(char *s1, const char *s2, int n)
{
    char  *p = s1;
    while (n) {
        n--;
        if (!(*s1++ = *s2++)) break;    // '\0'を見つけたら終了 
    }
    while (n--)
        *s1++ = '\0';                   // 残りを'\0'で埋める 
    return (p);
}

char *strcpy(char *s1, const char *s2)
{
    char *p = s1;
    while (*s1++ = *s2++ ){}
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
}

static int sceBootLfatRead(void *buff , int max_size)
{
	return read_ms_file( buff , max_size );
}

static int sceBootLfatClose(void)
{
	return 0;
}

static inline int pspSysconCtrlMsPower(u8 sw)
{
	return pspSyscon_tx_dword(sw,0x4c,3);
}

int sceBootLfatfsMountPatch(int a0 , int a1 , int a2)
{
	a0=a0;a1=a1;a2=a2;	
	return init_ms();
}

int sceBootLfatOpenPatched(char *path)
{
//	REG32(0xBE240008) = 0x80;//LAN

	if( memcmp( path + 4 , SYSTEMCTRL_PATH , sizeof( SYSTEMCTRL_PATH )) == 0)
	{
		if(size_systemctrl)
		{	
			elf_load_flag = 1;	
			return 0;
		}
	}
	else if( memcmp( path + 4 , "pspbtcnf" , sizeof( "pspbtcnf" )-1 ) == 0)
	{
		path[9] = 'j';
		btcnf_load_flag = 1;
	}
	else if(memcmp( path , "/rtm.prx" , sizeof("/rtm.prx")) == 0)
	{
		rtm_flag = 1;
		return 0;
	}
	else if(memcmp( path , "/kd/lfatfs.prx" , sizeof("/kd/lfatfs.prx")) == 0)
	{
		path = TMCTRL_PATH;
	}
	else if( boot_index == 1 )
	{
		if(memcmp( path + 4 , "isotope.prx" , sizeof("isotope.prx")) == 0)
		{
			memcpy( path + 4  , "dax9660.prx" , sizeof("dax9660.prx") );
		}
	}

	return sceBootLfatOpen( path );
}

#include "btcnf.h"

ModuleList module_rtm[] = {
	{ PATCH_ADD	, "" , "/rtm.prx",   0  , 0x8001 },
	{ -1		,NULL			, NULL					, 0  , 0}
};

int sceBootLfatReadPatched(void *buff , int max_size )
{
	if( elf_load_flag )//elf load flag1
	{
		int load_size = size_systemctrl;
		if( load_size > max_size)		
			load_size = max_size;

		memcpy( buff , systemctrl , size_systemctrl );

		size_systemctrl -= load_size;
		systemctrl += load_size;

		return load_size;
	}
	else if( rtm_flag )
	{
		int load_size = on_reboot_size;

		if( load_size > max_size)		
			load_size = max_size;

		memcpy( buff , on_reboot_buf , load_size );

		on_reboot_size -= load_size;
		on_reboot_buf += load_size;
		return load_size;
	}

	int ret = sceBootLfatRead( buff , max_size );

	if( btcnf_load_flag )
	{
		switch(boot_index)
		{
		case 2://np9660
			ret = btcnf_patch( buff , ret ,module_np9660 , 0x40 , 2 );
			break;
		case 1://M33
		case 3://me	
			ret = btcnf_patch( buff , ret ,module_isotope , 0x40 , 2 );
			break;
        case 5://Inferno
            ret = btcnf_patch(buff, ret, module_inferno, 0x40, 2);
            break;
		}

		if( boot_index == 4 )
		{
			ret = btcnf_patch( buff , ret ,module_recovery , 0 , 0 );
		}
		else if( on_reboot_after )
		{
			module_rtm[0].before_path = on_reboot_after;
			module_rtm[0].flag = on_reboot_flag;

			ret = btcnf_patch( buff , ret ,module_rtm , 0 , 0 );
		}

		btcnf_load_flag = 0;
	}
	return ret;
}

int sceBootLfatClosePatched(void)
{
	if(elf_load_flag)
	{
		elf_load_flag = 0;
		return 0;
	}
	else if(rtm_flag)
	{
		rtm_flag = 0;
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
		int data_size = buf->comp_size;
		scramble_simple( (u32 *)(&(buf->main_data)) , (u32 *)buf->key_data1 , 0x10 );
		scramble( &(buf->main_data) , data_size , &(buf->scheck[0x38]) , 0x20 );
		memcpy(buf,&(buf->main_data), data_size );
		*s = data_size;

		MAKE_CALL(loadcore_text_addr + rebootex_patch_list.memlmd_list.DecryptPatchAddr , memlmd_E42AFE2E);
		ClearCaches();
		return 0;
	}
	
	return memlmd_E42AFE2E(buf , size , s);	
}

//Sigcheck
int memlmd_3F2AC9C6_patched(void *a0, int size )
{
	PSP_Header *head=(PSP_Header *)a0;
	int i;

	for(i=0;i<0x30;i++)
	{
		if(head->scheck[i] != 0)
			return memlmd_3F2AC9C6(a0, size );
	}

//	MAKE_CALL(loadcore_text_addr + rebootex_patch_list.memlmd_list.SigcheckPatchAddr , memlmd_3F2AC9C6 );
//	ClearCaches();
	return 0;
}

//
//sub_0x08FB081C
int PatchLoadCore(void *a0, void *a1, void *a2, int (* module_start)(void *, void *, void *))
{
	u32 text_addr = ((u32)module_start) - rebootex_patch_list.memlmd_list.ModuleOffsetAddr;

	MAKE_CALL(text_addr + rebootex_patch_list.memlmd_list.SigcheckPatchAddr , memlmd_3F2AC9C6_patched );
	MAKE_CALL(text_addr + rebootex_patch_list.memlmd_list.DecryptPatchAddr , memlmd_E42AFE2E_patched );

	loadcore_text_addr = text_addr;
	memlmd_3F2AC9C6 = (void *)(text_addr + rebootex_patch_list.memlmd_list.SigcheckFuncAddr );
	memlmd_E42AFE2E = (void *)(text_addr + rebootex_patch_list.memlmd_list.DecryptFuncAddr );

	ClearCaches();

	return module_start(a0, a1, a2);
}


int Main(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{
	
	struct RebootexParam *rebootex_param = (void *)REBOOTEX_PARAM_OFFSET;

	size_systemctrl = *(u32 *)0x88FB00F0;

	if( size_systemctrl == 0xFFFFFFFF )
		size_systemctrl = 0;


	boot_index = rebootex_param->reboot_index;

	on_reboot_after = rebootex_param->on_reboot_after;
	on_reboot_buf	= rebootex_param->on_reboot_buf;
	on_reboot_size	= rebootex_param->on_reboot_size;
	on_reboot_flag	= rebootex_param->on_reboot_flag;

	elf_load_flag = 0;
	btcnf_load_flag = 0;
	rtm_flag = 0;


	MAKE_CALL( MS_MOUNT_PATCH_ADDR									, sceBootLfatfsMountPatch );

	MAKE_CALL( rebootex_patch_list.patch_list.BootLfatOpenPatch		, sceBootLfatOpenPatched );//
	MAKE_CALL( rebootex_patch_list.patch_list.BootLfatReadPatch		, sceBootLfatReadPatched );//
	MAKE_CALL( rebootex_patch_list.patch_list.BootLfatClosePatch	, sceBootLfatClosePatched);//
//	MAKE_CALL( rebootex_patch_list.patch_list.CheckPspConfigPatch	, sceKernelCheckPspConfigPatched);//

	//kdebug patch
	_sw( 0x03E00008 , rebootex_patch_list.patch_list.KdebugPatchAddr  );
	_sw( 0x24020001 , rebootex_patch_list.patch_list.KdebugPatchAddr + 4 );//addiu  $v0, $zr, 1 

	// Patch ~PSP header check - 01g
	_sw(0xafa50000, rebootex_patch_list.patch_list.BtHeaderPatchAddr	 );//sw $a0, 0($sp) -> sw $a1, 0($sp)
	_sw(0x20a30000, rebootex_patch_list.patch_list.BtHeaderPatchAddr + 4 );//addiu v1, zr,-1 -> addi	$v1, $a1, 0

	///patch sceBootLfatfsMount - 01g
	_sw(0, rebootex_patch_list.patch_list.LfatMountPatchAddr );

	//patch sceBootLfatSeek size -01g
	_sw(0, rebootex_patch_list.patch_list.LfatSeekPatchAddr1 );
	_sw(0, rebootex_patch_list.patch_list.LfatSeekPatchAddr2 );

	_sw( 0x03E00008 , SIGCHECK_PATCH_ADDR		);
	_sw( 0x00001021 , SIGCHECK_PATCH_ADDR + 4	);

	//MIPS_ADDU( 7 , 17 , 0 )//addu $a3,$s1,$zr
	_sw( 0x02203821 ,	rebootex_patch_list.patch_list.LoadCorePatchAddr );
	MAKE_JUMP(			rebootex_patch_list.patch_list.LoadCorePatchAddr + 4 , PatchLoadCore );
	_sw( 0x02A0E821 ,	rebootex_patch_list.patch_list.LoadCorePatchAddr + 8 );//addu  $sp, $s5, $zr

	//patch error
	_sw(0, rebootex_patch_list.patch_list.HashCheckPatchAddr );

	ClearCaches();

	
//	REG32(0xBC100058) |= 0x2;
//	REG32(0xBC10007C) |= 0xC8;
	pspSyscon_init();
	pspSysconCtrlMsPower(1);

	return Real_Reboot(a0, a1, a2, a3, t0, t1, t2);	
}
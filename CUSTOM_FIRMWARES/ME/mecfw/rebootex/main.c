/*
 * 
 *  rebootex.bin
 *
 */

#include <pspsdk.h>
#include <rebootex_config.h>

#include "main.h"

#include "rebootex_patch_addr.h"

static int Main(void *, void *, void *, void *, void *, void *, void *);
int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) __attribute__ ((section (".text.startup")));
int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{
	return Main(a0, a1, a2, a3, t0, t1, t2);
}

int (* Real_Reboot)(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) = (void *)0x88600000;

#if PSP_MODEL == 0
#define SYSTEMCTRL_PATH "systemctrl_01g.prx"
#elif PSP_MODEL == 1
#define SYSTEMCTRL_PATH "systemctrl_02g.prx"
/*
#elif PSP_MODEL == 2//03g
#define SYSTEMCTRL_PATH "systemctrl_03g.prx"
*/
#elif PSP_MODEL == 4
#define SYSTEMCTRL_PATH "systemctrl_05g.prx"
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

static int sceBootLfatOpen(const char *path)
{
	int (* sceBootLfatOpen_k)(const char *)	= (void *)rebootex_patch_list.function_list.BootLfatOpen;
	return sceBootLfatOpen_k( path );
}
static int sceBootLfatRead(void *buffer, int maxsize)
{
	int (* sceBootLfatRead_k)(void* , int)	= (void *)rebootex_patch_list.function_list.BootLfatRead;
	return sceBootLfatRead_k( buffer , maxsize);
}
static int sceBootLfatClose(void)
{
	int (* sceBootLfatClose_k )(void)		= (void *)rebootex_patch_list.function_list.BootLfatClose;
	return sceBootLfatClose_k();
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
		{
			return m2[i] - m1[i];
		}
	}

	return 0;
}

int strcmp(const char *s, const char *t)
{
	while(*s == *t)
	{
		if(*s == '\0')
		{
			return 0;
		}

		s++;
		t++;
	}

	return (*s - *t);
}

/*
void* memset(void *dst, u8 code, int size)
{
	u8 *p1 = (u8 *)dst;
	while(size--)
	{
		*p1++ = code;
	}
	return p1;
}
*/

int strlen(char *str)
{
	char *p;
	for(p = str; *p; ++p);

	return (p - str);
}

int sceBootLfatOpenPatched(char *path)
{
	if(memcmp(path + 4, SYSTEMCTRL_PATH, sizeof(SYSTEMCTRL_PATH)) == 0)
	{
		if(size_systemctrl)
		{	
			elf_load_flag = 1;	
			return 0;
		}
	}
	else if(memcmp(path + 4, "pspbtcnf", sizeof("pspbtcnf") - 1) == 0)
	{
		path[9] = 'j';
		btcnf_load_flag = 1;
	}
	else if(memcmp(path, "/rtm.prx", sizeof("/rtm.prx")) == 0)
	{
		rtm_flag = 1;
		return 0;
	}
	else if(boot_index == 1)
	{
		if(memcmp(path + 4, "isotope.prx", sizeof("isotope.prx")) == 0)
		{
			memcpy(path + 4, "dax9660.prx", sizeof("dax9660.prx"));
		}
	}

	return sceBootLfatOpen(path);
}

#include "btcnf.h"

ModuleList module_rtm[] = {
	{ PATCH_ADD,	"" ,	"/rtm.prx",	0,	0x8001 },
	{ -1,			NULL,	NULL,		0,	0 }
};

int sceBootLfatReadPatched(void *buff, int max_size)
{
	if(elf_load_flag)//elf load flag1
	{
		int load_size = size_systemctrl;
		if(load_size > max_size)
		{
			load_size = max_size;
		}

		memcpy(buff, systemctrl, size_systemctrl);

		size_systemctrl -= load_size;
		systemctrl += load_size;

		return load_size;
	}
	else if(rtm_flag)
	{
		int load_size = on_reboot_size;
		if(load_size > max_size)
		{
			load_size = max_size;
		}

		memcpy(buff, on_reboot_buf, load_size);

		on_reboot_size -= load_size;
		on_reboot_buf += load_size;

		return load_size;
	}

	int ret = sceBootLfatRead(buff, max_size);

	if(btcnf_load_flag)
	{
		switch(boot_index)
		{
			case 2://NP9660
				ret = btcnf_patch(buff, ret, module_np9660, 0x40, 2);
				break;
			case 1://M33
			case 3://ME	
				ret = btcnf_patch(buff, ret, module_isotope, 0x40, 2);
				break;
			case 5://Inferno
				ret = btcnf_patch(buff, ret, module_inferno, 0x40, 2);
				break;
		}

		if(boot_index == 4)
		{
			ret = btcnf_patch(buff, ret, module_recovery, 0, 0);
		}
		else if(on_reboot_after)
		{
			module_rtm[0].before_path = on_reboot_after;
			module_rtm[0].flag = on_reboot_flag;
			ret = btcnf_patch(buff, ret, module_rtm, 0, 0);
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

	while(target < end_buffer)
	{
		if(seed_offset >= seed_size)
		{
			seed_offset = 0;
		}

		*target ^= seed[seed_offset];
		seed_offset++;
		target++;	
	}
}

static void scramble_simple(unsigned int *target, unsigned int *seed, int size )
{
	unsigned int *end_buffer = target + size/sizeof(int);

	while(target < end_buffer)
	{
		*target ^= *seed;
		seed++;
		target++;
	}
}

int memlmd_E42AFE2E_patched(PSP_Header *buf, int size,int *s)//decrypt patch
{
	if(buf->oe_tag == 0xC6BA41D3)//0x55668D96
	{
		int data_size = buf->comp_size;

		scramble_simple((u32 *)(&(buf->main_data)), (u32 *)buf->key_data1, 0x10);
		scramble(&(buf->main_data), data_size, &(buf->scheck[0x38]), 0x20);

		memcpy(buf, &(buf->main_data), data_size);

		*s = data_size;

		MAKE_CALL(loadcore_text_addr + rebootex_patch_list.memlmd_list.DecryptPatchAddr, memlmd_E42AFE2E);
		ClearCaches();

		return 0;
	}
	
	return memlmd_E42AFE2E(buf , size , s);	
}

//Sigcheck
int memlmd_3F2AC9C6_patched(void *a0, int size )
{
	int i;
	PSP_Header *head = (PSP_Header *)a0;

	for(i = 0; i < 0x30; i++)
	{
		if(head->scheck[i] != 0)
		{
			return memlmd_3F2AC9C6(a0, size);
		}
	}

	MAKE_CALL(loadcore_text_addr + rebootex_patch_list.memlmd_list.SigcheckPatchAddr, memlmd_3F2AC9C6);
	ClearCaches();

	return 0;
}

//
//sub_0x08FB081C
int PatchLoadCore(void *a0, void *a1, void *a2, int (* module_start)(void *, void *, void *))
{
	u32 text_addr = ((u32)module_start) - rebootex_patch_list.memlmd_list.ModuleOffsetAddr;

	MAKE_CALL(text_addr + rebootex_patch_list.memlmd_list.SigcheckPatchAddr, memlmd_3F2AC9C6_patched);
//	MAKE_CALL(text_addr + 0x00005CC8 - 0x00000BBC, memlmd_3F2AC9C6_patched);
//	MAKE_CALL(text_addr + 0x00005CF8 - 0x00000BBC, memlmd_3F2AC9C6_patched);
//	MAKE_CALL(text_addr + 0x00005D90 - 0x00000BBC, memlmd_3F2AC9C6_patched);

//	MAKE_CALL(text_addr + 0x000041A4 - 0x00000BBC, memlmd_E42AFE2E_patched);
//	MAKE_CALL(text_addr + 0x00005CA4 - 0x00000BBC, memlmd_E42AFE2E_patched);
	MAKE_CALL(text_addr + rebootex_patch_list.memlmd_list.DecryptPatchAddr, memlmd_E42AFE2E_patched);

	loadcore_text_addr = text_addr;

	memlmd_3F2AC9C6 = (void *)(text_addr + rebootex_patch_list.memlmd_list.SigcheckFuncAddr);
	memlmd_E42AFE2E = (void *)(text_addr + rebootex_patch_list.memlmd_list.DecryptFuncAddr);

	ClearCaches();

	return module_start(a0, a1, a2);
}

static int Main(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{	
	struct RebootexParam *rebootex_param = (void *)REBOOTEX_PARAM_OFFSET;

	size_systemctrl = *(u32 *)0x88FB00F0;

	if(size_systemctrl == 0xFFFFFFFF)
	{
		size_systemctrl = 0;
	}

	boot_index		= rebootex_param->reboot_index;
	on_reboot_after = rebootex_param->on_reboot_after;
	on_reboot_buf	= rebootex_param->on_reboot_buf;
	on_reboot_size	= rebootex_param->on_reboot_size;
	on_reboot_flag	= rebootex_param->on_reboot_flag;

	elf_load_flag = 0;
	btcnf_load_flag = 0;
	rtm_flag = 0;

	MAKE_CALL(rebootex_patch_list.patch_list.BootLfatOpenPatch, sceBootLfatOpenPatched);//01g
	MAKE_CALL(rebootex_patch_list.patch_list.BootLfatReadPatch, sceBootLfatReadPatched);//01g
	MAKE_CALL(rebootex_patch_list.patch_list.BootLfatClosePatch, sceBootLfatClosePatched);//01g
//	MAKE_CALL(rebootex_patch_list.patch_list.CheckPspConfigPatch, sceKernelCheckPspConfigPatched);//01g

	//kdebug patch
	_sw( 0x03E00008, rebootex_patch_list.patch_list.KdebugPatchAddr);
	_sw( 0x24020001, rebootex_patch_list.patch_list.KdebugPatchAddr + 4);//addiu  $v0, $zr, 1 

	// Patch ~PSP header check - 01g
	_sw(0xafa50000, rebootex_patch_list.patch_list.BtHeaderPatchAddr);//sw $a0, 0($sp) -> sw $a1, 0($sp)
	_sw(0x20a30000, rebootex_patch_list.patch_list.BtHeaderPatchAddr + 4);//addiu v1, zr,-1 -> addi	$v1, $a1, 0

	//patch sceBootLfatfsMount - 01g
	_sw(0x00000000, rebootex_patch_list.patch_list.LfatMountPatchAddr);

	//patch sceBootLfatSeek size -01g
	_sw(0x00000000, rebootex_patch_list.patch_list.LfatSeekPatchAddr1);
	_sw(0x00000000, rebootex_patch_list.patch_list.LfatSeekPatchAddr2);

	//MIPS_ADDU( 7 , 17 , 0 )//addu $a3,$s1,$zr
	_sw(0x02203821, rebootex_patch_list.patch_list.LoadCorePatchAddr);
	MAKE_JUMP(rebootex_patch_list.patch_list.LoadCorePatchAddr + 4, PatchLoadCore);
	_sw(0x02A0E821, rebootex_patch_list.patch_list.LoadCorePatchAddr + 8);//addu  $sp, $s5, $zr

	//patch error
	_sw(0x00000000, rebootex_patch_list.patch_list.HashCheckPatchAddr);

	ClearCaches();

	return Real_Reboot(a0, a1, a2, a3, t0, t1, t2);	
}
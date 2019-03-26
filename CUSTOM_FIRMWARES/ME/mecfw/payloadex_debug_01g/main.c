/*
 * 
 * 6.37 payloadex.bin
 *
 */

#include <pspsdk.h>
#include "main.h"

#include "../systemctrl/systemctrl_01g_bin.h"
unsigned char *sysmini = (u8 *)systemctrl_01g;
static unsigned int size_sysmini = sizeof(systemctrl_01g);


//#include "sysmini_bin.h"
//#include "systemctrl_01g_bin.h"

#include "payloadex_patch_addr.h"
//#include "recovery_btcnf.h"

int Main(void *, void *, void *, void *, void *, void *, void *);

int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) __attribute__ ((section (".text.start")));
int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{
	return Main(a0, a1, a2, a3, t0, t1, t2);
}

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define MIPS_JAL(IMM) (0xC000000 + (u32)(IMM)&0x3FFFFFF)
#define MIPS_ADDI(RT,RS,IMM)    (0x24000000|(RS<<21)|(RT<<16)|((u32)(IMM)&0xffff))
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 

#define MIPS_ADDU(RD,RS,RT) (0x00000021|(RD<<11)|(RT<<16)|(RS<<21))

//#define CHANGE_FUNC(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); _sw(0, a+4);

int elf_load_flag;//0x88FB6918
int btcnf_load_flag;
int psp_model;
u32 hold_key;

int (* Real_Reboot)(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) = (void *)0x88600000;

int (* sceKernelCheckExecFile)(void *buf, int *check) =NULL;
int (* memlmd_E42AFE2E)(void *buf ,void *check , void *s) = NULL;
int (* memlmd_3F2AC9C6)(void *a0,void *a1) = NULL;

static int sceKernelCheckPspConfig(void *a0 , int size , int flag)
{
	int (* sceKernelCheckPspConfig_k)(void *a0 , int size , int flag) = (void *)payloadex_patch_list.function_list.CheckPspConfig;
	return sceKernelCheckPspConfig_k( a0, size, flag);
}


void ClearCaches()
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
void *memcpy(void *dst,void *src,int size)
{
	u8 *p1 = (u8 *)dst;
	u8 *p2 = (u8 *)src;
	while(size--)
	{
		*p1++ = *p2++;
	}
	return dst;
}

//sub_08FB03A4:
void memcpy_b(u8 *d, u8 *s, int len)
{
	while(len--) 
	{
		d[len] = s[len];
	} 
}

//sub_08FB0414
int memcmp(u8 *m1, u8 *m2, int size)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (m1[i] != m2[i])
			return m2[i] - m1[i];
	}

	return 0;
}

void *memset(void *dst,u8 code,int size)
{
	u8 *p1 = (u8 *)dst;
	while(size--)
	{
		*p1++ = code;
	}
	return dst;
}

//sub_0x08FB0460
int sceBootLfatOpenPatched(char *path)//open
{

	if( memcmp( path + 4, "systemctrl" , sizeof("systemctrl") - 1 ) == 0)
	{
		elf_load_flag = 1;
		return 0;
	}

	if( memcmp( path + 4 , "pspbtcnf" , sizeof("pspbtcnf") - 1 ) == 0)
	{
		path[9] = 'j';

//		btcnf_load_flag = 1;//
//		return 0;
	}

	return sceBootLfatOpen( path );
}

char *read_offset =(char *)0x88FB0100; 
int sceBootLfatReadPatched(void *buff , int max_size )//load &decompress patch
{
	if( elf_load_flag )
	{
		
		int ret_size;
		ret_size = size_sysmini;
		if( ret_size >  max_size)
			ret_size = max_size;

		memcpy( buff ,  read_offset ,  ret_size  );
//		memcpy( buff , systemctrl_01g , size_systemctrl_01g );

		size_sysmini -= ret_size;
		read_offset += ret_size;
		return (int)ret_size;
	//	return size_systemctrl_01g;
	}
/*
	if( btcnf_load_flag )
	{
//		memcpy( buff , recovery_btcnf , size_recovery_btcnf );
//		return size_recovery_btcnf;

		memcpy( buff , pspbtcnf , size_pspbtcnf );
		return size_pspbtcnf;
	}
*/
	return sceBootLfatRead( buff , max_size );//0x88FB6924
}
//sub_0x08FB0504
int sceBootLfatClosePatched(void)//close
{
	if(elf_load_flag)//0x88FB6918
	{
		*( volatile unsigned long *)(0xbe240008) = 0x80;
		elf_load_flag = 0;//data0x88FB6918
		return 0;
	}
/*
	if(btcnf_load_flag)//
	{
		btcnf_load_flag = 0;//
		return 0;
	}
*/
	return sceBootLfatClose();//(void *)0x88FB6920
}
//sub_0x08FB052C

int sceKernelCheckPspConfigPatched(BtcnfHeader *a0 , int size , int flag)//decrypt patch
{

	int ret = sceKernelCheckPspConfig(a0, size , flag);//
	int i , j;
	int module_cnt;

	BtcnfHeader *header = a0;

//	if( 0 )
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

					ret += 32 + sizeof("/kd/systemctrl_01g.prx");//(32 + 19)

					header->nmodules ++;
					header->modnamestart += 0x20;
					header->modnameend += 0x20;

					ModuleEntry *sp = &(module_offset[i]);

					sp->stroffset = header->modnameend - header->modnamestart;
					sp->flags= 0xEF;//flag
					//sp->loadmode=1;
					//sp->loadmode2=0x80;

					memcpy( (char *)((u32)header + (u32)(header->modnameend)) , "/kd/systemctrl_01g.prx" , sizeof("/kd/systemctrl_01g.prx"));
					header->modnameend += sizeof("/kd/systemctrl_01g.prx");//sizeof("/kd/systemctrl.prx")+1

					int mode_cnt = header->nmodes;
					ModeEntry * mode_entyr = (ModeEntry *)( (u32)header + (u32)(header->modestart));

					for(j=0;j< mode_cnt;j++)
					{
						mode_entyr[j].maxsearch ++;
						mode_entyr[j].searchstart = 0;
					}
	

					break;
		
					modname_start =(u32)header+ (u32)(header->modnamestart);

				}
			}

		}	
	}
		
	return ret;
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

int memlmd_E42AFE2E_patched(PSP_Header *buf, int *check,int *s)//read & decrypt patch
{
	if(buf->oe_tag == 0x55668D96 )
	{
		memcpy(buf,&(buf->main_data), buf->comp_size);
		s[0] = buf->comp_size;
		return 0;
	}
	else if(buf->oe_tag == 0xC6BA41D3 )
	{
		scramble_simple( (u32 *)(&(buf->main_data)) , (u32 *)buf->key_data1 , 0x10 );
		scramble( &(buf->main_data) , buf->comp_size , &(buf->scheck[0x38]) , 0x20 );
		memcpy(buf,&(buf->main_data), buf->comp_size);
		s[0] = buf->comp_size;
		return 0;
	}

	
	return memlmd_E42AFE2E(buf , check , s);	
}

//sub_0x08FC04A4
int memlmd_3F2AC9C6_patched(void *a0,void *a1)//header check
{
	PSP_Header *head=(PSP_Header *)a0;
	int i;

	for(i=0;i<0x30;i++)
	{
		if(head->scheck[i] != 0)
			return memlmd_3F2AC9C6(a0,a1);
	}

	return 0;
}

int PatchLoadCore(void *a0, void *a1, void *a2, int (* module_start)(void *, void *, void *))
{
	u32 text_addr = ((u32)module_start) - payloadex_patch_list.memlmd_list.ModuleOffsetAddr;

	MAKE_CALL(text_addr + payloadex_patch_list.memlmd_list.SigcheckPatchAddr , memlmd_3F2AC9C6_patched );
//	MAKE_CALL(text_addr + 0x00005CC8 - 0x00000BBC , memlmd_3F2AC9C6_patched );
//	MAKE_CALL(text_addr + 0x00005CF8 - 0x00000BBC , memlmd_3F2AC9C6_patched );
//	MAKE_CALL(text_addr + 0x00005D90 - 0x00000BBC , memlmd_3F2AC9C6_patched );

//	MAKE_CALL(text_addr + 0x000041A4 - 0x00000BBC , memlmd_E42AFE2E_patched );
//	MAKE_CALL(text_addr + 0x00005CA4 - 0x00000BBC , memlmd_E42AFE2E_patched );
	MAKE_CALL(text_addr + payloadex_patch_list.memlmd_list.DecryptPatchAddr , memlmd_E42AFE2E_patched );

//	memlmd_3F2AC9C6 = (void *)(text_addr + 0x00007AE8 );
//	memlmd_E42AFE2E = (void *)(text_addr + 0x00007B08 );
	memlmd_3F2AC9C6 = (void *)(text_addr + payloadex_patch_list.memlmd_list.SigcheckFuncAddr );
	memlmd_E42AFE2E = (void *)(text_addr + payloadex_patch_list.memlmd_list.DecryptFuncAddr );

	ClearCaches();//

	return module_start(a0, a1, a2);
}


#define SYSCON_CTRL_LTRG      0x00000200
#define SYSCON_CTRL_RTRG      0x00000400
#define SYSCON_CTRL_HOME      0x00001000

int Main(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{	

	psp_model =  *(int *)0x88FB0004;
	hold_key = *(u32 *)0x88FB0000;

	memset(0x88FB0000 ,0,0x100);

	*(u32 *)0x88FB00F0 = size_sysmini;
//	*(u32 *)0x88FB00F0 = 0x88FB0100;


	memset( 0x88FB0100 , 0 , size_sysmini);
	memcpy( 0x88FB0100 , sysmini , size_sysmini );

	elf_load_flag = 0;//
	btcnf_load_flag = 0;
		*( volatile unsigned long *)(0xbe240008) = 0x80;

//	if(0)
	{

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

		//patch sceBootLfatSeek size -6.38
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
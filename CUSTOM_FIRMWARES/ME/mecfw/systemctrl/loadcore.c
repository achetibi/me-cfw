#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psputilsforkernel.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

int Patch_Init(int (* real_init)(),  u32 a1)//init patch
;
//int sceKernelCheckExecFile(void *buf, int *check);
u32 sceKernelCheckExecFile[4];

;
int (* LinkLibraryEntries)();
int (* ProbeExec1)(void *buf, u32 *check);
int (* ProbeExec2)(u8 *buf, u32 *check);


static inline int IsStaticElf(void *buf)
{
	Elf32_Ehdr *header = (Elf32_Ehdr *)buf;
	if (header->e_magic == 0x464C457F 
		&& header->e_type == 2) {
			return 1;
	}

	return 0;
}

//get attr in PSP_MODULE_INFO
static int PatchExec2(void *buf, int *check)
{
//	printf("%s", __func__ );
	int index = check[0x4C/4];

	if (index < 0) {
		index += 3;
	}

	u32 addr = (u32)(buf + index);
	if (addr >= 0x88400000 && addr < 0x88800000) {
		return 0;
	}

	check[0x58/4] = *((u16 *)addr);

	return *((u32 *)addr);
}

static int PatchExec3(void *buf, int *check, int isPlain, int res)
{
	//if elf or not
	if ( isPlain ) {
		if ( (u32)check[8/4] >= 0x52) {
			if ( IsStaticElf(buf) ) {
				//3 - 0x2
				//1 - 0xFFA0
				check[0x20/4] = 3;
			}

			return res;
		}

		if ( PatchExec2(buf, check) & 0x0000FF00 ) {
			check[0x44/4] = 1;
			return 0;
		}
	}

	return res;
}

static int PatchExec1(void *buf, int *check)
{
	int api_type = check[8/4];

	if ( api_type >= 0x120) {
		if (   (api_type >= 0x120 && api_type <= 0x126)
			|| (api_type >= 0x140 && api_type <= 0x143)	
#if PSP_MODEL == 4
			|| (api_type >= 0x151 && api_type <= 0x154) 
#endif
			)
		{
			//~PSP filesize?
			if (check[0x10/4] == 0) {
				//kernel flag
				if (check[0x44/4] != 0) {
					
					check[0x48/4] = 1;//check done flag?
					return 0;
				} 
			
				return -1;
			}

			check[0x48/4] = 1;
			check[0x44/4] = 1;
			PatchExec2(buf, check);
			return 0;
		}

		return -1;

	}
	else if ( api_type >= 0x52) {
		return -1;
	}

	if (check[0x44/4] != 0) { 
		check[0x48/4] = 1; 
		return 0; 
	} 
	
	return -2;
}

int sceKernelCheckExecFilePatched(int *buf, int *check)
{
	PSP_Header *head=(PSP_Header *)buf;
	int isPlain = (head->signature == 0x464C457F /* ELF */);
	int res;

	if( isPlain ) {
		if( PatchExec1(buf, check) == 0) {
			return 0;
		}
	}

	res = ((int(*)(void*,void*))sceKernelCheckExecFile)(buf, check);
	return PatchExec3(buf, check, isPlain, res);
}


int ProbeExec1Patched(void *buf, u32 *check)
{
	int res; 
	u16 attr;
	u16 *modinfo;
	u16 realattr;

	res = ProbeExec1(buf, check);

	if (((u32 *)buf)[0] != 0x464C457F /* ELF */)
		return res;

	modinfo = (u16 *)(buf + check[0x4C/4] );

	realattr = *modinfo;
	attr = realattr & 0x1E00;
	
	if (attr != 0)
	{
		u16 attr2 = ((u16 *)check)[0x58/2];
		attr2 &= 0x1e00;
		
		if (attr2 != attr) {
			((u16 *)check)[0x58/2] = realattr;
		}
	}

	if (check[0x48/4] == 0) {
		check[0x48/4] = 1;	
	}
	
	return res;
}

static char *GetStrTab(u8 *buf)
{
	Elf32_Ehdr *header = (Elf32_Ehdr *)buf;
	int i;

	if (header->e_magic != 0x464C457F /* ELF */) {
		return NULL;
	}

	u8 *pData = buf+header->e_shoff;

	for (i = 0; i < header->e_shnum; i++) {
		if (header->e_shstrndx == i) {
			Elf32_Shdr *section = (Elf32_Shdr *)pData;

			if (section->sh_type == 3) {
				return (char *)buf+section->sh_offset;
			}
		}
		pData += header->e_shentsize;
	}
	
	return NULL;
}

int ProbeExec2Patched(u8 *buf, u32 *check)
{
	int res = ProbeExec2(buf, check);

	if ( IsStaticElf(buf) ) {
		if( check[8/4] > 0x120 ) {
			// Fake apitype to avoid reject	
			check[8/4] = 0x120;
		}

		if (check[0x4C/4] == 0) {
			char *strtab = GetStrTab(buf);
			if (strtab) {
				Elf32_Ehdr *header = (Elf32_Ehdr *)buf;
				int i;

				u8 *pData = buf+header->e_shoff;

				for (i = 0; i < header->e_shnum; i++) {
					Elf32_Shdr *section = (Elf32_Shdr *)pData;

					if (strcmp(strtab+section->sh_name, ".rodata.sceModuleInfo") == 0) {
						check[0x4C/4] = section->sh_offset;
						check[0x58/4] = 0;//set atter
					}
				
					pData += header->e_shentsize;
				}
			}
		}	
	}

	return res;
}

#if _PSP_FW_VERSION == 620
#define LoadcoreCheckExecFuncAddr				0x000042E0//OK
//#define LoadcoreCheckExecFuncPatchAddr			0x000086B4//OK
//#define LoadcoreCheckExecPatchAddr1				0x00001578//OK
//#define LoadcoreCheckExecPatchAddr2				0x000015C8//OK
//#define LoadcoreCheckExecPatchAddr3				0x00004A18//OK
#define LoadcoreLocationPatchOffset				0x00008B58//OK
#define LoadcoreLinkLibraryPatchAddr			0x000011E8//OK
#define LoadcoreLinkLibraryUserPatchAddr1		0x00002918//OK
#define LoadcoreLinkLibraryUserPatchAddr2		0x00002940//OK
#define LoadcoreLinkLibraryFuncAddr				0x00003404//OK
#define LoadcoreProbeExec1PatchAddr				0x000046A4//OK
#define LoadcoreProbeExec1FuncAddr				0x000061D4//OK
#define LoadcoreProbeExec2PatchAddr1			0x00004878//OK
//#define LoadcoreProbeExec2PatchAddr2			0x00004878//OK (One call on 6.20)
#define LoadcoreProbeExec2FuncAddr				0x000060F0//OK
#define LoadcoreSyscallStubPatchAddr			0x000040A4//OK
#define LoadcoreModuleSdkVersionCheckPatch1		0x00007E84//OK
#define LoadcoreModuleSdkVersionCheckPatch2		0x00007684//OK
#define LoadcoreTagCheckPatchAddr1				0x00006880//OK
#define LoadcoreTagCheckPatchAddr2				0x00006990//OK
#define LoadcoreTagCheckPatchAddr3				0x00006A28//OK
#define LoadcoreInitPatchAddr					0x00001DB0//OK
#define LoadcoreMemlmdSigcheckNid				0x2E208358//OK
#define LoadcoreMemlmdSigcheckFixAddr1			0x00006914//OK
#define LoadcoreMemlmdSigcheckFixAddr2			0x00006944//OK
#define LoadcoreMemlmdSigcheckFixAddr3			0x000069DC//OK
#define LoadcoreMemlmdDecryptNid				0xCA560AA6//OK
#define LoadcoreMemlmdDecryptFixAddr1			0x000041A4//OK
#define LoadcoreMemlmdDecryptFixAddr2			0x000068F0//OK

#elif _PSP_FW_VERSION == 639
#define LoadcoreCheckExecFuncAddr				0x000042E0
//#define LoadcoreCheckExecFuncPatchAddr			0x00007DC0
//#define LoadcoreCheckExecPatchAddr1				0x00001570
//#define LoadcoreCheckExecPatchAddr2				0x000015C0
//#define LoadcoreCheckExecPatchAddr3				0x00004BE8
#define LoadcoreLocationPatchOffset				0x00008274
#define LoadcoreLinkLibraryPatchAddr			0x000011E0
#define LoadcoreLinkLibraryUserPatchAddr1		0x00002910
#define LoadcoreLinkLibraryUserPatchAddr2		0x00002938
#define LoadcoreLinkLibraryFuncAddr				0x000033FC
#define LoadcoreProbeExec1PatchAddr				0x000047E4
#define LoadcoreProbeExec1FuncAddr				0x0000679C
#define LoadcoreProbeExec2PatchAddr1			0x000049E4
//#define LoadcoreProbeExec2PatchAddr2			0x00006A14
#define LoadcoreProbeExec2FuncAddr				0x000066F4
#define LoadcoreSyscallStubPatchAddr			0x000040A4
#define LoadcoreModuleSdkVersionCheckPatch1		0x000076E4
#define LoadcoreModuleSdkVersionCheckPatch2		0x00006EE4
#define LoadcoreTagCheckPatchAddr1				0x00005C34
#define LoadcoreTagCheckPatchAddr2				0x00005D44
#define LoadcoreTagCheckPatchAddr3				0x00005DDC
#define LoadcoreInitPatchAddr					0x00001DA8
#define LoadcoreMemlmdSigcheckNid				0x3F2AC9C6
#define LoadcoreMemlmdSigcheckFixAddr1			0x00005CC8
#define LoadcoreMemlmdSigcheckFixAddr2			0x00005CF8
#define LoadcoreMemlmdSigcheckFixAddr3			0x00005D90
#define LoadcoreMemlmdDecryptNid				0xE42AFE2E
#define LoadcoreMemlmdDecryptFixAddr1			0x000041A4
#define LoadcoreMemlmdDecryptFixAddr2			0x00005CA4

#elif _PSP_FW_VERSION == 660
#define LoadcoreCheckExecFuncAddr				0x00003FAC
//#define LoadcoreCheckExecFuncPatchAddr			0x00007B5C
//#define LoadcoreCheckExecPatchAddr1				0x000011F0
//#define LoadcoreCheckExecPatchAddr2				0x00001240
//#define LoadcoreCheckExecPatchAddr3				0x000048B4
#define LoadcoreLocationPatchOffset				0x00007F94
#define LoadcoreLinkLibraryPatchAddr			0x0000111C
#define LoadcoreLinkLibraryUserPatchAddr1		0x00002590
#define LoadcoreLinkLibraryUserPatchAddr2		0x000025B8
#define LoadcoreLinkLibraryFuncAddr				0x00003080
#define LoadcoreProbeExec1PatchAddr				0x000044B0
#define LoadcoreProbeExec1FuncAddr				0x00006468
#define LoadcoreProbeExec2PatchAddr1			0x000046B0
//#define LoadcoreProbeExec2PatchAddr2			0x000066D4
#define LoadcoreProbeExec2FuncAddr				0x000063C0
#define LoadcoreSyscallStubPatchAddr			0x00003D70
#define LoadcoreModuleSdkVersionCheckPatch1		0x000073A4
#define LoadcoreModuleSdkVersionCheckPatch2		0x00006BA4
#define LoadcoreTagCheckPatchAddr1				0x00005900
#define LoadcoreTagCheckPatchAddr2				0x00005A10
#define LoadcoreTagCheckPatchAddr3				0x00005AA8
#define LoadcoreInitPatchAddr					0x00001A28
#define LoadcoreMemlmdSigcheckNid				0x6192F715
#define LoadcoreMemlmdSigcheckFixAddr1			0x00005994
#define LoadcoreMemlmdSigcheckFixAddr2			0x000059C4
#define LoadcoreMemlmdSigcheckFixAddr3			0x00005A5C
#define LoadcoreMemlmdDecryptNid				0xEF73E85B
#define LoadcoreMemlmdDecryptFixAddr1			0x00003E70
#define LoadcoreMemlmdDecryptFixAddr2			0x00005970

#else
#error loadcore
#endif

static inline void hijack_CheckExec(u32 func_addr, void *function)
{
	sceKernelCheckExecFile[0] = ((u32 *)func_addr)[0];
	sceKernelCheckExecFile[1] = ((u32 *)func_addr)[1];
	sceKernelCheckExecFile[2] = GENERATE_JUMP( (void *)(func_addr + 8) );
	sceKernelCheckExecFile[3] = 0;

	((u32 *)func_addr)[0] = GENERATE_JUMP( function );
	((u32 *)func_addr)[1] = 0;
}

void PatchLoadCore()
{
//	u32 func_addr;
	SceModule2 *mod = sceKernelFindModuleByName("sceLoaderCore");
	u32 text_addr = mod->text_addr;

	create_DNR_list("LoadCoreForKernel", text_addr );

	/* Patch sceKernelCheckExecFile in export table */
	hijack_CheckExec( text_addr + LoadcoreCheckExecFuncAddr , sceKernelCheckExecFilePatched );
/*	
	_sw((u32)sceKernelCheckExecFilePatched ,text_addr + LoadcoreCheckExecFuncPatchAddr );
	MAKE_CALL(text_addr + LoadcoreCheckExecPatchAddr1 , sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr + LoadcoreCheckExecPatchAddr2 , sceKernelCheckExecFilePatched);
	MAKE_CALL(text_addr + LoadcoreCheckExecPatchAddr3 , sceKernelCheckExecFilePatched);
*/
	_sw( *(u32*)(text_addr+ LoadcoreLocationPatchOffset ) , text_addr + LoadcoreLocationPatchOffset + 0x1C );

	//sceKernelLinkLibraryEntries
	MAKE_CALL( text_addr+ LoadcoreLinkLibraryPatchAddr		, LinkLibraryEntriesPatch );//NID resolder
	MAKE_CALL( text_addr+ LoadcoreLinkLibraryUserPatchAddr1	, LinkLibraryEntriesUserPatch );//
	MAKE_CALL( text_addr+ LoadcoreLinkLibraryUserPatchAddr2	, LinkLibraryEntriesUserPatch );//
	LinkLibraryEntries=(void *)(text_addr+ LoadcoreLinkLibraryFuncAddr);//

	/* Patch 2 functions called by sceKernelProbeExecutableObject */
	//prx load patch
	MAKE_CALL(text_addr+ LoadcoreProbeExec1PatchAddr , ProbeExec1Patched ); 
	ProbeExec1=(void *)(text_addr+ LoadcoreProbeExec1FuncAddr );//

	//CheckElfSection?
	// static elf load
	MAKE_CALL(text_addr+ LoadcoreProbeExec2PatchAddr1 , ProbeExec2Patched ); 
//	MAKE_CALL(text_addr+ LoadcoreProbeExec2PatchAddr2 , ProbeExec2Patched ); 
	ProbeExec2=(void *)(text_addr+ LoadcoreProbeExec2FuncAddr );

	/* Allow kernel modules to have syscall imports */
	//lui        $t1, 0
	_sw( 0x3C090000, text_addr + LoadcoreSyscallStubPatchAddr );

	/* kill module_sdk_version check */
	//addu       $v1, $t5, $zr
//	_sw( 0x01A01821 , text_addr+ LoadcoreModuleSdkVersionCheckPatch );
	_sw( 0 , text_addr+ LoadcoreModuleSdkVersionCheckPatch1 );
//	_sw( 0x10000000 | *(u16 *)(text_addr+ LoadcoreModuleSdkVersionCheckPatch2 ) , text_addr+ LoadcoreModuleSdkVersionCheckPatch2 );
	_sh( 0x1000 , text_addr+ LoadcoreModuleSdkVersionCheckPatch2 + 2);


	/* kill TagCheck error */
	//KernelTag
	_sw(0, text_addr+ LoadcoreTagCheckPatchAddr1 );
	_sw(0, text_addr+ LoadcoreTagCheckPatchAddr1 + 4 );
	//UserTag
	_sw(0, text_addr+ LoadcoreTagCheckPatchAddr2 );
	_sw(0, text_addr+ LoadcoreTagCheckPatchAddr2 + 4 );
	_sw(0, text_addr+ LoadcoreTagCheckPatchAddr3 );
	_sw(0, text_addr+ LoadcoreTagCheckPatchAddr3 + 4 );


	MAKE_CALL(		text_addr + LoadcoreInitPatchAddr	, Patch_Init);
	_sw(0x02E02021,	text_addr + LoadcoreInitPatchAddr + 4 );
	//jalr       $s7, $ra		->jump
	//addiu      $a0, $zr, 4	->addu       $a0, $s7, $zr

#if 0
	// memlmd_3F2AC9C6//MemlmdSigcheck
	func_addr = sctrlHENFindFunction("sceMemlmd","memlmd", LoadcoreMemlmdSigcheckNid );
	MAKE_CALL(text_addr + LoadcoreMemlmdSigcheckFixAddr1 , func_addr );
	MAKE_CALL(text_addr + LoadcoreMemlmdSigcheckFixAddr2 , func_addr );
	MAKE_CALL(text_addr + LoadcoreMemlmdSigcheckFixAddr3 , func_addr );


	//memlmd_E42AFE2E//MemlmdDecrypt
	func_addr = sctrlHENFindFunction("sceMemlmd","memlmd", LoadcoreMemlmdDecryptNid );
	MAKE_CALL(text_addr + LoadcoreMemlmdDecryptFixAddr1 , func_addr );
	MAKE_CALL(text_addr + LoadcoreMemlmdDecryptFixAddr2 , func_addr );
#endif
}
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psputilsforkernel.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

int sceKernelApplyPspRelSectionPatched(void *buffer);

static void register_init_func(u32 text_addr);
static int (* PartitionCheck)(void *st0, void *check);
void ReleaseExtraMemory(u32 forced);

static inline int IsStaticElf(void *buf)
{
	Elf32_Ehdr *header = (Elf32_Ehdr *)buf;
	if (header->e_magic == 0x464C457F 
		&& header->e_type == 2) {
			return 1;
	}

	return 0;
}

SceUID sceKernelCreateThreadPatched(const char *name,SceKernelThreadEntry entry,int initPriority,int stackSize,SceUInt attr,SceKernelThreadOptParam *option)
{
	return sceKernelCreateThread(name,entry,initPriority,stackSize,attr,option);
}

static int (* PrologueModule)(void *mod_params, SceModule2 *mod) = NULL;
int PrologueModulePatched(void *mod_params, SceModule2 *mod)
{
	int ret = PrologueModule(mod_params, mod);
	if (ret >= 0)
	{
		ReleaseExtraMemory(0);
		sceKernelApplyPspRelSectionPatched(mod);
	}

	return ret;
}


static u32 buf[128/4] __attribute__((aligned(16)));
int PartitionCheckPatched(u32 *st0, u32 *check)
{
	SceUID fd = (SceUID)st0[0x18/4];

	SceOff pos;
	u16 attributes;

	if (fd < 0) {
		return PartitionCheck(st0, check);
	}
	
	pos = sceIoLseek(fd, 0, PSP_SEEK_CUR);

	// rewind to beginning 
	sceIoLseek(fd, 0, PSP_SEEK_SET);

	if (sceIoRead(fd, buf, 128) != 128 ) {
		sceIoLseek(fd, pos, PSP_SEEK_SET);
		return PartitionCheck(st0, check);
	}

	// go to module info offset 
	if ( buf[0/4] == 0x50425000 /* PBP */) {
		
		sceIoLseek(fd, buf[0x20/4], PSP_SEEK_SET);
		sceIoRead(fd, buf, 0x14);

		if ( buf[0/4] != 0x464C457F /* ELF */) {
			// Encrypted module 
			sceIoLseek(fd, pos, PSP_SEEK_SET);
			return PartitionCheck(st0, check);
		}

		sceIoLseek(fd, buf[0x20/4] + check[0x4C/4], PSP_SEEK_SET);
		
		if (IsStaticElf(buf) == 0 ) {
			//elf filesize
			check[0x10/4] = buf[0x24/4]-buf[0x20/4]; //Allow psar's in decrypted pbp's
		}
	}else if( buf[0/4] == 0x464C457F /* ELF */) {
		sceIoLseek(fd, check[0x4C/4], PSP_SEEK_SET);
	}else {
		// encrypted module 
		sceIoLseek(fd, pos, PSP_SEEK_SET);
		return PartitionCheck(st0, check);
	}

	sceIoRead(fd, &attributes, 2);

	if (IsStaticElf(buf)) {
		check[0x44/4] = 0;
	}else{
		if (attributes & 0x1000){
			check[0x44/4] = 1;
		}else{
			check[0x44/4] = 0;
		}
	}

	sceIoLseek(fd, pos, PSP_SEEK_SET);
	return PartitionCheck(st0, check);
}


#if _PSP_FW_VERSION == 620
#define ModuleMgrCheckExecFilePatchAddr		0x00008854//OK
#define ModuleMgrLoadModuleUserPatchAddr1	0x00000760//OK
#define ModuleMgrLoadModuleUserPatchAddr2	0x000007C0//OK
#define ModuleMgrLoadModuleVshPatchAddr1	0x000030B0//OK
#define ModuleMgrLoadModuleVshPatchAddr2	0x0000310C//OK
#define ModuleMgrLoadModuleVshPatchAddr3	0x00003138//OK
#define ModuleMgrLoadModuleKernelPatchAddr1	0x00003444//OK
#define ModuleMgrLoadModuleKernelPatchAddr2	0x0000349C//OK
#define ModuleMgrLoadModuleKernelPatchAddr3	0x000034C8//OK
#define ModuleMgrPartitionCheckPatchAddr1	0x000064FC//OK
#define ModuleMgrPartitionCheckPatchAddr2	0x00006878//OK
#define ModuleMgrPartitionCheckFuncAddr		0x00007FC0//OK
#define ModuleMgrPrologueModulePatchAddr	0x00007028//OK
#define ModuleMgrPrologueModuleFuncAddr		0x00008114//OK
#define ModuleMgrCreateThreadPatchAddr		0x0000894C//OK
#define ModuleMgrStartThreadPatchAddr		0x00008994//OK
#define ModuleMgrInitApitypeAddr			0x00009990//OK
#define ModuleMgrInitFileNameAddr			0x00009994//OK
#define ModuleMgrInitKeyConfigAddr			0x000099EC//OK
#define ModuleMgrInitBootDeviceAddr			0x00004DCC//OK

#elif _PSP_FW_VERSION == 639
#define ModuleMgrCheckExecFilePatchAddr		0x000087E0
#define ModuleMgrLoadModuleUserPatchAddr1	0x00000760
#define ModuleMgrLoadModuleUserPatchAddr2	0x000007C0
#define ModuleMgrLoadModuleVshPatchAddr1	0x000030B0
#define ModuleMgrLoadModuleVshPatchAddr2	0x0000310C
#define ModuleMgrLoadModuleVshPatchAddr3	0x00003138
#define ModuleMgrLoadModuleKernelPatchAddr1	0x00003444
#define ModuleMgrLoadModuleKernelPatchAddr2	0x0000349C
#define ModuleMgrLoadModuleKernelPatchAddr3	0x000034C8
#define ModuleMgrPartitionCheckPatchAddr1	0x00006528
#define ModuleMgrPartitionCheckPatchAddr2	0x000068A4
#define ModuleMgrPartitionCheckFuncAddr		0x00007FDC
#define ModuleMgrPrologueModulePatchAddr	0x00007054
#define ModuleMgrPrologueModuleFuncAddr		0x00008130
#define ModuleMgrCreateThreadPatchAddr		0x00008968
#define ModuleMgrStartThreadPatchAddr		0x000089B0
#define ModuleMgrInitApitypeAddr			0x000099A0
#define ModuleMgrInitFileNameAddr			0x000099A4
#define ModuleMgrInitKeyConfigAddr			0x000099FC
#define ModuleMgrInitBootDeviceAddr			0x00004DDC

#elif _PSP_FW_VERSION == 660
#define ModuleMgrCheckExecFilePatchAddr		0x00008884
#define ModuleMgrLoadModuleUserPatchAddr1	0x00000760
#define ModuleMgrLoadModuleUserPatchAddr2	0x000007C0
#define ModuleMgrLoadModuleVshPatchAddr1	0x000030B0
#define ModuleMgrLoadModuleVshPatchAddr2	0x0000310C
#define ModuleMgrLoadModuleVshPatchAddr3	0x00003138
#define ModuleMgrLoadModuleKernelPatchAddr1	0x00003444
#define ModuleMgrLoadModuleKernelPatchAddr2	0x0000349C
#define ModuleMgrLoadModuleKernelPatchAddr3	0x000034C8
#define ModuleMgrPartitionCheckPatchAddr1	0x0000651C//0x00006528
#define ModuleMgrPartitionCheckPatchAddr2	0x00006898//0x000068A4
#define ModuleMgrPartitionCheckFuncAddr		0x00007FD0//0x00007FDC
#define ModuleMgrPrologueModulePatchAddr	0x00007048//0x00007054
#define ModuleMgrPrologueModuleFuncAddr		0x00008124//0x00008130
#define ModuleMgrCreateThreadPatchAddr		0x0000895C//0x00008968
#define ModuleMgrStartThreadPatchAddr		0x000089A4//0x000089B0
#define ModuleMgrInitApitypeAddr			0x000099A0//0x000099A0
#define ModuleMgrInitFileNameAddr			0x000099A4//0x000099A4
#define ModuleMgrInitKeyConfigAddr			0x000099FC//0x000099FC
#define ModuleMgrInitBootDeviceAddr			0x00004DDC//0x00004DDC

#elif _PSP_FW_VERSION == 661
#define ModuleMgrCheckExecFilePatchAddr		0x00008884
#define ModuleMgrLoadModuleUserPatchAddr1	0x00000760
#define ModuleMgrLoadModuleUserPatchAddr2	0x000007C0
#define ModuleMgrLoadModuleVshPatchAddr1	0x000030B0
#define ModuleMgrLoadModuleVshPatchAddr2	0x0000310C
#define ModuleMgrLoadModuleVshPatchAddr3	0x00003138
#define ModuleMgrLoadModuleKernelPatchAddr1	0x00003444
#define ModuleMgrLoadModuleKernelPatchAddr2	0x0000349C
#define ModuleMgrLoadModuleKernelPatchAddr3	0x000034C8
#define ModuleMgrPartitionCheckPatchAddr1	0x0000651C//0x00006528
#define ModuleMgrPartitionCheckPatchAddr2	0x00006898//0x000068A4
#define ModuleMgrPartitionCheckFuncAddr		0x00007FD0//0x00007FDC
#define ModuleMgrPrologueModulePatchAddr	0x00007048//0x00007054
#define ModuleMgrPrologueModuleFuncAddr		0x00008124//0x00008130
#define ModuleMgrCreateThreadPatchAddr		0x0000895C//0x00008968
#define ModuleMgrStartThreadPatchAddr		0x000089A4//0x000089B0
#define ModuleMgrInitApitypeAddr			0x000099A0//0x000099A0
#define ModuleMgrInitFileNameAddr			0x000099A4//0x000099A4
#define ModuleMgrInitKeyConfigAddr			0x000099FC//0x000099FC
#define ModuleMgrInitBootDeviceAddr			0x00004DDC//0x00004DDC

#else
#error modulemgr
#endif


void PatchModuleMgr()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceModuleManager");
	u32 text_addr = mod->text_addr;
	
	// Patch ModuleMgr sceKernelCheckExec calls 
//	MAKE_JUMP(text_addr + ModuleMgrCheckExecFilePatchAddr , sceKernelCheckExecFilePatched);

	// NoDeviceCehckPatch (this time  has to be done differently) 
	// sceKernelLoadModule (User)
	_sw( 0			, text_addr + ModuleMgrLoadModuleUserPatchAddr1 );//jal	sceIoIoctl( 0x208001 ) -> nop
	_sw( 0x24020000 , text_addr + ModuleMgrLoadModuleUserPatchAddr2 );//jal	sceIoIoctl( 0x208081 ) ->addiu	$v0, $zr, $zr

	// sceKernelLoadModuleVSH
	_sw( 0			, text_addr + ModuleMgrLoadModuleVshPatchAddr1 );//sceIoIoctl( 0x208003 )
	_sw( 0			, text_addr + ModuleMgrLoadModuleVshPatchAddr2 );//sceIoIoctl( 0x208081 )
	_sw( 0x10000009 , text_addr + ModuleMgrLoadModuleVshPatchAddr3 );//sceIoIoctl( 0x208082 )

	// sceKernelLoadModule (Kernel)
	_sw( 0			, text_addr + ModuleMgrLoadModuleKernelPatchAddr1 );//sceIoIoctl( 0x208006 )
	_sw( 0			, text_addr + ModuleMgrLoadModuleKernelPatchAddr2 );//sceIoIoctl( 0x208081 )
	_sw( 0x10000010 , text_addr + ModuleMgrLoadModuleKernelPatchAddr3 );//sceIoIoctl( 0x208082 )

	MAKE_CALL(text_addr + ModuleMgrPartitionCheckPatchAddr1 , PartitionCheckPatched);
	MAKE_CALL(text_addr + ModuleMgrPartitionCheckPatchAddr2 , PartitionCheckPatched);
	PartitionCheck = (void *)(text_addr + ModuleMgrPartitionCheckFuncAddr );

	MAKE_CALL( text_addr + ModuleMgrPrologueModulePatchAddr , PrologueModulePatched );
	PrologueModule =(void *)( text_addr + ModuleMgrPrologueModuleFuncAddr );

	//patch export table
	MAKE_JUMP(text_addr+ ModuleMgrCreateThreadPatchAddr , sceKernelCreateThreadPatched );
//	MAKE_JUMP(text_addr+ ModuleMgrStartThreadPatchAddr  , sceKernelStartThreadPatched  );

	//store init addr
	register_init_func( text_addr );
}


/////////////////////// Init lib //////////////////////////
u32 init_addrs[4];

int sctrlKernelSetInitApitype(int apitype)
{
	int k1 = pspSdkSetK1(0);
	int prev = sceKernelInitApitype();

	_sw( apitype, init_addrs[0]);

	pspSdkSetK1(k1);
	return prev;
}

int sctrlKernelSetInitFileName(char *filename)
{
	int k1 = pspSdkSetK1(0);

	_sw( (u32)filename, init_addrs[1]);

	pspSdkSetK1(k1);
	return 0;
}

int sctrlKernelSetInitKeyConfig(int key)//old sctrlKernelSetInitMode
{
	int k1 = pspSdkSetK1(0);
	int r = sceKernelInitKeyConfig();

	_sw( key, init_addrs[2]);

	pspSdkSetK1(k1);
	return r;
}

int sctrlKernelBootFrom()
{
#if PSP_MODEL == 4
	int k1 = pspSdkSetK1(0);
	int (* BootFrom)() = (void *)(init_addrs[3]);
	int ret = BootFrom();
	pspSdkSetK1(k1);
	return ret;
#else
	return sceKernelBootFrom();
#endif
}

static void register_init_func(u32 text_addr)
{
	init_addrs[0] = text_addr + ModuleMgrInitApitypeAddr;	//sctrlKernelSetInitApitype
	init_addrs[1] = text_addr + ModuleMgrInitFileNameAddr;	//sctrlKernelSetInitFileName
	init_addrs[2] = text_addr + ModuleMgrInitKeyConfigAddr;	//sctrlKernelSetInitKeyConfig
	init_addrs[3] = text_addr + ModuleMgrInitBootDeviceAddr;	//sceKernelBootDevice
}

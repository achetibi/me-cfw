#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psputilsforkernel.h>
#include <pspsysmem_kernel.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "systemctrl.h"
#include "systemctrl_se.h"


PSP_MODULE_INFO("SystemControl", 0x3007, 2, 5);
PSP_MAIN_THREAD_ATTR(0);

#include "patch_addr.h"

SceUID thread_id;
SceModule2 *threadStart;

int model;

void ClearCaches()
{
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();	
}

void *search_module_export(SceModule2 *pMod, const char *szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	void *entTab;
	int entLen;
	int i = 0;

	entTab = pMod->ent_top;
	entLen = pMod->ent_size;
	while(i < entLen)
	{
		int count;
		int total;
		unsigned int *vars;
		entry = (struct SceLibraryEntryTable *) (entTab + i);

		if( (entry->libname == szLib) || (entry->libname && strcmp(entry->libname, szLib) == 0)) {
			total = entry->stubcount + entry->vstubcount;
			vars = entry->entrytable;

			if(total > 0) {
				for(count = 0; count < total ; count++) {
					if (vars[count] == nid)
						return (void *)(vars[count+total]);
				}
			}
		}
		i += (entry->len * 4);
	}
	return NULL;
}

void *search_module_stub(SceModule2 *pMod, const char *szLib, u32 nid)
{
	void *entTab = pMod ->stub_top;
	int entLen = pMod->stub_size;
	struct SceLibraryStubTable *current;
	int i = 0 ,j;

	while( i < entLen ) {
		current = (struct SceLibraryStubTable *)(entTab + i);
		if(strcmp(current->libname, szLib ) == 0) {
			for(j=0;j< current->stubcount ;j++) {
				if( current->nidtable[j] == nid ) {
					return (void *)((u32)(current->stubtable) + 8*j );
				}
			}

			break;
		}
		i += (current->len * 4);
	}

	return NULL;
}

u32 MesgLedDecrypt_buff[4];
int (* MesgLedDecrypt)() = (void *)MesgLedDecrypt_buff;

int MesgLedDecryptPatched(u32* a0/*tag*/,u8* a1/*key*/,int a2,u8 *buff,int t0/*size*/,int *t1/*ret*/,int t2/*flag*/,char *t3 ,
						  int sp0 , int sp1/* type*/ , int sp2 , int sp3)
{
	if( t2 == 0 )
	{
		if(buff && t1)
		{
			PSP_Header *head=(PSP_Header *)buff;
			int tag=head->oe_tag;

			if(tag==0x28796DAA || tag==0x7316308C ||
				tag==0x3EAD0AEE || tag==0x8555ABF2)
			{
				if( buff[336] == 0x1F && buff[337] == 0x8B )
				{
					*t1=head->comp_size;
					memmove(buff ,buff + 336 , head->comp_size );
					return 0;
				}
			}
		}
	}

	return MesgLedDecrypt( a0, a1, a2, buff, t0, t1,t2,t3, sp0 , sp1 , sp2 , sp3);
}

/*
int MesgLedDecryptPatchAddr_01g[] = { 0x00001D20, 0x00001ADC, 0x00003914, 0x00003AA0 };
int MesgLedDecryptPatchAddr_02g[] = { 0x00001DD0, 0x00001AE8, 0x00003E84, 0x00004010 };
int MesgLedDecryptPatchAddr_03g[] = { 0x00001E60, 0x00001AE8, 0x000043AC, 0x00004538 };
int MesgLedDecryptPatchAddr_05g[] = { 0x00001EF8, 0x00001AEC, 0x00004880, 0x00004A0C };		
*/
void PatchMesgLed( u32 text_addr )
{
/*
	const int *bridge = NULL;
	if (model == 0)
	{
		bridge = MesgLedDecryptPatchAddr_01g;
	}
	else if(model == 1)
	{
		bridge = MesgLedDecryptPatchAddr_02g;
	}
	else if( model == 2 || model == 3 || model == 6 || model == 8 )
	{
		bridge = MesgLedDecryptPatchAddr_03g;
	}
	else if ( model == 4 )
	{
		bridge = MesgLedDecryptPatchAddr_05g;
	}

	MAKE_CALL(text_addr+ bridge[0] , MesgLedDecryptPatched);
	MAKE_CALL(text_addr+ bridge[1] , MesgLedDecryptPatched);
	MAKE_CALL(text_addr+ bridge[2] , MesgLedDecryptPatched);
	MAKE_CALL(text_addr+ bridge[3] , MesgLedDecryptPatched);

	MesgLedDecrypt =(void *)(text_addr+ 0xE0);
*/
	u32 *MesgLedDecryptAddr =(void *)(text_addr + 0xE0);

	MesgLedDecrypt_buff[0] = MesgLedDecryptAddr[0];
	MesgLedDecrypt_buff[1] = MesgLedDecryptAddr[1];

	MAKE_JUMP( (u32)MesgLedDecryptAddr , MesgLedDecryptPatched );
	MesgLedDecryptAddr[1] = 0;

	MAKE_JUMP( (u32)MesgLedDecrypt_buff + 8 ,(u32)MesgLedDecryptAddr + 8 );
	MesgLedDecrypt_buff[3] = 0;

	ClearCaches();	
}

void exitHook()
{
	int k1;
	k1 = pspSdkSetK1(0);
	SceModule2 *mod = sceKernelFindModuleByName("scePower_Service");
	void ( *_scePowerRequestColdReset)() = (void *)search_module_export( mod, "scePower", 0x0442D852 );

	_scePowerRequestColdReset();
	pspSdkSetK1(k1);

}
void PatchLoadExec( SceModule2 *mod )
{
//	u32 text_addr;
	u32 LoadExecForUser_D1FB50DC = (u32)search_module_export( mod, "LoadExecForUser", 0xD1FB50DC );

	REDIRECT_FUNCTION( LoadExecForUser_D1FB50DC , exitHook );

//	MAKE_CALL( text_addr+ 0x00001AE0 , exitHook );
	ClearCaches();	
}

static int sceKernelApplyPspRelSectionPatched(void *buffer)
{
	SceModule2 *mod = buffer;	
	u32 buf=mod->text_addr;
	char *modinfo=mod->modname;

	if( strcmp(modinfo, "sceMesgLed") == 0 )
	{
		PatchMesgLed(buf);
	}
	else if (strcmp(modinfo,"sceLoadExec") == 0)
	{
		PatchLoadExec( mod );	
	}

	return 0;
}


SceUID sceKernelCreateThreadPatched(const char *name,SceKernelThreadEntry entry,int initPriority,int stackSize,SceUInt attr,SceKernelThreadOptParam *option)
{
	SceUID ret = sceKernelCreateThread(name,entry,initPriority,stackSize,attr,option);

	if( ret >= 0)
	{
		if (strcmp(name, "SceModmgrStart") == 0)
		{
			threadStart=sceKernelFindModuleByAddress((u32)entry);	
			thread_id =ret;
		}
	}
	return ret;
}
int sceKernelStartThreadPatched(SceUID thid,SceSize arglen,void *argp)
{
	if(thread_id ==thid)
	{
		thread_id =-1;

		if(threadStart)
		{
			sceKernelApplyPspRelSectionPatched( threadStart );
		}
	}

	return sceKernelStartThread(thid,arglen,argp);
}

void PatchLoadCore()
{
	SceModule2 *mod;
	mod = sceKernelFindModuleByName("sceLoaderCore");
//	u32 text_addr = mod->text_addr;

//	u32 func_addr;
//	_sw( *(u32*)(text_addr+ 0x8274) , text_addr + 0x8290);
/*
	//Allow kernel modules to have syscall exports 
	_sw(0x3C090000, text_addr + 0x40A4);//

	_sw(0, text_addr+ 0x76E4 );	//	

	_sw(0, text_addr+ 0x5C34 );	//	
	_sw(0, text_addr+ 0x5C38 );	//	

	_sw(0, text_addr+ 0x5D44 );	//	
	_sw(0, text_addr+ 0x5D48 );	//	

*/
/*
	// memlmd_3F2AC9C6 SigCheck
	func_addr = (text_addr + LoadcoreMemlmdFixAddr[0] );
	MAKE_CALL(text_addr + LoadcoreMemlmdFixAddr[1] , func_addr );
	MAKE_CALL(text_addr + LoadcoreMemlmdFixAddr[2] , func_addr );
	MAKE_CALL(text_addr + LoadcoreMemlmdFixAddr[3] , func_addr );

	//memlmd_E42AFE2E Decrypt
	func_addr = (text_addr + LoadcoreMemlmdFixAddr[4] );
	MAKE_CALL(text_addr + LoadcoreMemlmdFixAddr[5] , func_addr );
	MAKE_CALL(text_addr + LoadcoreMemlmdFixAddr[6] , func_addr );
*/
}

int sceIoIoctlPatch(SceUID fd, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
//int PatchDevctl(const char *dev,unsigned int cmd,void *indata,int inlen,void *outdata,int outlen)
{
	int ret = sceIoIoctl( fd,cmd,indata,inlen,outdata,outlen);
	if( ret < 0 )
	{
		if( cmd == 0x208001 || cmd == 0x208081 || cmd == 0x208003 || cmd == 0x208006 )
		{
			ret = 0;
		}
	}
	else
	{
		if( cmd == 0x208082 )
		{
			ret = -1;
		}
	}

	return ret;
}

void PatchModuleMgr()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceModuleManager");
//	u32 text_addr = mod->text_addr;
/*
	_sw(0		, text_addr + 0x00000760 ); // sceKernelLoadModule (User)
	_sw(0x24020000, text_addr + 0x000007C0 ); // sceKernelLoadModule (User)

	_sw(0, text_addr + 0x30B0); // sceKernelLoadModuleVSH
	_sw(0, text_addr + 0x310C); // sceKernelLoadModuleVSH
	_sw(0x10000009, text_addr + 0x3138); // sceKernelLoadModuleVSH

	// NoDeviceCehckPatch (this time  has to be done differently) 
	_sw(0, text_addr + 0x3444); // sceKernelLoadModule (Kernel)
	_sw(0, text_addr + 0x349C); // sceKernelLoadModule (Kernel)
	_sw(0x10000010, text_addr + 0x34C8); // sceKernelLoadModule (Kernel)
*/

	MAKE_JUMP( (u32)search_module_stub( mod, "IoFileMgrForKernel", 0x63632449 ), sceIoIoctlPatch );

	//patch export table
//	MAKE_JUMP(text_addr+ 0x00008968 , sceKernelCreateThreadPatched);//639
//	MAKE_JUMP(text_addr+ 0x000089B0 , sceKernelStartThreadPatched);//639
	MAKE_JUMP( (u32)search_module_stub( mod, "ThreadManForKernel", 0x446D8DE6 ) , sceKernelCreateThreadPatched);
	MAKE_JUMP( (u32)search_module_stub( mod, "ThreadManForKernel", 0xF475845D ) , sceKernelStartThreadPatched);

}

int (* MemlmdSigcheck)() = NULL;//dataA7A8
int (* MemlmdDecrypt)(u8 *buf, int size, int *ret, int u) = NULL;//dataA7AC


int loc_00000348(void *buf,int u,int flag)
{
	PSP_Header *head=(PSP_Header *)buf;
	int i;

	if(head->signature == 0x5053507E)
	{
#if _PSP_FW_VERSION == 660
		for(i=0;i<0x30;i++)
#else
		for(i=0;i<0x38;i++)
#endif
		{
			if(head->scheck[i] != '\0')
			{	
				if(head->reserved2[0] !=0 && head->reserved2[1]!=0 )
					return MemlmdSigcheck(buf, u ,flag);

				break;
			}
		}
	}

	return 0;
}

int MemlmdDecryptPatched(u8 *buf, int size ,int *ret, int flag )
{
	PSP_Header *head=(PSP_Header *)buf;

	if(buf && ret)
	{
		if(head->oe_tag ==0xC6BA41D3 || head->oe_tag == 0x55668D96 || head->oe_tag == 0xC01DB15D)
		{
			if( (buf[336] == 0x1F) && (buf[337] == 0x8B))
			{
				*ret=head->comp_size;
				memmove(buf ,buf+336 ,*ret);
				return 0;
			}
		}
	}

	return MemlmdDecrypt(buf, size, ret, flag);//dataA7AC
}

void PatchMemlmd()
{

	const int *bridge = NULL;

	if (model == 0)
	{
		bridge = MemlmdPatchAddr_01g;
	}
	else// if( model == 1 || model == 2 || model == 3 || model == 4 || model == 6 || model == 8 )
	{
		bridge = MemlmdPatchAddr_02g;
	}

	SceModule2 *mod = sceKernelFindModuleByName("sceMemlmd");
	u32 text_addr = mod->text_addr;


	//MemlmdSigcheck
	MemlmdSigcheck = (void *)(text_addr+ bridge[0] );
	//memlmd_3F2AC9C6(a0,a1,a2)
	MAKE_CALL(text_addr + bridge[1] , loc_00000348);
	//memlmd_97DA82BC(a0,a1,a2)
	MAKE_CALL(text_addr + bridge[2] , loc_00000348);

	//MemlmdDecrypt(buf, size, ret, flag);
	MemlmdDecrypt = (void *)(text_addr+ bridge[3] );
	//memlmd_E42AFE2E
	MAKE_CALL(text_addr+ bridge[4] , MemlmdDecryptPatched );
	//memlmd_D56F8AEC
	MAKE_CALL(text_addr+ bridge[5] , MemlmdDecryptPatched );

//	ClearCaches();
}
int module_bootstart(SceSize args, void *argp)
{	
	model = sceKernelGetModel();//*(int *)0x88FB0000;

//	asm("break");
	PatchLoadCore();
	PatchModuleMgr();
	PatchMemlmd();

	ClearCaches();
	return 0;
}

int module_stop(void)
{
	return 0;
}

int kuKernelGetModel(void)
{
	int k1, res;

	k1 = pspSdkSetK1(0);
	res = sceKernelGetModel();
	pspSdkSetK1(k1);

	return res;
}

int kuKernelInitKeyConfig()
{
	return sceKernelInitKeyConfig();
}

/*
int sctrlSEGetVersion()
{
	return 0x00020000;
}
*/
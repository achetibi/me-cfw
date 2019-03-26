
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <pspopenpsid.h>
#include <psputilsforkernel.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "horoscope_patch_list.h"

static SceUID (* KernelLoadModuleMs2_hook)() = NULL;
static int execute_apitype = 0;

SceUID sceKernelLoadModuleMs2_bridge(const char *path, int flags, SceKernelLMOption *option)
{
	SceUID (* sceKernelLoadModuleMs2_k)() = sceKernelLoadModuleMs2;
	return sceKernelLoadModuleMs2_k( execute_apitype , path , flags , option);
}

SceUID sceKernelLoadModuleMs2_patched(int apitype , const char *path, int flags, SceKernelLMOption *option)
{	
	execute_apitype = apitype;
	return KernelLoadModuleMs2_hook( path , flags , option);	
}
/*
void hook_module_stub(void *addr)
{
	SceModule2 *pMod = sceKernelFindModuleByAddress((u32 )addr);
	void *entTab = pMod ->stub_top;
	int entLen = pMod->stub_size;
	struct SceLibraryStubTable *current;
	int i = 0 ,j;

	while( i < entLen )
	{
		current = (struct SceLibraryStubTable *)(entTab + i);
		if(strcmp(current->libname,"ModuleMgrForKernel") == 0)
		{	
			for(j=0;j< current->stubcount ;j++)
			{
				if( current->nidtable[j] == 0x4535F1C2 )//sceKernelLoadModuleMs2
				{
					REDIRECT_FUNCTION( (u32)(&(current->stubtable[j*2]) ) , sceKernelLoadModuleMs2_bridge );
					return;
				}
			}

			break;
		}
		i += (current->len * 4);
	}

	return;
}
*/

int (*lzrc)(void *outbuf, u32 outcapacity, void *inbuf, void *unk) = NULL;
int get_addr(void *outbuf, u32 outcapacity, void *inbuf, void *unk)
{
	int k1 = pspSdkSetK1(0);
	if( !lzrc )
	{
		u32 *mod = (u32 *)sceKernelFindModuleByName("sceNp9660_driver");		
		if (!mod)
		{
			SceUID modload = sceKernelLoadModule("flash0:/kd/np9660.prx", 0, 0);
			mod = (u32 *)sceKernelFindModuleByUID(modload);
		}
		u32 *code = (u32 *)mod[27];
		
		int i;
		for (i = 0; i < 0x8000; i++) 
		{		
			if (code[i] == 0x27bdf4f0 && code[i+20] == 0x98C90001 ) 
			{	
				lzrc = (void *)&code[i];
			}
		}
	}

	pspSdkSetK1(k1);
	return lzrc( outbuf,outcapacity, inbuf, unk);
}

STMOD_HANDLER leda_previous = NULL;
int LedaModulePatch(SceModule2 *mod)
{
//	u32 text_addr = mod->text_addr;
	char *modinfo=mod->modname;
 
	if (strncmp(modinfo, "Resurssiklunssi", sizeof("Resurssiklunssi")-1 ) == 0) 
	{   	
		MAKE_DUMMY_FUNCTION0( 0x889007A8);
		_sw( (u32)get_addr , 0x8891C300 );
		ClearCaches();
	}
   
	if( leda_previous )  return leda_previous( mod );
	return 0;
}

//#define LoadModuleMs2HookAddr	0x00001C44

//hook sceKernelLoadModuleMs2
void sctrlHoroscopeHookLoadModuleMs2(void *func, u32 text_addr )
{
	KernelLoadModuleMs2_hook = func;
//	hook_module_stub( func );

	REDIRECT_FUNCTION( ((u32)func) + 0x00002E28 - 0x00000CE8 , sceKernelLoadModuleMs2_bridge );
	_sw( NOP , ((u32)func) + 0x00000C58 - 0x00000CE8 );
	
	//patch sceKernelLoadModuleMs2
	MAKE_JUMP( text_addr + horoscope_list.LoadModuleMs2HookAddr , sceKernelLoadModuleMs2_patched );

	leda_previous = sctrlHENSetStartModuleHandler( LedaModulePatch );
	ClearCaches();
}
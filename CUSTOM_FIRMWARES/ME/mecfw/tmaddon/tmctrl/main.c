#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <pspsysmem_kernel.h>
#include <psputilsforkernel.h>
#include <string.h>

#include "systemctrl_m33.h"
#include "tmctrl.h"

#include "../rebootex/rebootex_01g.h"
#include "../rebootex/rebootex_02g.h"

void printf (const char *fmt, ...);
void FlashEmu_D780E25C();

PSP_MODULE_INFO("TimeMachine_Control", 0x1007 , 1, 0);
STMOD_HANDLER previous = NULL;

extern SceUID flashfat_sema;
extern OpenInfo open_info[32];

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheInvalidateAll();	
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

int sub_00000854()
{
	sceKernelWaitSema( flashfat_sema , 1, NULL);

	int i;
	int ret = 0x80010018;
	for(i=0;i<32;i++)
	{
		if( open_info[ i ].index != 0
			&& open_info[ i ].resume == 0
			&& open_info[ i ].flags == 1 /* read only */)
		{
			int id = open_info[ i ].uid;
			open_info[ i ].offset = sceIoLseek( id , 0 , 1 );
			sceIoClose( id );
			open_info[ i ].resume = 1;
			ret = 0;
            printf("ms_backup done %s\n", open_info[i].filename );
			break;
		}
	}

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

wchar_t dc_path[] = L"\\TM\\"DEVKIT_VER_STR"\\";


int msfatOpenPatch( int type, void(* cb)(void *) , OpenParams *open_params)
{
	int ret;
	while( ( ret = sceKernelExtendKernelStack( type , cb , open_params ) ) == 0x80010018 )
	{
		if( open_params->file && memcmp( open_params->file + 4 , dc_path , sizeof(dc_path) - sizeof(wchar_t)  ) != 0 )
		{
			ret = sceKernelExtendKernelStack(  0x4000 , (void *)sub_00000854 , NULL );
			if( ret < 0)
			{
				break;
			}
		}
	}
	return ret;
}

int msfatDopenPatch( int 	type, void(* cb)(void *) , DopenParams *dopen_params)
{
	int ret;
	while( ( ret = sceKernelExtendKernelStack( type , cb , dopen_params ) ) == 0x80010018 )
	{
		if( dopen_params->dirname	&& memcmp( dopen_params->dirname + 4 , dc_path , sizeof(dc_path) - sizeof(wchar_t)  ) != 0 )
		{
			ret = sceKernelExtendKernelStack(  0x4000 , (void *)sub_00000854 , NULL );
			if( ret < 0)
			{
				break;
			}
		}
	}
	return ret;
}

int msfatDevctlPatch( int 	type, void(* cb )(void *) , void *buff)
{
	int ret;

	while( ( ret = sceKernelExtendKernelStack( type , cb , buff  ) ) == 0x80010018 )
	{
		ret = sceKernelExtendKernelStack(  0x4000 , (void *)sub_00000854 , NULL );
		if( ret <  0)
		{
			break;
		}
	}
	
	return ret;
}

SceUID sceIoOpenPatched(const char *file, int flags, SceMode mode)
{

//	if( sceKernelFindModuleByName("sceInit") == NULL )
	if( sceKernelFindModuleByName("vsh_module") != NULL )
	{
        printf("codepage load\n");
		return sceIoOpen( file, flags, mode);
	}

	return 0x80010018;
}

#if _PSP_FW_VERSION == 660
#define MSFAT_OPEN_PATCH_ADDR		0x000030FC
#define MSFAT_DOPEN_PATCH_ADDR		0x00003BA4
#define MSFAT_DEVCTL_PATCH_ADDR		0x000044CC
#define SIGCHECK_PATCH_ADDR         0x00005994
#define SIGCHECK_FUNC_ADDR          0x00007824

#elif _PSP_FW_VERSION == 661
#define MSFAT_OPEN_PATCH_ADDR		0x000030FC
#define MSFAT_DOPEN_PATCH_ADDR		0x00003BA4
#define MSFAT_DEVCTL_PATCH_ADDR		0x000044CC
#define SIGCHECK_PATCH_ADDR         0x00005994
#define SIGCHECK_FUNC_ADDR          0x00007824

#else
#error devkit_ver
#endif


int ApplyPatch(SceModule2 *mod)
{      
	u32 text_addr = mod->text_addr;
	char *modinfo=mod->modname;

    printf("%s \n", modinfo);

//	if (strcmp(modinfo, "sceMediaSync") == 0) 
	if (strcmp(modinfo, "sceUtility_Driver") == 0) 
//	if (strcmp(modinfo, "sceMSFAT_Driver") == 0) 
	{   
		SceModule2 *fat = sceKernelFindModuleByName("sceMSFAT_Driver");
		text_addr = fat->text_addr;
//		text_addr = mod->text_addr;
			
		MAKE_CALL( text_addr + MSFAT_OPEN_PATCH_ADDR	, msfatOpenPatch /* sub_00000CA0 */ );//open=0x48D4
		MAKE_CALL( text_addr + MSFAT_DOPEN_PATCH_ADDR	, msfatDopenPatch /* sub_00000BD8 */ );//dopen=0x5338
		MAKE_CALL( text_addr + MSFAT_DEVCTL_PATCH_ADDR	, msfatDevctlPatch );//devctl = 0x5B90

		ClearCaches();
	}
	else
	if( strcmp(modinfo, "sceLflashFatfmt") == 0 )
	{
		u32 sceLflashFatfmtStartFatfmt_k = sctrlHENFindFunction("sceLflashFatfmt","LflashFatfmt", 0xB7A424A4 );

		if( sceLflashFatfmtStartFatfmt_k )
		{
			MAKE_DUMMY_FUNCTION0( sceLflashFatfmtStartFatfmt_k );
			ClearCaches();
		}
	}
	else if( strcmp(modinfo, "sceCodepage_Service") == 0 )
	{
		void *func = search_module_stub(mod,"IoFileMgrForKernel", 0x109F50BC );
		if( func )
		{
			REDIRECT_FUNCTION( (u32)func	, sceIoOpenPatched );
			ClearCaches();
		}
	}

	if( previous )  
		return previous( mod );

	return 0;
}

#define REG32(a) *(volatile unsigned long *)(a)

int sceKernelGzipDecompressPatched(u8 *dest, int destSize, u8 *src, u32 unknown)
{
//	REG32(0xBE240008) = 0x80;

	if( (u32)dest == 0x88FC0000 )
	{
		int model = sceKernelGetModel();
		switch(model){
		case 0:
			src = rebootex_01g;
			break;
		case 1:
			src = rebootex_02g;
			break;
		default:
			break;
		}
	}
	
	return sceKernelGzipDecompress(dest, destSize, src, 0);
}

static void rebootex_patch()
{
	SceModule2 *mod = (SceModule2 *)sceKernelFindModuleByName("SystemControl");
	u32 func = (u32)search_module_stub(mod, "UtilsForKernel", 0x78934841 );
	MAKE_JUMP( func, sceKernelGzipDecompressPatched);
}

static void fix_sigcheck()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceLoaderCore");
	u32 text_addr = mod->text_addr;
	MAKE_CALL( text_addr + SIGCHECK_PATCH_ADDR, text_addr + SIGCHECK_FUNC_ADDR );
}

int module_start(SceSize args, void *argp)
{
	
	//patch rebootex
	rebootex_patch();
	
	FlashEmu_D780E25C();

	//sub_00000120
	previous = sctrlHENSetStartModuleHandler( ApplyPatch );

	fix_sigcheck();
	ClearCaches();

	return 0;
}
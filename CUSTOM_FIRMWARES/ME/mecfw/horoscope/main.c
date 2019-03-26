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

PSP_MODULE_INFO("sctrlHoroscope", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

u32 module_sdk_version = 0x03060010;

void *SystemCtrlForKernel_1F3037FB( int(* func)() );

SEConfig config;
static STMOD_HANDLER previous;

void ClearCaches()
{
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();	
}

int (* UtilityLoadModule)();
int (* UtilityUnLoadModule)();

int sceUtilityLoadModulePatch(int module) 	
{
	int ret = UtilityLoadModule(module);
	return (ret == 0x80020139) ? 0 : ret;
}

int sceUtilityUnloadModulePatch(int module) 	
{
	int ret= UtilityUnLoadModule(module);
	return (ret == 0x80111103) ? 0 : ret;
}

int (*sceKernelLoadModuleUser)(const char *path, int flags, SceKernelLMOption *option) = NULL;

int load_module_get_function(void)
{
	sceKernelLoadModuleUser = (void *)sctrlHENFindFunction("sceModuleManager", "ModuleMgrForUser", 0x977DE386);
	
	if (sceKernelLoadModuleUser == NULL)
		return -5;

	return 0;
}

int sceKernelLoadModuleUserPatched(const char *path, int flags, SceKernelLMOption *option)
{
	int ret = (*sceKernelLoadModuleUser)(path, flags, option);

	if (ret == 0x80020148 || ret == 0x80020130)
	{
		if (strstr(path,"ms0:/") != NULL)
		{
			ret = 0x80020146;
		}
	}

	return ret;
}

static void PatchLoadModule(SceModule *mod)
{
	/*u32 *text_addr = (u32 *)sctrlHENFindFunction("sceModuleManager","ModuleMgrForUser",0x977DE386);

	sceKernelLoadModuleUser_buff[0] = text_addr[0];
	sceKernelLoadModuleUser_buff[2] = text_addr[1];

	REDIRECT_FUNCTION( (u32)text_addr, sceKernelLoadModuleUserPatched );
	MAKE_JUMP( (u32)(sceKernelLoadModuleUser_buff + 1 ), (u32)(text_addr + 2 ) );*/

	sctrlHookImportByNid(mod, "ModuleMgrForUser", 0x977DE386, &sceKernelLoadModuleUserPatched, 1);
}

static int get_param( const char *filename )
{
	SceUID fd = sceIoOpen( filename , PSP_O_RDONLY, 0777);
	if (fd < 0)
	{
		return fd;
	}

	u32 buff[0x10/4];
	sceIoRead(fd, buff, 0x10);
	sceIoClose(fd);

	u8 ret = 0;
	PSP_Header *header = (PSP_Header *)buff;

	if( header->signature == 0x5053507E)
	{
		ret = header->module_ver_lo;
	}

	return (int)ret;
}

//char opnssmp_path[256];
static const char *opnssmp_path = "disc0:/PSP_GAME/SYSDIR/OPNSSMP.BIN";

u32* (*sceKernelGetGameInfo_k)() = NULL;
u64 Gameinfo_patch(const char *path)
{
	SceIoStat fstat;
	u32 *info = sceKernelGetGameInfo_k();
	const char *ret = opnssmp_path;

//	create_path( opnssmp_path , path + 8 );
	u32 real_param = info[216/4];

	memset(&fstat, 0, sizeof(SceIoStat));
	if(sceIoGetstat( opnssmp_path ,&fstat) >= 0 )
	{
		if( real_param == 0 )
		{
			int param;
			if( ( param = get_param( opnssmp_path )) > 0 )	
			{
				real_param = (u8)param;
			}
		}
	}	
	else	
	{
		ret = path;	
	}

	u64 ret_value = (u64)((u32)ret);
	return ( ret_value << 32)| ((u64)real_param);
}
/*
int sceKernelLoadModulePatched(const char *path, int flags, int *type ,int stat_ret)
{
	int ret;
	
	char *malloc_buffer = (char *)sctrlKernelMalloc( 4096 + 64 );

	if( malloc_buffer )
	{
		char *backup = malloc_buffer;

		if ((((u32)malloc_buffer) % 64) != 0)
		{
			malloc_buffer += (64 - (((u32)malloc_buffer) % 64));	
		}		

		memset( malloc_buffer , 0 , 4096 );

		//flash2:/opnssmp.bin
		int size = Read_opnssmp( path + 8 , malloc_buffer , 4096 );
		if(size < 0)
		{
			sctrlKernelFree(backup);
			goto NORMAL;
		}

		PspOpenPSID openpsid;
//		int (*sceOpenPSIDGetOpenPSID_k)(PspOpenPSID *openpsid) = (void *)FindProc("sceOpenPSID_Service","sceOpenPSID_driver", 0xC69BEBCE );
//		sceOpenPSIDGetOpenPSID_k(&openpsid);
		sceOpenPSIDGetOpenPSID(&openpsid);
		memcpy( malloc_buffer + size , &openpsid , 0x10 );
		size += 0x10;

		u32 buff[2];
//		int (*sceIdStorageLookup_k)(u16 key, u32 offset, void *buf, u32 len) = (void *)FindProc("sceIdStorage_Service","sceIdStorage_driver", 0x6FE062D1 );
//		sceIdStorageLookup_k( 0x44 , 0 , buff ,  6 );
		sceIdStorageLookup( 0x44 , 0 , buff ,  6);
		memcpy( malloc_buffer + size , buff , 6 );
		size += 8;

		strcpy( malloc_buffer + size , "UCJS10041");//ULJM05781
		size += 0x10;

		u32 devkit_version = sceKernelDevkitVersion();
		char *ver_str = (char *)(malloc_buffer + size);
		ver_str[0] = (devkit_version >> 24) + '0';
		ver_str[1] = '.';
		ver_str[2] = ((devkit_version & 0x00FF0000) >> 16)  + '0';
		ver_str[3] =((devkit_version & 0x0000FF00) >> 8) + '0';
//		strcpy( malloc_buffer + size , CFW_VERSION );
		size += 8;

		ret = sceKernelLoadModuleBuffer( (void *)size , (int)malloc_buffer , 0 , NULL);
		
		sctrlKernelFree(backup);
		return ret;

	}


NORMAL:
	;
	return sceKernelLoadModule( path , 0 , NULL );
}
*/

static void PatchInit()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceInit");
	u32 text_addr = mod->text_addr;

	//addu       $a3, $v0, $zr	
//	_sw( 0x00403821 , text_addr + horoscope_list.init_list.IoGetStatPatchAddr  ); 

	//addu       $a0, $s1, $zr  
	_sw( 0x02202021 , text_addr + horoscope_list.init_list.ModuleStrInPatchAddr );

	//addu       $s1, $v1, $zr
	_sw( 0x00608821 , text_addr + horoscope_list.init_list.ModuleStrOutPatchAddr );

//	MAKE_CALL( text_addr  + horoscope_list.init_list.LoadModulePatchAddr  , sceKernelLoadModulePatched );
	MAKE_CALL( text_addr  + horoscope_list.init_list.GetGameInfoPatchAddr , Gameinfo_patch );

	sceKernelGetGameInfo_k = (void *)( text_addr + horoscope_list.init_list.GetGameInfoFuncAddr );
}

typedef struct{
	u32 tag;
	u8 key[0x10];
	u32 code;//scramble ?
	u32 type;
} key_table;

key_table decryptkey_table[] = {
	{ 0xD91609F0, { 0xD0, 0x36, 0x12, 0x75, 0x80, 0x56, 0x20, 0x43, 0xC4, 0x30, 0x94, 0x3E, 0x1C, 0x75, 0xD1, 0xBF }, 0x5D, 2},
//	{ 0xD9160AF0, { 0x10, 0xA9, 0xAC, 0x16, 0xAE, 0x19, 0xC0, 0x7E, 0x3B, 0x60, 0x77, 0x86, 0x01, 0x6F, 0xF2, 0x63 }, 0x5D, 2},
//	{ 0xD9160BF0, { 0x83, 0x83, 0xF1, 0x37, 0x53, 0xD0, 0xBE, 0xFC, 0x8D, 0xA7, 0x32, 0x52, 0x46, 0x0A, 0xC2, 0xC2 }, 0x5D, 2},
//	{ 0xD91611F0, { 0x61, 0xB0, 0xC0, 0x58, 0x71, 0x57, 0xD9, 0xFA, 0x74, 0x67, 0x0E, 0x5C, 0x7E, 0x6E, 0x95, 0xB9 }, 0x5D, 2},
	{ 0xD91612F0, { 0x9E, 0x20, 0xE1, 0xCD, 0xD7, 0x88, 0xDE, 0xC0, 0x31, 0x9B, 0x10, 0xAF, 0xC5, 0xB8, 0x73, 0x23 }, 0x5D, 2},
	{ 0xD91613F0, { 0xEB, 0xFF, 0x40, 0xD8, 0xB4, 0x1A, 0xE1, 0x66, 0x91, 0x3B, 0x8F, 0x64, 0xB6, 0xFC, 0xB7, 0x12 }, 0x5D, 2},
	{ 0xD91614F0, { 0xFD, 0xF7, 0xB7, 0x3C, 0x9F, 0xD1, 0x33, 0x95, 0x11, 0xB8, 0xB5, 0xBB, 0x54, 0x23, 0x73, 0x85 }, 0x5D, 2},
	{ 0xD91615F0, { 0xC8, 0x03, 0xE3, 0x44, 0x50, 0xF1, 0xE7, 0x2A, 0x6A, 0x0D, 0xC3, 0x61, 0xB6, 0x8E, 0x5F, 0x51 }, 0x5D, 2},
	{ 0xD91616F0, { 0x53, 0x03, 0xB8, 0x6A, 0x10, 0x19, 0x98, 0x49, 0x1C, 0xAF, 0x30, 0xE4, 0x25, 0x1B, 0x6B, 0x28 }, 0x5D, 2},
	{ 0xD91617F0, { 0x02, 0xFA, 0x48, 0x73, 0x75, 0xAF, 0xAE, 0x0A, 0x67, 0x89, 0x2B, 0x95, 0x4B, 0x09, 0x87, 0xA3 }, 0x5D, 2},
	{ 0xD91618F0, { 0x96, 0x96, 0x7C, 0xC3, 0xF7, 0x12, 0xDA, 0x62, 0x1B, 0xF6, 0x9A, 0x9A, 0x44, 0x44, 0xBC, 0x48 }, 0x5D, 2},
	{ 0xD91619F0, { 0xE0, 0x32, 0xA7, 0x08, 0x6B, 0x2B, 0x29, 0x2C, 0xD1, 0x4D, 0x5B, 0xEE, 0xA8, 0xC8, 0xB4, 0xE9 }, 0x5D, 2},
	{ 0xD9161AF0, { 0x27, 0xE5, 0xA7, 0x49, 0x52, 0xE1, 0x94, 0x67, 0x35, 0x66, 0x91, 0x0C, 0xE8, 0x9A, 0x25, 0x24 }, 0x5D, 2},
	{ 0xD91620F0, { 0x52, 0x1C, 0xB4, 0x5F, 0x40, 0x3B, 0x9A, 0xDD, 0xAC, 0xFC, 0xEA, 0x92, 0xFD, 0xDD, 0xF5, 0x90 }, 0x5D, 2},
	{ 0xD91621F0, { 0xD1, 0x91, 0x2E, 0xA6, 0x21, 0x14, 0x29, 0x62, 0xF6, 0xED, 0xAE, 0xCB, 0xDD, 0xA3, 0xBA, 0xFE }, 0x5D, 2},
	{ 0xD91622F0, { 0x59, 0x5D, 0x78, 0x4D, 0x21, 0xB2, 0x01, 0x17, 0x6C, 0x9A, 0xB5, 0x1B, 0xDA, 0xB7, 0xF9, 0xE6 }, 0x5D, 2},
	{ 0xD91623F0, { 0xAA, 0x45, 0xEB, 0x4F, 0x62, 0xFB, 0xD1, 0x0D, 0x71, 0xD5, 0x62, 0xD2, 0xF5, 0xBF, 0xA5, 0x2F }, 0x5D, 2},
	{ 0xD91624F0, { 0x61, 0xB7, 0x26, 0xAF, 0x8B, 0xF1, 0x41, 0x58, 0x83, 0x6A, 0xC4, 0x92, 0x12, 0xCB, 0xB1, 0xE9 }, 0x5D, 2},
	{ 0xD91628F0, { 0x49, 0xA4, 0xFC, 0x66, 0xDC, 0xE7, 0x62, 0x21, 0xDB, 0x18, 0xA7, 0x50, 0xD6, 0xA8, 0xC1, 0xB6 }, 0x5D, 2},
	{ 0xD91681F0, { 0x52, 0xB6, 0x36, 0x6C, 0x8C, 0x46, 0x7F, 0x7A, 0xCC, 0x11, 0x62, 0x99, 0xC1, 0x99, 0xBE, 0x98 }, 0x5D, 6},
	{ 0x0B2B80F0, { 0x57, 0xB4, 0xA6, 0x5C, 0x75, 0x2D, 0xB9, 0x4D, 0xE1, 0x67, 0xE3, 0x31, 0xBF, 0x4D, 0x70, 0xF8 }, 0x5C, 6},
//	{ 0x457B80F0, { 0xD4, 0x35, 0x18, 0x02, 0x29, 0x68, 0xFB, 0xA0, 0x6A, 0xA9, 0xA5, 0xED, 0x78, 0xFD, 0x2E, 0x9D }, 0x5B, 6},
//	{ 0x457B8AF0, { 0x47, 0xEC, 0x60, 0x15, 0x12, 0x2C, 0xE3, 0xE0, 0x4A, 0x22, 0x6F, 0x31, 0x9F, 0xFA, 0x97, 0x3E }, 0x5B, 6}
};

int (* decrypt_next)() = NULL;
int (* MesgLedDecrypt)(u32* a0/*tag*/,u8* a1/*key*/,int a2,u8 *buff,int t0/*size*/,int *t1/*ret*/,int t2/*flag*/,char *t3 ,
						   int sp0 , int sp1/* type*/ , u8 *sp2 , u8 *sp3) = NULL;

int opnssmp_decrypt_helper( u8 *buff,int t0/*size*/,int *t1/*ret*/,int t2/*flag*/)
{
	PSP_Header *head=(PSP_Header *)buff;
	u32 tag = head->tag;
	int i;

	for(i=0;i< sizeof(decryptkey_table) / sizeof(key_table) ;i++)
	{
		if( decryptkey_table[i].tag == tag )
		{
			int ret;
			if( (ret = MesgLedDecrypt( &(decryptkey_table[i].tag) , decryptkey_table[i].key , decryptkey_table[i].code , buff , t0 , t1 , 0 ,NULL 
				,0 , decryptkey_table[i].type , NULL , NULL )) >= 0 )
			{
//				printf("Fooooooooooooooooooooooo\n");
				return ret;
			}

			break;
		}
	}

	if( !decrypt_next )
		return -1;

//	return MesgLedDecrypt( a0 , a1 , a2 , buff , t0 , t1 , t2 , t3 , sp0 , sp1 , sp2 , sp3 );
	return decrypt_next( buff , t0 , t1 , t2 );
}

static int IsHomebrewsRunlevel(void)
{
	int apitype = sceKernelInitApitype();

	if(apitype == 0x152 || apitype == 0x141)
	{
		return 1;
	}

	return 0;
}

static int HoroscopeModuleChain(SceModule2 *mod)
{
	if (previous)
	{
		(*previous)(mod);
	}

	PatchLoadModule((SceModule *)mod);
	patch_for_old_fw((SceModule *)mod);

	if(!config.usenodrm)
	{
		if(strcmp(mod->modname, "scePspNpDrm_Driver") == 0)
		{
			int ret;

			fix_weak_imports();
			ret = nodrm_get_npdrm_functions();
		}

		patch_drm_imports((SceModule *)mod);
	}

	if(!config.hidecfwdirs && !IsHomebrewsRunlevel())
	{
		HideCFWDirs((SceModule *)mod);
	}

	return 0;
}

static void patch_sceLoadExec(void)
{
	u32 k1 = pspSdkSetK1(0);
	SceModule2 *mod = (SceModule2*) sceKernelFindModuleByName("sceLoadExec");

	if(mod != NULL)
	{
		_sw(0, mod->text_addr + 0x000009B4);
		ClearCaches();
	}

	pspSdkSetK1(k1);
}

int module_start()
{	
	sctrlSEGetConfig(&config);
//	int ret = 1;

	char *umd_file = sctrlSEGetUmdFile();
	if(umd_file)
	{
		if( strncmp( umd_file, "ms0:/", 5)  == 0 || strncmp( umd_file, "ef0:/", 5)  == 0)
		{		

			if(!config.executeopnssmp)
			{
				PatchInit();
			}

			SceModule2 *mod = sceKernelFindModuleByName("scePspNpDrm_Driver");		
			if(mod)
			{
				mod = sceKernelFindModuleByName("sceUtility_Driver");		
				u32 text_addr = mod->text_addr;

				MAKE_CALL( text_addr + horoscope_list.utility_list.LoadModulePatchAddr	 , sceUtilityLoadModulePatch );
				MAKE_CALL( text_addr + horoscope_list.utility_list.UnloadModulePatchAddr , sceUtilityUnloadModulePatch );

				UtilityLoadModule =(void *)(text_addr  + horoscope_list.utility_list.LoadModuleAddr );
				UtilityUnLoadModule =(void *)(text_addr+ horoscope_list.utility_list.UnloadModuleAddr );
			}

//			ret = 0;
		}
	}

	if( sceKernelBootFrom() == PSP_BOOT_DISC )
	{
		SceModule2 *mod = sceKernelFindModuleByName("sceMesgLed");
		u32 text_addr = mod->text_addr;

		decrypt_next = (void *)SystemCtrlForKernel_1F3037FB( opnssmp_decrypt_helper );	
		MesgLedDecrypt =(void *)(text_addr + 0xE0);

//		PatchLoadModule();
	}

	load_module_get_function();

	if(!config.usenodrm)
	{
		NoDRM_Init();
		nodrm_get_npdrm_functions(); // np9660 mode: npdrm already loaded
	}

	patch_sceLoadExec();
	get_functions_for_old_fw();
	previous = sctrlHENSetStartModuleHandler(&HoroscopeModuleChain);
	
	ClearCaches();
	return 0;
}

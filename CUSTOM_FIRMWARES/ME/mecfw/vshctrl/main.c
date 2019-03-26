#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspreg.h>
#include <pspthreadman_kernel.h>
#include <psploadcore.h>
#include <pspctrl.h>
#include <psppower.h>
#include <psprtc.h>
#include <pspumd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "systemctrl_me.h"
#include "main.h"
#include "vshctrl_patch_list.h"

#include "virtualpbpmgr.h"
#include "io_patch.h"


PSP_MODULE_INFO("VshControl", 0x1007, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define EBOOT_BIN "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN"
#define PROME_BIN "disc0:/PSP_GAME/SYSDIR/EBOOT.OLD"
//#define BOOT_BIN  "disc0:/PSP_GAME/SYSDIR/BOOT.BIN"

static char vshmount[128];
static int psp_model;


SceUID satelitedfd = -1;
int (*vshmenu_kernel)() = NULL;

u32 iso_mount = 0;
u32 clock_flag = 0;
u32 real_time = 0;
u32	firsttime = 0;

STMOD_HANDLER previous = NULL;
SEConfig config;

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

int (* get_sfo_param)() = NULL;
int sfo_patch(int a0, const char *a1 ,char *a2, int a3 )
{
	int ret = get_sfo_param( a0, a1, a2, 8 );
	if( ret == 0 )
	{
		int k1 = pspSdkSetK1(0);
		const char *ver_str = init_version_str();
		if( strncmp( ver_str , a2 , 4 ) < 0 )	{
			memcpy( a2, ver_str, 4 );	
		}
		pspSdkSetK1(k1);
	}
	return ret;
}


int umd_id = -1;
u32	umd_timer=0;
int umd_mode = 0;

void pspUmdCallback(int dstat)
{
	if (umd_id > 0)
	{
		sceKernelNotifyCallback(umd_id, dstat);
	}
	/*
	else
	{
		printf("id error 0x%08X \n", umd_id );
	}
	*/
}

int exit_cb(int arg1, int arg2, void *arg)
{
	pspUmdCallback( arg2);

	if( iso_mount && umd_mode ) {
		if( arg2 == 1) {
			umd_timer = sceKernelGetSystemTimeLow();
		}
	}
	return 0;
}

void unmount_iso()
{
	iso_mount=0;
	sctrlSEUmountUmd();
	sceKernelCallSubIntrHandler(4,26,0,0);
	pspUmdCallback( 0x01);
}


int vctrlVSHExitVSHMenu(SEConfig *g_conf, char *videoiso, int disctype)
{
	int k1 = pspSdkSetK1(0); 
	vshmenu_kernel = NULL;

	if( g_conf )
	{
		int old_cpu = config.vshcpuspeed;
		memcpy(&config,g_conf,sizeof(config));
		sctrlSEApplyConfig(&config);
		
		if(clock_flag && k1 != 0 )
		{			
			int new_cpu = config.vshcpuspeed;	
			int new_bus = config.vshbusspeed;
	
			if(old_cpu != new_cpu )
			{			
				if(new_cpu == 0) {	
					new_cpu = 222;
					new_bus = 111;
				}

//				printf("User Level3:%d . %d \n", level , sceKernelGetUserLevel() );
//				printf("Speed:%d . %d \n", new_cpu , new_bus );

				int level = sctrlKernelSetUserLevel(4);
				SetSpeed(new_cpu , new_bus);
				sctrlKernelSetUserLevel( level );

				real_time = sceKernelGetSystemTimeLow();
			}

		}
	}

	if(videoiso)
	{
		sctrlSESetDiscType(disctype);
		
		int st = sceUmdCheckMedium();
//		printf( "umd 0x%08X \n", st );
		if( st != 1 )
			umd_mode = 1;

		if(iso_mount == 0 )
		{
			sctrlSESetDiscOut(1);	
			iso_mount=1;
			exit_cb( 0, 0x1, NULL);
		}
		else
		{
			char *umd = sctrlSEGetUmdFile();
			if(strcmp(umd,videoiso) ==0)
			{
					pspSdkSetK1(k1);
					return 0;
			}
			else
			{
				sctrlSEUmountUmd();	
//				pspUmdCallback( 0x01);
				exit_cb( 0, 0x1, NULL);
			}
		}
//		printf( "umd_mode 0x%08X \n",umd_mode );

		sctrlSEMountUmdFromFile( videoiso , 0 , umd_mode );
	}
	else
	{
		if(iso_mount)
			unmount_iso();	
	}

	pspSdkSetK1(k1);
	return 0;
}

void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

static int Load150Module()
{
	SceUID mod = sceKernelLoadModule("flash0:/kd/reboot150.prx", 0, 0);

	if(mod<0)
	{
		mod = sceKernelLoadModule("ms0:/seplugins/reboot150.prx", 0, 0);
	}

	if(mod < 0)
	{
		return mod;
	}
	
	return sceKernelStartModule(mod, 0, 0, 0, 0);
}

void KXploitString(char *str)
{
	if (str) 
	{
		char *perc = strchr(str, '%');
		if (perc) 
		{
			strcpy(perc, perc+1);
		}
	}
}

static void FixPathCommon(const char *file, char *label, char *add )
{
	char str[256];
	const char *file_str = file + 2;

	if (strstr( file, "0:/PSP/GAME/") == file_str )
	{
		strcpy(str, (char *)file);

		char *p = strstr(str, label );
		if (p)
		{
			strcpy((char *)file+13, add );
			strncpy((char *)file+17, str+14, p-(str+14));
			strcpy((char *)file+17+(p-(str+14)), p+5);
		}
	}
}

void Fix150Path(const char *file)
{
	return FixPathCommon( file, "__150", "150/" );
}

int GetIsoIndex(const char *file)
{
	char number[8];

	char *pos = strstr(file, "/MMMMMISO");
	if(!pos)
		return -1;

	char *p = strchr( pos + 9, '/');
	if (!p)
		return strtol( pos + 9, NULL, 10);

	memset(number , 0 , sizeof(number));

	strncpy(number,  pos + 9 , p-( pos + 9));
	return strtol(number, NULL, 10);
}

static int CheckPrometheus()
{
	int ret = 0;
	PspIoDrvFileArg arg;
	memset(&arg, 0, sizeof(arg));

	if (isofs_init(NULL) >= 0)
	{
		getlba_andsize( &arg, (PROME_BIN + 6) , &ret, &ret);
	}
	isofs_exit(NULL);
	return ret;
}

int LoadExecVSHCommonPatched(int apitype, char *file, struct SceKernelLoadExecVSHParam *param, int unk2)
{
	int k1 = pspSdkSetK1(0);
	int index;
//	int reboot150 = 0;

//	sctrlSESetUmdFile("");
	sctrlSESetUmdFileEx("", NULL);

	if (config.high_memory)
	{
		sctrlHENSetMemory(MAX_HIGH_MEMSIZE, 0);
	}

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		int pboot_flag = 0;
		const char *iso_path = virtualpbp_getfilename(index);
		sctrlSESetUmdFile( iso_path );

		int umd_mode = config.umdmode;
		if(umd_mode == 2)//MODE_MARCH33
		{
			sctrlSESetBootConfFileIndex(1);
		}
		else if(umd_mode == 3)//MODE_NP9660
		{
			sctrlSESetBootConfFileIndex(2);
		}
		else if(umd_mode == 4)//MODE_ME
		{
			sctrlSESetBootConfFileIndex(3);
		}
		else if(umd_mode == 5)//MODE_INFERNO
		{
			sctrlSESetBootConfFileIndex(5);
		}

		u32 opn_type = virtualpbp_get_isotype(index);
		u32 *info = (u32 *)sceKernelGetGameInfo();
		if(opn_type)
		{
			info[216/4] = opn_type;
		}

//		printf("info = 0x%08X \n", (u32)info );
//		asm("break");

		if(strstr( param->argp , "PBOOT.PBP") != NULL)
		{
			pboot_flag = 1;
		}

		if( pboot_flag == 0  )// || umd_mode < 2
		{
			if ( CheckPrometheus() )
			{
//				strcpy(file, PROME_BIN);
				param->args = sizeof(PROME_BIN);
				param->argp = PROME_BIN;
			}
			else
			{
//				strcpy(file, EBOOT_BIN);
				param->args = sizeof(EBOOT_BIN);
				param->argp = EBOOT_BIN;
			}
		}

		if(umd_mode >= 2)
		{
			if(psp_model == 4)
			{
				if((apitype & 0xF0) != 0x20)
				{
					apitype = ((apitype & 0xF0) == 0x50) ? 0x125 : 0x123;
				}

				strcpy(file, iso_path);
			}
			else
			{
				if(pboot_flag == 0)
				{
					apitype = PSP_INIT_APITYPE_DISC;
					file = param->argp;
				}
			}

			param->key = "umdemu";
		}
		else
		{
			if( pboot_flag == 0 )
			{
				apitype = PSP_INIT_APITYPE_DISC;
			}
			else
			{
				apitype = 0x160;
			}

			file = param->argp;
			param->key = "game";
		}

		pspSdkSetK1(k1);
		return sctrlKernelLoadExecVSHWithApitype(apitype, file, param);
	}

	if((apitype & 0xF0) != 0x20)
	{
		Fix150Path(file);
		Fix150Path(param->argp);

		const char *file_str = file + 2;

		if (strstr(file, "0:/PSP/GAME150/") == file_str)
		{
//			reboot150 = 1;
			KXploitString(file);
			KXploitString(param->argp);
			Load150Module(file);
		}
/*
		else if (strstr( file, "0:/PSP/GAME/") == file_str )
		{
//			if (strstr( file_str, "0:/PSP/GAME/UPDATE/") == NULL)
			{
				if(config.gamekernel150)
				{
					reboot150 = 1;
				}
			}
		}

		if (reboot150 == 1)
		{
			KXploitString(file);
			KXploitString(param->argp);
			Load150Module(file);
		}
*/
		param->args = strlen(param->argp) + 1; // update length
	}

	pspSdkSetK1(k1);
	return sctrlKernelLoadExecVSHWithApitype(apitype, file, param);
}

int sceCtrlReadBufferPositivePatched(SceCtrlData *pad, int no)
{
	int r = sceCtrlReadBufferPositive(pad, no);
	int k1 = pspSdkSetK1(0); 

	if(clock_flag == 0)
	{
		if(config.vshcpuspeed != 0)
		{
			int i = sceKernelGetSystemTimeLow();
			if((i - firsttime) >= (10 * 1000 * 1000))
			{
				SetSpeed(config.vshcpuspeed ,config.vshbusspeed);
				clock_flag = 1;
				real_time  = i;
				firsttime  = 0;
			}
		}
	}
	else
	{
		int cpu = scePowerGetCpuClockFrequency();
		if(config.vshcpuspeed != 0)//&& config.vshcpuspeed != 222	
		{
			u32 diff = config.vshcpuspeed - cpu;
			if(diff > 1)
			{
				int i = sceKernelGetSystemTimeLow();
				if((i - real_time) >= (1 * 1000 * 1000))
				{
//					printf("clock walker\n");
					SetSpeed(config.vshcpuspeed ,config.vshbusspeed);
					real_time = i;
				}
			}
		}
		else
		{
			if(cpu != 222)
			{
//				printf("clock walker2\n");
				SetSpeed(222, 111);
			}

			clock_flag = 0;
		}
	}

	if(sceKernelFindModuleByName("VshCtrlSatelite") == 0)
	{
		u32 bttons = pad->Buttons;
/*
		if( bttons & PSP_CTRL_SELECT)
		{
			printf("Select \n ");
		}
*/
		if ((bttons & PSP_CTRL_SELECT) && 
			((bttons & (PSP_CTRL_RTRIGGER|PSP_CTRL_LTRIGGER)) == (PSP_CTRL_RTRIGGER|PSP_CTRL_LTRIGGER) || ( 
			sceKernelFindModuleByName("htmlviewer_plugin_module") == NULL &&
			sceKernelFindModuleByName("sceVshStoreBrowser_Module") == NULL &&
			sceKernelFindModuleByName("sceVshOSK_Module") == NULL &&
			sceKernelFindModuleByName("Skyhost") == NULL &&
			sceKernelFindModuleByName("premo_plugin_module") == NULL &&
			sceKernelFindModuleByName("oneseg_plugin_module") == NULL &&
			sceKernelFindModuleByName("camera_plugin_module") == NULL )))
		{
//			sceKernelSetDdrMemoryProtection(0x08400000,0x400000,15);//(0x73546131)

#if 0
			satelitedfd = sceKernelLoadModule("ms0:/seplugins/satellite.prx", 0, NULL);

			if(satelitedfd < 0)
#endif
			satelitedfd = sceKernelLoadModule("flash0:/vsh/module/satellite.prx", 0, NULL);
			if(satelitedfd > 0)
			{
				int m = 0;
				char *umd = NULL;

				if(iso_mount)
				{
					umd = sctrlSEGetUmdFile();
					m = strlen(umd) + 1;
//					printf("disc: %s \n", umd);
				}

				sceKernelStartModule(satelitedfd, m, umd, 0, 0);
				pad->Buttons &=~(PSP_CTRL_SELECT);
				//pad->Buttons &= 0xFFFFFFFE;
			}
		}
	}
	else
	{
		if(vshmenu_kernel)
		{
			vshmenu_kernel(pad, no);
		}
		else
		{
			if(satelitedfd >= 0)
			{
				if(sceKernelStopModule(satelitedfd, 0, NULL, NULL, NULL) >= 0)
				{
					sceKernelUnloadModule(satelitedfd);
				}
			}
		}
	}

	if(umd_timer && iso_mount)
	{	
		int i = sceKernelGetSystemTimeLow();
		if((i - umd_timer) >= 500 * 1000)
		{
			if(umd_mode)
			{
//				printf("timer start \n");
				pspUmdCallback(0x32);
			}	

			umd_timer = 0;
		}
	}

	pspSdkSetK1(k1);
	return r;
}

static const char *version_txt_path[] = {
	"ms0:/seplugins/version.txt",
	"ef0:/seplugins/version.txt",
	"flash0:/vsh/etc/version.txt"
};

int (* sceResmgr_driver_9DC14891_k)(char *buf, int size, int *ret) = NULL;
int PatchVer(char *buf, int size, int *ret)
{
	SceUID mod;
	int r = sceResmgr_driver_9DC14891_k(buf, size, ret);

	int k1 = pspSdkSetK1(0); 
	if(strstr(buf, "release:") != NULL)
	{
		int i;
		for(i = 0; i<(sizeof(version_txt_path) / sizeof(char *)); i++)
		{
			mod = sceIoOpen(version_txt_path[i], PSP_O_RDONLY, 0);
			if(mod >= 0)
			{
				*ret = sceIoRead(mod, buf, size);
				sceIoClose(mod);
				break;
			}
		}
	}

	pspSdkSetK1(k1);
	return r;
}

int PatchDevctl(const char *dev, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	int ret = sceIoDevctl(dev,cmd,indata,inlen,outdata,outlen);
	int k1 = pspSdkSetK1(0);
	
	if(cmd == 0x01E18030 && strcmp(dev, "umd0:") == 0)
	{
		if(iso_mount)
		{
			ret = 1;
		}
		else
		{
			if(umd_mode)
				sctrlKernelExitVSH(NULL);
		}
	}
/*
	else if( umd_mode && cmd == 0x01F20001 )
	{
		((int *)outdata)[0]= -1;
		((int *)outdata)[1]= sctrlSEGetDiscType();	
		ret = 0;
	}
*/
	pspSdkSetK1(k1);
	return ret;
}

int sceUmdRegisterUMDCallBackPatch(int cbid)
{
	int k1 = pspSdkSetK1(0);
	umd_id = cbid;
	int id = sceKernelCreateCallback("isof", exit_cb, NULL);
	int ret = sceUmdRegisterUMDCallBack(id);
	pspSdkSetK1(k1);
	return ret;
}

static void PatchVshMain(SceModule2 *mod )
{
	u32 buf = mod->text_addr;
	//uncheck PBP Header

	/* kill DISC_ID check ( may needless ) */
	_sw(0		, buf + PBP_HEADER_PATCH1 );
	_sw(0		, buf + PBP_HEADER_PATCH2 );

	/*   */
	_sw(0		, buf + PBP_HEADER_PATCH3 );

	MAKE_SYSCALL( (u32)search_module_stub( mod, "sceUmdUser", 0xAEE7404D) + 4 , sceKernelQuerySystemCall(sceUmdRegisterUMDCallBackPatch));
//	MAKE_SYSCALL( buf + VSH_UMD_CALLBACK_PATCH_ADDR , sceKernelQuerySystemCall(sceUmdRegisterUMDCallBackPatch));

	MAKE_SYSCALL( buf +	VSH_DISC_VERSION_PATCH_ADDR , sceKernelQuerySystemCall( sfo_patch ) );
	get_sfo_param = (void *)( ( buf +	VSH_DISC_VERSION_FUNC_ADDR  )| 0x80000000 );


	if( psp_model == 4 )
	{
		if(! config.netupdate)
		{
			//addiu      $v1, $zr, 2
			_sw( 0x24030002 , buf + VSH_UPDATE_TYPECHECK_PACH_ADDR );
		}

		if(config.startupprog)
		{	
			//device type vsh_890CAA93	
			MAKE_DUMMY_FUNCTION1( buf + VSH_UMDVIDEO_PATCH_ADDR1 );

			//icon load up
			_sw( 0, buf + VSH_UMDVIDEO_PATCH_ADDR2 );
		}
	}

	IoPatches();

	mod = sceKernelFindModuleByName("sceVshBridge_Driver"); 
	u32 text_addr = mod->text_addr;

	if (!config.novshmenu) 
	{ 
		MAKE_CALL( text_addr + VSH_READ_KEY_PATCH_ADDR, sceCtrlReadBufferPositivePatched);
		PatchSyscall(FindProc("sceController_Service", "sceCtrl", 0x1F803938), sceCtrlReadBufferPositivePatched);
	}

	if( config.skipgameboot)
		_sw( 0x00001021 , text_addr + VSH_SKIP_GAMEBOOT_PATCH_ADDR );

	MAKE_JUMP( (u32)search_module_stub( mod, "IoFileMgrForKernel", 0x54F5FB11 ), PatchDevctl );
	//MAKE_CALL( text_addr + VSH_DEVCTRL_PATCH_ADDR , PatchDevctl);

	if(psp_model == 4 && !config.netupdate)
	{
		//hib block
		MAKE_DUMMY_FUNCTION0(text_addr + VSH_HIB_BLOCK_PATCH_ADDR);
	}

	if(config.versiontxt)
	{
		u32 addr = FindProc("sceMesgLed", "sceResmgr", 0x9DC14891);
		sctrlHENPatchSyscall(addr, PatchVer);//Version.txt
		sceResmgr_driver_9DC14891_k = (void *)addr;
	}

	PatchSyscall(FindProc("sceUSB_Driver", "sceUsb", 0xAE5DE6AF), PatchUsbStart);//sceUsbStart
	PatchSyscall(FindProc("sceUSB_Driver", "sceUsb", 0xC2464FA0), PatchUsbStop);//sceUsbStop

	ClearCaches();
}

static wchar_t fake_mac[] = L"XX:XX:XX:XX:XX:XX";

//wchar_t verinfo[] = L"%1 ME   ";

#ifdef HEN
//wchar_t verinfo[] = L""CFW_VER" LME   ";
wchar_t verinfo[] = L"%1 LME      ";
#else
//wchar_t verinfo[] = L""CFW_VER" ME   ";
wchar_t verinfo[] = L"%1 ME      ";
#endif

static void PatchSysconfPlugin(u32 text_addr)
{
	int i;
	char ver[32];
	u32 addrlow, addrhigh;
	u32 sev = sctrlSEGetVersion();
	u8 minor = (sev & 0xF);
	u8 major = ((sev & 0xFF) >> 4);

	if (minor > 0)
	{
		sprintf(ver, "%i.%i", major, minor);
	}
	else
	{
		sprintf(ver, "%i", major);
	}

#ifdef HEN
	verinfo[6] = (wchar_t)'-';

	for (i = 0; i < strlen(ver); i++)
	{
		verinfo[7+i] = (wchar_t)ver[i];
	}
#else
	verinfo[5] = (wchar_t)'-';
	
	for (i = 0; i < strlen(ver); i++)
	{
		verinfo[6+i] = (wchar_t)ver[i];
	}
#endif

	memcpy((void *)(text_addr+ SP_MODULE_NAME), verinfo, sizeof(verinfo));

	addrhigh = (text_addr+ SP_MODULE_NAME) >> 16;
	addrlow = (text_addr+ SP_MODULE_NAME) & 0xFFFF;

	// lui v0, addrhigh
	_sw(0x3c020000 | addrhigh, text_addr + SP_PATCH_ADDR);
	// ori v0, v0, addrlow
	_sw(0x34420000 | addrlow, text_addr+ SP_PATCH_ADDR + 4);

	if(config.umdactivatedplaincheck)
	{
		memcpy((void *)(text_addr+ SYSCON_MAC_PATCH_ADDR ), fake_mac , sizeof(fake_mac));
	}

	if( psp_model == 0)
	{
		if( config.usbcharge )
		{
			u32 value = *(u32 *)( text_addr + SYSCON_COLOR_PATCH_ADDR + 4);
			_sw( 0x24020001 , text_addr + SYSCON_COLOR_PATCH_ADDR + 4);
			_sw( value , text_addr + SYSCON_COLOR_PATCH_ADDR );
		}
	}

	ClearCaches();
}


static void PatchGamePlugin(u32 text_addr )
{
	_sw(0x03E00008, text_addr + GP_PATCH_ADDR1);	//jr	$ra
	_sw(0x00001021, text_addr + GP_PATCH_ADDR1 + 4);	//addu	$v0, $zr, $zr

	_sw(0x03E00008, text_addr + GP_PATCH_ADDR2);
	_sw(0x00001021, text_addr + GP_PATCH_ADDR2 + 4);

	//multi disc patch
	_sw( 0 , text_addr + MULTI_DISC_PSX_PATCH_ADDR );


	//Lisence error patch
	_sw( 0x00001021 , text_addr + RIF_CHECK_PATCH_ADDR );
//	_sw(0x00001021, text_addr + 0x00001AFC );


	if(config.hidepng)
	{
		_sw(0x00601021, text_addr + HIDE_PNG_PATCH1);//addu	$v0, $v1, $zr
		_sw(0x00601021, text_addr + HIDE_PNG_PATCH2);
	}

	if(config.skipgameboot)
	{
		MAKE_CALL(  text_addr + SKIP_GAMEBOOT_PATCH_ADDR ,  text_addr + SKIP_GAMEBOOT_FUNC_ADDR );
		_sw( 0x24040002 , text_addr + SKIP_GAMEBOOT_PATCH_ADDR + 4);
	}

	ClearCaches();
}

static void PatchMsVideoMainPlugin(u32 text_addr )
{
	// Patch resolution limit to 130560 pixels (480x272)
 
	// Allow play avc 320*240 -> 480*272
	//ori $v0, $v0, 0x2C00 -> ori $v0, $v0, 0xFE00
	_sh(0xFE00, text_addr+ MSVM_RES_PATCH1_ADDR);
	_sh(0xFE00, text_addr+ MSVM_RES_PATCH2_ADDR);
	//ori v1
	_sh(0xFE00, text_addr+ MSVM_RES_PATCH3_ADDR);
	//ori a0
	_sh(0xFE00, text_addr+ MSVM_RES_PATCH4_ADDR);
	//ori v0
	_sh(0xFE00, text_addr+ MSVM_RES_PATCH5_ADDR);
	_sh(0xFE00, text_addr+ MSVM_RES_PATCH6_ADDR);
	_sh(0xFE00, text_addr+ MSVM_RES_PATCH7_ADDR);

	// Patch bitrate limit	(increase to 16384+2)
	//sltiu $v0, $v0, 0x303 -> sltiu $v0, $v0, 0x4003
	_sh(0x4003, text_addr+ MSVM_BR_PATCH1_ADDR);

	//sltiu	$v0, $v0, 4001 -> sltiu $v0, $v0, 0x4003
	_sh(0x4003, text_addr+ MSVM_BR_PATCH2_ADDR);
	_sh(0x4003, text_addr+ MSVM_BR_PATCH3_ADDR);

	ClearCaches();
}

static void PatchUpdatePlugin(u32 text_addr )	
{
	u32 var = sctrlSEGetVersion();

	//beq        $v0, $zr,  0x1040XXXX
	_sw( 0x10400002 , text_addr + NET_UPDATE_VERSION_PATCH_OFFSET );

	_sw( (var >> 16)	| 0x3c050000 , text_addr + NET_UPDATE_VERSION_PATCH_OFFSET + 4 );
	_sw( (var & 0xFFFF)	| 0x34A40000 , text_addr + NET_UPDATE_VERSION_PATCH_OFFSET + 12 );

	ClearCaches();
}

static void PatchNetUptate( u32 pos )
{	
	if(sceKernelFindModuleByName("sceVshNpSignin_Module") == NULL && 
		sceKernelFindModuleByName("npsignup_plugin_module") == NULL )
	{
		pspTime tick;
		sceRtcGetCurrentClockLocalTime (&tick);
		u8 value = (tick.seconds)%5;

		//dest = ?0
		_sw( 0x24020030 | value , pos + UPDATE_LIB_DEST_PATCH_ADDR1 );

		//dest = 0?
#ifdef HEN
		_sw( 0x24020032 , pos + UPDATE_LIB_DEST_PATCH_ADDR2 );
#else		
		_sw( 0x24020030 , pos + UPDATE_LIB_DEST_PATCH_ADDR2 );
#endif	

		_sw( 0 , pos + UPDATE_LIB_STRCAT_PATCH1 );	
		_sw( 0 , pos + UPDATE_LIB_STRCAT_PATCH2 );
		_sw( 0 , pos + UPDATE_LIB_STRCAT_PATCH3 );
		strcpy( (char *)( pos + UPDATE_LIB_URL_PATCH ), "akiba.geocities.jp/psp_shell/");

		ClearCaches();
	}
}

int OnModuleRelocated(SceModule2 *mod)
{

	u32 modbuf=mod->text_addr;
	char *name = mod->modname;

	if (strcmp( name , "vsh_module") == 0)
	{
		PatchVshMain( mod );

//		sceKernelDelayThread( 3 * 1000 * 1000);
//		printf("addr 0x%08X \n", (u32)vshmount );
	}
	else if (strcmp( name, "msvideo_main_plugin_module") == 0)
	{
		PatchMsVideoMainPlugin(modbuf); 
	}
	else if (strcmp( name, "sysconf_plugin_module") == 0)
	{
		PatchSysconfPlugin(modbuf); 
	}
	else if (strcmp( name, "game_plugin_module") == 0)
	{
		PatchGamePlugin(modbuf);
	}
	else if (strcmp( name, "update_plugin_module") == 0)
	{
		if( !config.netupdate)
			PatchUpdatePlugin(modbuf);
	}
	else if( strcmp( name, "SceUpdateDL_Library") == 0 )
	{
		if( !config.netupdate)
			PatchNetUptate(modbuf);
	}

//	printf("%s \n", name);

	if (!previous)
		return 0;

	return previous(mod);
}

static void LoadExecPatch()
{
	SceModule2 *mod = sceKernelFindModuleByName("sceLoadExec");
	u32 text_addr = mod->text_addr;

	if( psp_model != 4 )
	{
//		MAKE_CALL(text_addr+ LE_VSH_COMMON_CALL1_ADDR, LoadExecVSHCommonPatched); //sceKernelLoadExecVSHDisc
//		MAKE_CALL(text_addr+ LE_VSH_COMMON_CALL2_ADDR, LoadExecVSHCommonPatched); //sceKernelLoadExecVSHDiscUpdater
//		MAKE_CALL(text_addr+ LE_VSH_COMMON_CALL3_ADDR, LoadExecVSHCommonPatched); //sceKernelLoadExecVSHMs1
		MAKE_CALL(text_addr+ LE_VSH_COMMON_CALL4_ADDR, LoadExecVSHCommonPatched); //sceKernelLoadExecVSHMs2

		MAKE_CALL(text_addr+ LE_VSH_COMMON_CALL5_ADDR, LoadExecVSHCommonPatched);//UMDEMU_Ms
		MAKE_CALL(text_addr+ LE_VSH_COMMON_CALL6_ADDR, LoadExecVSHCommonPatched);//PBOOT_Ms

//		MAKE_CALL(text_addr+ 0x00001F78, LoadExecVSHCommonPatched);
	}
	else
	{
//		MAKE_CALL(text_addr + LE_VSH_COMMON_CALL1_ADDR_05g , LoadExecVSHCommonPatched); //sceKernelLoadExecVSHMs1
		MAKE_CALL(text_addr + LE_VSH_COMMON_CALL2_ADDR_05g , LoadExecVSHCommonPatched); //sceKernelLoadExecVSHMs2
//		MAKE_CALL(text_addr + LE_VSH_COMMON_CALL3_ADDR_05g , LoadExecVSHCommonPatched); //sceKernelLoadExecVSHEf1
		MAKE_CALL(text_addr + LE_VSH_COMMON_CALL4_ADDR_05g , LoadExecVSHCommonPatched); //sceKernelLoadExecVSHEf2

		MAKE_CALL(text_addr + LE_VSH_COMMON_CALL5_ADDR_05g , LoadExecVSHCommonPatched); //UMDEMU_Ms
		MAKE_CALL(text_addr + LE_VSH_COMMON_CALL6_ADDR_05g , LoadExecVSHCommonPatched); //PBOOT_Ms
		MAKE_CALL(text_addr + LE_VSH_COMMON_CALL7_ADDR_05g , LoadExecVSHCommonPatched); //UMDEMU_Ef
		MAKE_CALL(text_addr + LE_VSH_COMMON_CALL8_ADDR_05g , LoadExecVSHCommonPatched); //PBOOT_Ef
	}

	ClearCaches();

}
int module_start(SceSize args, void *argp)
{
	psp_model = sceKernelGetModel();

	LoadExecPatch();

	sctrlSEGetConfig(&config);
	sctrlSESetUmdFileEx( "", vshmount);

	if (config.vshcpuspeed != 0 )//|| !config.novshmenu
	{
		firsttime = sceKernelGetSystemTimeLow();
	}

	previous = sctrlHENSetStartModuleHandler(OnModuleRelocated); 

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}


#include <pspkernel.h>
#include <pspusb.h>
#include <pspusbstor.h>
#include <pspctrl.h>
#include <psputility.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <psploadexec_kernel.h>
#include <pspvshbridge.h>

#include "pspusbdevice.h"

#include "main.h"
#include "action.h"
#include "text.h"

extern SEConfig config;
extern VshConfig config_mini;
extern int psp_model;

char str_buffer[256];


const u16 cpu_list[]={0, 20, 75, 100, 133, 166, 200, 222, 266, 300, 333};
#define MAX_CLK_LIST (sizeof(cpu_list)/sizeof(u16))

const u16 cache_sels[] = { 64, 128, 160, 196, 256, 320, 384, 512, 640, 768 };
#define MAX_CACHE_SEL (sizeof(cache_sels)/sizeof(u16))

static char *cache_policy[] = {"LRU", "Random"};
#define ISO_CACHE_POLICY_CNT 2

char *iso_cache_policy(int type, int direction )
{
	char *bridge;
	char buffer[256];

	if(type == FUNC_GET_STR)
	{	
		my_sprintf( buffer ,"%s", cache_policy[limit( config.iso_cache_policy , 0 , ISO_CACHE_POLICY_CNT - 1)]);	
		bridge = buffer;

		return bridge;
	}
	
	else if(type == FUNC_CHANGE_VALUE)
	{
		int sel = limit( config.iso_cache_policy + direction , 0, ISO_CACHE_POLICY_CNT - 1 );
		config.iso_cache_policy = sel;
		
		send_msg(iso_cache_policy(FUNC_GET_STR, 0));
	}

	return NULL;
}

char *iso_cache_num(int type, int direction )
{
	char *bridge;
	char buffer[256];

	if(type == FUNC_GET_STR)
	{	
		my_sprintf( buffer ,"%d", cache_sels[limit( config.iso_cache_num , 0 , MAX_CACHE_SEL - 1)]);	
		bridge = buffer;
		
		return bridge;
	}

	else if(type == FUNC_CHANGE_VALUE)
	{
		int sel = limit( config.iso_cache_num + direction , 0, MAX_CACHE_SEL - 1 );
		config.iso_cache_num = sel;
		
		send_msg(iso_cache_num(FUNC_GET_STR, 0));
	}

	return NULL;
}

char *iso_cache_total_size(int type, int direction)
{
	char *bridge;
	char buffer[256];

	if(type == FUNC_GET_STR)
	{	
		my_sprintf( buffer ,"%d Mb", limit( config.iso_cache_total_size , 1 , 24));	
		bridge = buffer;
		
		return bridge;
	}

	else if(type == FUNC_CHANGE_VALUE)
	{
		int sel = limit( config.iso_cache_total_size + direction , 1, 24 );
		config.iso_cache_total_size = sel;
		
		send_msg(iso_cache_total_size(FUNC_GET_STR, 0));
	}

	return NULL;
}

int cpu2no(int cpu)
{
	int i;

	for(i=0;i< MAX_CLK_LIST;i++)
	{
		if(cpu==cpu_list[i])
			return i;
	
	}
	return 0;
}

char *clock_control(int type ,int direction, int * current_clock , int *current_bus )
{
	int sel = cpu2no( *current_clock );
	int len;

	if(type == FUNC_GET_STR)
	{
		if( sel )
		{
			my_sprintf(str_buffer ,"%d/%d",  cpu_list[sel] ,  cpu_list[sel]/2 );
		//	len = itoa( cpu_list[sel] , str_buffer );

		//	str_buffer[len + 1] = '/';
		//	itoa( cpu_list[sel] / 2 , str_buffer + len + 2 );

		}
		else
		{
			strcpy( str_buffer , recovery_str.Default );
		}

		return str_buffer;
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
		sel = limit( sel + direction ,0, MAX_CLK_LIST - 1 );
		len = cpu_list[sel];

		*current_clock = len;
		*current_bus = len/2;

		if(len)
			itoa( len , str_buffer);
		else
			strcpy( str_buffer , recovery_str.Default );

//		send_msg("Change to ");
//		printf(str_buffer);
		send_msg(str_buffer);
	}

	return NULL;
}

char *vsh_clock(int type , int direction)
{
	return clock_control( type , direction, &(config.vshcpuspeed) , &(config.vshbusspeed));
}

char *game_clock(int type, int direction)
{
	return clock_control( type , direction, &(config.umdisocpuspeed) , &(config.umdisobusspeed) );
}

static char ** umd_mode[] =
{
	&recovery_str.umd_mode_list.Normal,
	&recovery_str.umd_mode_list.OE,
	&recovery_str.umd_mode_list.M33,
	&recovery_str.umd_mode_list.NP9660,
	&recovery_str.umd_mode_list.ME,
	&recovery_str.umd_mode_list.INFERNO
};
#define UMD_MODE_CNT 6

/*
static const char * umd_mode[] =
{
	"Normal -UMD required-",
	"OE isofs legacy -NO UMD-",
	"M33 driver -NO UMD-",
	"Sony NP9660 -NO UMD-",
	"ME driver -NO UMD-",
	
};
*/

const char *umdmode_change(int type, int direction )
{
	if(type == FUNC_GET_STR)
	{	
		return *umd_mode[ limit( config.umdmode , 0 , UMD_MODE_CNT - 1)];
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
		int umd_type = limit( config.umdmode + direction, (psp_model == 4) ? 1 : 0, UMD_MODE_CNT - 1 );
		config.umdmode = umd_type;

//		send_msg("Change to ");
//		printf( *umd_mode[umd_type]);
		send_msg( *umd_mode[umd_type]);
	}

	return NULL;
}
/*
char *dummy_change(int type)
{
	if(type == FUNC_GET_STR)
	{	
		strcpy( str_buffer , "Not yet");	
		return str_buffer;
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
		send_msg("This config is not available yet.");
	}

	return NULL;
}
*/
char ** fake_region[] =
{
	&recovery_str.Disabled,
	&recovery_str.region_list.Japan,
	&recovery_str.region_list.America,
	&recovery_str.region_list.Europe,
	&recovery_str.region_list.Korea,
	&recovery_str.region_list.UnitedKingdom,
	&recovery_str.region_list.Mexico,
	&recovery_str.region_list.Australia,
	&recovery_str.region_list.East,
	&recovery_str.region_list.Taiwan,
	&recovery_str.region_list.Russia,
	&recovery_str.region_list.China,
	&recovery_str.region_list.DebugI,
	&recovery_str.region_list.DebugII
};

/*
static const char * fake_region[] =
{
	"Disabled",//recovery_str.Disabled,
	"Japan",
	"America",
	"Europe",
	"Korea",
	"United Kingdom",
	"Mexico",
	"Australia/New Zealand",
	"East",
	"Taiwan",
	"Russia",
	"China",
	"Debug Type I",
	"Debug Type II"
};
*/

#define FAKE_REGION_CNT 14

const char *region_change(int type, int direction)
{
	if(type == FUNC_GET_STR)
	{	
		//strcpy( str_buffer , fake_region[ limit( config.fakeregion , 0 , FAKE_REGION_CNT - 1)]);	
		return *fake_region[ limit( config.fakeregion , 0 , FAKE_REGION_CNT - 1)];
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
		int region = limit( config.fakeregion + direction ,0, FAKE_REGION_CNT - 1 );
		config.fakeregion = region;

//		send_msg("Change to ");
//		printf( *fake_region[region]);
		send_msg( *fake_region[region]);
	}

	return NULL;
}
/*
static char ** folder_name[] =
{
	&recovery_str.folder_name_list.Folder3xx,
	&recovery_str.folder_name_list.Folder150
};
*/
/*
static const char * folder_name[] =
{
	"6.XX Kernel",
	"1.50 Kernel"
};
*/
/*
const char *folder_change(int type, int direction)
{
	if(type == FUNC_GET_STR)
	{	
		return *folder_name[ limit( config.gamekernel150 , 0 , 1)];
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
		int region = limit( config.gamekernel150 + direction ,0, 1 );
		config.gamekernel150 = region;

		send_msg("Change to ");
		printf( *folder_name[region]);
	}

	return NULL;
}
*/

typedef struct {
	char** name;
	const u32 color;
} ColorList;

ColorList color_list[] = {
	{ &(recovery_str.color_list.green)		, 0x0000FF00 },
	{ &(recovery_str.color_list.red)		, 0x000000FF },
	{ &(recovery_str.color_list.blue)		, 0x00FF0000 },
	{ &(recovery_str.color_list.grey)		, 0x00808080 },
	{ &(recovery_str.color_list.pink)		, 0x00FF80FF },
	{ &(recovery_str.color_list.purple)		, 0x00FF0080 },
	{ &(recovery_str.color_list.turquoise)	, 0x00FFFF00 },
	{ &(recovery_str.color_list.orange)		, 0x004080FF },
	{ &(recovery_str.color_list.yellow)		, 0x0000FFFF },
	{ &(recovery_str.color_list.white)		, 0x00FFFFFF }
};

const char *color_change(int type, int direction)
{
	static u32 cur_color = 0;
	if(type == FUNC_GET_STR)
	{	
		return *color_list[ limit( cur_color , 0 , 9)].name;
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
		int sel = limit( cur_color + direction ,0, 9 );
		cur_color = sel;
		set_select_color( color_list[ cur_color ].color );

//		send_msg("");
//		printf( color_change( FUNC_GET_STR, 0) );
		send_msg( color_change( FUNC_GET_STR, 0) );
	}
	else if( type == FUNC_GET_INT )
	{
		return (void *)cur_color;
	}

	return NULL;
}

char ** vsh_color_list[] = {
	&recovery_str.color_list.red,
	&recovery_str.color_list.green,
	&recovery_str.color_list.blue,
	&recovery_str.color_list.pink,
	&recovery_str.color_list.purple,
	&recovery_str.color_list.orange,
	&recovery_str.color_list.yellow,
	&recovery_str.color_list.black,
	&recovery_str.color_list.white
};

const char *vsh_color_change(int type, int direction)
{
	if(type == FUNC_GET_STR)
	{	
		return *vsh_color_list[limit( config_mini.vsh_color, 0, 8)];
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
		int sel = limit(config_mini.vsh_color + direction, 0, 8);
		config_mini.vsh_color = sel;
		send_msg(*vsh_color_list[sel]);
	}

	return NULL;
}

char *usb_change(int type, int direction)
{
	int status = config.usbdevice;	
	char *bridge;

	if(type == FUNC_CHANGE_VALUE)
	{
		status = limit( status + direction ,0, 5 );
	}

	if( status > 0 && status < 5)
	{
		my_sprintf( str_buffer ,"Flash %d", status -1);	
		bridge = str_buffer;
	}
	else if( status == 5)
	{
		if( psp_model == 4 )
			bridge = recovery_str.HiddenStorage;
		else
			bridge = recovery_str.UmdDisc;
	}
	else
	{
		bridge = recovery_str.MemoryStick;//"Memory Stick";
	}
	
	if(type == FUNC_GET_STR)
	{	
		return bridge;
	}

	config.usbdevice = status;
	return NULL;
}

static char **wma[] =
{
	&(recovery_str.registry_list.Wma_on),
	&(recovery_str.registry_list.Wma_already)
};

void activate_wma()
{
	u32 value;

	getRegistryValue("/CONFIG/MUSIC", "wma_play", &value);
	if (value == 0)
	{
		setRegistryValue("/CONFIG/MUSIC", "wma_play", 1);	
	}

	send_msg( *wma[value & 1]);

	sceKernelDelayThread(1*1000*1000);
	myDebugScreenClear();
}

static char **flashplayer[] =
{
	&(recovery_str.registry_list.FPlayer_on),
	&(recovery_str.registry_list.FPlayer_already)
};

void activate_flash()
{
	u32 value;

	getRegistryValue("/CONFIG/BROWSER", "flash_activated", &value);

	if (value == 0)
	{
		setRegistryValue("/CONFIG/BROWSER", "flash_activated", 1);	
		setRegistryValue("/CONFIG/BROWSER", "flash_play", 1);
	}

	send_msg( *flashplayer[value & 1]);

	sceKernelDelayThread(1*1000*1000);
	myDebugScreenClear();
}

static char ** isEnter[] =
{
	&(recovery_str.registry_list.Button_o),
	&(recovery_str.registry_list.Button_x)
};
u32 key_value = 0xFFFFFFFF;
const char *swap_key(int type)
{
	u32 value;

	if(key_value == 0xFFFFFFFF)
	{
		getRegistryValue("/CONFIG/SYSTEM/XMB", "button_assign", &value);
		key_value = value;
	}
	else
	{
		value = key_value;
	}

	if(type == FUNC_GET_STR)
	{	
//		strcpy( str_buffer , isEnter[value & 1]);	
		return ( *isEnter[value & 1]);
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
		value = 1 - value;
		setRegistryValue("/CONFIG/SYSTEM/XMB", "button_assign", value);
		key_value = value;

		send_msg( *isEnter[value & 1]);
	}

	return NULL;
}


int usb_status = -1;

int disable_usb()
{
	if(usb_status < 0)
		return 0;

	sceUsbDeactivate(0);
	sceUsbStop(PSP_USBSTOR_DRIVERNAME, 0, 0);	
	sceUsbStop(PSP_USBBUS_DRIVERNAME, 0, 0);

	if(usb_status > 0)
		pspUsbDeviceFinishDevice();

	usb_status = -1;
	return 0;
}
int usb_ctrl(int driver)
{
//	int model = vshKernelGetModel();

	if(usb_status >= 0 )
	{
		int stock = usb_status;

		disable_usb();
		sceKernelDelayThread( 300*1000 );

		if( stock == driver)
		{
			send_msg( recovery_str.UsbDisabled );
//			sceKernelDelayThread(1*1000*1000);
//			myDebugScreenClear();
			return 0;
		}
	}


	send_msg( recovery_str.UsbEnabled );

	if( driver > 0 )
	{
		driver = limit( driver  , 1 , 4 );
		pspUsbDeviceSetDevice( driver -1, 0, 0);
	}

	sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
	sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0);
	sceUsbstorBootSetCapacity(0x800000);
	sceUsbActivate((psp_model) ? 0x2D2 : 0x1C8 );

	usb_status = driver;

//	sceKernelDelayThread(1*1000*1000);
//	myDebugScreenClear();

	return 0;
}

extern Menu_pack menu_main[];

char * usb_ctrl_switch(int type, int cnt)
{
	if(type == FUNC_GET_STR)
	{	
		my_sprintf(str_buffer ,"%s (flash%d)",  menu_main[0].path ,  cnt - 1 );
		return str_buffer;
	}

	usb_ctrl( cnt );
	return NULL;
}
int toggle_usb_ms()
{ 
	usb_ctrl_switch( FUNC_CHANGE_VALUE , 0 );
	sceKernelDelayThread(1*1000*1000);
	myDebugScreenClear();
	return 0;
}

void* toggle_usb_flash0(int type )
{
	return usb_ctrl_switch( type, PSP_USBDEVICE_FLASH0 +1);
}
void* toggle_usb_flash1(int type)
{
	return usb_ctrl_switch( type, PSP_USBDEVICE_FLASH1 +1);
}
void* toggle_usb_flash2(int type)
{
	return usb_ctrl_switch( type, PSP_USBDEVICE_FLASH2 +1);
}
void* toggle_usb_flash3(int type)
{
	return usb_ctrl_switch( type, PSP_USBDEVICE_FLASH3 +1);
}

#define RECOVERY_PBP "ms0:/PSP/GAME/RECOVERY/EBOOT.PBP"
#define RECOVERY_PBP_EF "ef0:/PSP/GAME/RECOVERY/EBOOT.PBP"

void* run_game(int type)
{
	if(type == FUNC_GET_STR)
	{
		my_sprintf(str_buffer ,"%s %s",  recovery_str.RunProgram , RECOVERY_PBP + 4 );
		return str_buffer;
	}

	struct SceKernelLoadExecVSHParam param;//
	SceIoStat stat;

	save_config();

	memset(&param, 0, sizeof(param));
	
	param.size = sizeof(param);
	param.args = sizeof(RECOVERY_PBP);
	param.argp = RECOVERY_PBP;
	param.key = "game";
	
	send_msg( recovery_str.Wait );
	printf(" ... ");

	if (sceIoGetstat(RECOVERY_PBP, &stat) < 0 && psp_model == 4)
	{
		param.argp = RECOVERY_PBP_EF;
		sctrlKernelLoadExecVSHEf2( RECOVERY_PBP_EF, &param);
		goto EXIT;
	}
	
	vshKernelLoadExecVSHMs2( RECOVERY_PBP ,&param);

EXIT:
	sceKernelSleepThread();
	return NULL;
}

char *autorun_umdvideo_change(int type)
{
	int status = config.startupprog;
	char *bridge = NULL;;

	switch(type){
	case FUNC_GET_STR:
		if( psp_model == 4 )
		{
			bridge = recovery_str.UmdVideoPatch;
		}
		else
		{
			my_sprintf( str_buffer , "%s %s", recovery_str.AutoRun , "/PSP/GAME/BOOT/EBOOT.PBP");
			bridge = str_buffer;
		}
		break;
	case FUNC_GET_STR2:
		bridge = (status != 0)? recovery_str.Enabled:recovery_str.Disabled;
		break;
	case FUNC_CHANGE_VALUE:
		config.startupprog = (status)?0:1;
		send_msg( recovery_str.Setting);
		printf(" ...");
		break;
	}
	
	return bridge;
}

char *usbcharge_slimcolor_change(int type)
{
	int status = config.usbcharge;
	char *bridge = NULL;;

	switch(type){
	case FUNC_GET_STR:
		if( psp_model == 0 )
		{
			bridge = recovery_str.SlimColor;
		}
		else
		{
			bridge = recovery_str.UsbCharge;
		}
		break;
	case FUNC_GET_STR2:
		bridge = (status != 0)? recovery_str.Enabled:recovery_str.Disabled;
		break;
	case FUNC_CHANGE_VALUE:
		config.usbcharge = (status)?0:1;
		send_msg( recovery_str.Setting);
		printf(" ...");
		break;
	}
	
	return bridge;
}

int AssignFlashes()
{
	int res;
	res = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
		return -1;

	res = sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
		return -1;

	return 0;
}

int UnAssignFlashes()
{
	int res;
	
	res = sceIoUnassign("flash0:");
	if (res < 0 && res != 0x80020321 )
		return -1;

	res = sceIoUnassign("flash1:");
	if (res < 0 && res != 0x80020321 )
		return -1;

	res = sceIoUnassign("flash2:");
	if (res < 0 && res != 0x80020321 )
		return -1;

//	if( vshKernelGetModel() != 0)
	if( psp_model != 0 )
	{
		res = sceIoUnassign("flash3:");
		if (res < 0 && res != 0x80020321 )
			return -1;
	}

	return 0;
}

int start_format(int flash_dev )
{
	int res;
	char *argv[2];

	res = UnAssignFlashes();
	if (res < 0)
		return -1;	


	if( flash_dev == 1)
	{
		argv[0] = "fatfmt";
		argv[1] = "lflash0:0,1";
	}
	else if( flash_dev == 2 )
	{
		argv[0] = "fatfmt";
		argv[1] = "lflash0:0,2";
	}
	else
		return -1;

	res = vshLflashFatfmtStartFatfmt(2, argv);
	if (res < 0)
		return -2;	

	res = AssignFlashes();
	if( res < 0)
		return -3;

	if( flash_dev == 1)
	{
		sceIoMkdir("flash1:/dic" , 511);
		sceIoMkdir("flash1:/gps" , 511);
		sceIoMkdir("flash1:/net" , 511);
		sceIoMkdir("flash1:/net/http" , 511 );
		sceIoMkdir("flash1:/registry" , 511 );
		sceIoMkdir("flash1:/vsh" , 511 );
		sceIoMkdir("flash1:/vsh/theme" , 511 );
	}

	return 0;
}

int format_flash(int flash_dev )
{	
	disable_usb();
	sceKernelDelayThread( 300*1000 );

	send_msg( recovery_str.Formatting );
	printf(" flash%d. %s ... ", flash_dev , recovery_str.Wait );

	int res = start_format( flash_dev );

	if(res < 0)
		printf("%s %d\n" , recovery_str.Failed, res);
	else
		printf( recovery_str.Done );

	sceKernelDelayThread(3*1000*1000);
	vshKernelExitVSHVSH(NULL);

	return 0;
}

int format_flash1()
{
	return format_flash( 1 );	
}

int format_flash2()
{
	return format_flash( 2 );
}

char eeprom_str[12] = "0x";
u16 eeprom_info[2];
int eeprom_change_flag = 0;
//const char *unknown_str = recovery_str.Unknown;//"Unknown";
static const char hex[]="0123456789ABCDEF";

char *create_eeprom_str()
{
	if( eeprom_change_flag )
	{
		if( eeprom_str[2] == 0 )
		{
			return recovery_str.Unknown;//(char *)unknown_str;
		}
	}
	else
	{
		eeprom_change_flag = 1;
		int ret = ReadBatterySerial( eeprom_info );
		if( ret < 0 )
		{
			return recovery_str.Unknown;//unknown_str;
		}
		else
		{
			u32 eeprom_u32 = eeprom_info[0] << 16 |  eeprom_info[1];
			char *cal_point = eeprom_str + 2 + 7;
			int i;
			for(i=0;i<8;i++)
			{
				cal_point[0] = hex[ eeprom_u32 & 0xF ];
				eeprom_u32 = eeprom_u32 >> 4;
				cal_point --;
			}

		}
	}

	return eeprom_str;
}

char *backup_eeprom(int type )
{

	if(type == FUNC_GET_STR)
	{
		return create_eeprom_str();
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
//		if( eeprom_change_flag && eeprom_str[2] )
		{	
			send_msg( recovery_str.Reloading );
			printf(" ... ");
			eeprom_change_flag = 0;
			create_eeprom_str();
/*			send_msg("Writing  ... ");
			WriteFile( "ms0:/serial.bin", eeprom_info , 4);
*/	
			printf( recovery_str.Done );
		}
/*		else
		{
			send_msg("Cannot get serial from battery !\n");
		}
		*/
	}

	return NULL;
}

int write_eeprom(u32 cnt )
{
	u16 buffer[2];

	buffer[0] = cnt >> 16;
	buffer[1] = cnt & 0xFFFF;

	send_msg( recovery_str.Writing );
	printf(" ... ");
	int ret = WriteBatterySerial(buffer);
	if( ret < 0 )
	{
		printf( recovery_str.Failed );
	}
	else
	{
		printf( recovery_str.Done );
	}

	eeprom_change_flag = 0;
	sceKernelDelayThread(1*1000*1000);

	create_eeprom_str();
	myDebugScreenClear();
	return 0;
}

int make_JigkickBattery()
{
	return 	write_eeprom( 0xFFFFFFFF );
}

int make_NormalBattery()
{
	u32 serial = sctrlKernelRand();
	return 	write_eeprom( serial ); // now use a random serial
}

int make_AutobootBattery()
{
	return 	write_eeprom( 0x00000000 );
}

#define MAX_MS_LIST 8

const u8 ms_flag_list[]={0, ( 1 + 2 + 4) , ( 1 ) , ( 1 + 2 ) , ( 1 + 4 ) , ( 2 ) , ( 2 + 4 ) , ( 4 ) };

typedef struct
{
	int type;
	char *path;
	int size;
}speed_ms_pack;

static const speed_ms_pack keyconf_list[] =
{
	{ 1 , "VSH", 3 },
	{ 2 , "GAME", 4},
	{ 4 , "POPS", 4}
};

static int ms2no(int flag)
{
	int i;

	for(i=0;i< MAX_MS_LIST;i++)
	{
		if(flag == ms_flag_list[i])
			return i;
	
	}
	return 0;
}

char *ms_speed_change(int type, int direction)
{
	if(type == FUNC_GET_STR)
	{	
		char *bridge = str_buffer;
		int ms_flag = config.fastms;

		if(ms_flag)
		{
			if( ms_flag == ( 1 + 2 + 4))
			{
				bridge = recovery_str.Always;
			}
			else
			{
				int offset = 0;
				int i;
				for(i=0;i<3;i++)
				{
					if(ms_flag & keyconf_list[i].type )
					{
						if(offset)
						{
							strcpy(str_buffer + offset , " & ");
							offset += 3;
						}
	
						strcpy(str_buffer + offset, keyconf_list[i].path );
						offset += keyconf_list[i].size;
					}
				}
			}
		}
		else
		{
			bridge = recovery_str.Never;
		}

		return bridge;
	}
	else if(type == FUNC_CHANGE_VALUE)
	{
		int sel = ms2no( config.fastms );
		int flag = limit( sel + direction ,0, MAX_MS_LIST - 1 );
		config.fastms = ms_flag_list[flag];

//		send_msg("-> ");
//		printf(ms_speed_change( FUNC_GET_STR, 0) );
		send_msg( ms_speed_change( FUNC_GET_STR, 0) );
	}

	return NULL;
}

static int get_data_size(int param, netData *data )
{
	int size = 0;

	switch(param){
		// string
		case 0://PSP_NETPARAM_NAME
		case 1://PSP_NETPARAM_SSID
		case 3://PSP_NETPARAM_WEPKEY
		case 5://PSP_NETPARAM_IP
		case 6://PSP_NETPARAM_NETMASK
		case 7://PSP_NETPARAM_ROUTE
		case 9://PSP_NETPARAM_PRIMARYDNS
		case 10://PSP_NETPARAM_SECONDARYDNS
		case 11://PSP_NETPARAM_PROXY_USER
		case 12://PSP_NETPARAM_PROXY_PASS
		case 14://PSP_NETPARAM_PROXY_SERVER
		case 19://auth_8021x_auth_name
		case 20://auth_8021x_auth_key
		case 22://wpa_key
			size = strlen(data->asString) + 1;
			if( size & 3)
				size = (size + 3)& ~3;
			break;

			//int
		case 2://PSP_NETPARAM_SECURE
		case 4://PSP_NETPARAM_IS_STATIC_IP
		case 8://PSP_NETPARAM_MANUAL_DNS
		case 13://PSP_NETPARAM_USE_PROXY
		case 15://PSP_NETPARAM_PROXY_PORT
		case 16://PSP_NETPARAM_UNKNOWN1
		case 17://PSP_NETPARAM_UNKNOWN2
		case 18://auth_8021x_type
		case 21://wpa_key_type
		case 23://browser_flag
		case 24://wifisvc_config
			size = sizeof(int);
			break;
	}

	return size;
}

#define NUM_NETPARAMS 24

static const char *netcnf_path[] = {
	"ms0:/netconf.bak",
	"ef0:/netconf.bak",
};
static void set_config(int target_id, void *buffer, int size)
{
	netData data;
	int data_size;
	int ret;
	u8 *param = buffer;
	u8 *end_addr = param + size;

	ret = sceUtilityCreateNetParam( target_id );
	ret = sceUtilityCopyNetParam( target_id, 0);

	while( param < end_addr)
	{
		data_size = *(int *)(param + 4);
		memset( &data, 0, 128 );
		memcpy( &data, (param + 8), data_size );
		ret = sceUtilitySetNetParam ( *(int *)(param) , &data );
		param += data_size + 8;
	}

	ret = sceUtilityCopyNetParam( 0, target_id );
}

int netcnf_restore()
{
	u32 buffer[1024/4];
	int target_id = 1;
	SceUID fd;
	int i;

	for(i=0;i<sizeof(netcnf_path)/sizeof(char*);i++)
	{
		fd = sceIoOpen(netcnf_path[i], PSP_O_RDONLY, 0);
		if( fd >= 0) break;
	}

	if (fd >= 0)
	{
		send_msg( recovery_str.Setting );
		printf(" %s ",netcnf_path[i]);

		while( sceIoRead(fd, buffer , 8 ) == 8 )
		{
			if( buffer[0] == 0x464E434E)//NCNF
			{
				int read_size = buffer[1];
				if( sceIoRead(fd, buffer , read_size) != read_size )
					break;

				while( sceUtilityCheckNetParam( target_id ) == 0 )
				{
					target_id++;
				}

				printf(".");
				set_config( target_id, buffer, read_size );
			}
		}
		sceIoClose(fd);
		printf(" ");
		printf( recovery_str.Done );
	}
	else
	{
		printf( recovery_str.Failed );
	}
	
	sceKernelDelayThread(1*1000*1000);
	myDebugScreenClear();
	return 0;
}

static int get_config(int target_id, void *out )
{
	int out_size = 0;
	int ret;
	char buffer[sizeof(netData) + 8];
	netData *data = (netData *)( buffer + 8);

	int i;
	for (i = 0; i <= NUM_NETPARAMS; i++) 
	{
		memset( data, 0, sizeof(netData));
		ret = sceUtilityGetNetParam( target_id, i, data);
		if( ret < 0)
			data->asUint = 0;
 	
		int data_size = get_data_size( i, data);
		((int *)buffer)[0] = i;
		((int *)buffer)[1] = data_size;

		memcpy( out + out_size, buffer, data_size + 8 );
		out_size += data_size + 8;
	}

	return out_size;
}

int netcnf_backup()
{
	u32 buffer[1024/4];
	int size;
	int i;
	SceUID fd;
	
	for(i=0;i<sizeof(netcnf_path)/sizeof(char*);i++)
	{
		fd = sceIoOpen( netcnf_path[i], PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
		if( fd >= 0) break;
	}

	if (fd >= 0)
	{
		send_msg( recovery_str.Saving );
		printf(" %s ",netcnf_path[i]);

		for(i=1;i< 100;i++)
		{
			if( sceUtilityCheckNetParam(i) == 0)
			{
				printf(".");
				size = get_config(i, buffer + 8/4);
				if(size > 0)
				{
					buffer[0] = 0x464E434E;//NCNF
					buffer[1] = size;
					sceIoWrite(fd, buffer, size + 8);
				}
			}
		}

		sceIoClose(fd);
		printf(" ");
		printf( recovery_str.Done );
	}
	else
	{
		printf( recovery_str.Failed );
	}

	sceKernelDelayThread(1*1000*1000);
	myDebugScreenClear();
	return 0;
}

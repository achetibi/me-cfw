
#include <pspsdk.h>
#include <psputility_sysparam.h>
#include "text.h"

#include "main.h"

MenuText recovery_str = {
	.Enabled	= "Enabled",
	.Disabled	= "Disabled",
	.Default	= "Default",
	.Formatting = "Formatting",
	.Currently	= "currently",
	.Back	= "Back",
	.Exit	= "Exit",
	.Exitting	= "Exiting recovery",
	.UsbEnabled = "USB enabled",
	.UsbDisabled = "USB disabled",
	.Always = "ALWAYS",
	.Never = "NEVER",
	.UmdDisc = "UMD Disc",
	.MemoryStick = "Memory Stick",
	.HiddenStorage = "Hidden Storage",
	.MainMenu = "Main menu",
	.SlimColor = "Use slim color in XMB",
	.UmdVideoPatch = "Use UmdVideo Patch in XMB",
	.RunProgram = "Run program at",
	.Plugins = "Plugins",
	.Unknown = "Unknown",
	.Wait = "Please wait",
	.Done = "Done",
	.Failed = "Failed",
	.Saving = "Saving",
	.Setting = "Setting",
	.Reloading = "Reloading",
	.Writing = "Writing",
	.AutoRun = "Autorun PBP at",
	.UsbCharge = "Battery charge from USB",
	.region_list = {
		.Japan = "Japan",
		.America = "America",
		.Europe = "Europe",
		.Korea = "Korea",
		.UnitedKingdom = "United Kingdom",
		.Mexico = "Mexico",
		.Australia = "Australia/New Zealand",
		.East = "East",
		.Taiwan = "Taiwan",
		.Russia = "Russia",
		.China = "China",
		.DebugI = "Debug Type I",
		.DebugII = "Debug Type II",
	},
	/*
	.folder_name_list = {
		.Folder3xx = "",//"6.XX Kernel",
		.Folder150 = "",//"1.50 Kernel",
	},
	*/
	.umd_mode_list = {
		.Normal	="Normal -UMD required-",
		.OE		="OE isofs -NO UMD-",
		.M33	="M33 driver -NO UMD-",
		.NP9660	="Sony NP9660 -NO UMD-",
		.ME		="ME driver -NO UMD-",
		.INFERNO="Inferno -NO UMD-",
	},
	.registry_list = {
		.Button_o	= "O is enter",
		.Button_x	= "X is enter",
		.Wma	= "Activate WMA",
		.Wma_on	= "Activating WMA...",
		.Wma_already = "WMA was already activated.",
		.FPlayer = "Activate Flash Player",
		.FPlayer_on	= "Activating Flash Player...",
		.FPlayer_already= "Flash player was already activated.",
	},
	.color_list = {
		.green		= "Green",
		.red		= "Red",
		.blue		= "Blue",
		.grey		= "Grey",
		.pink		= "Pink",
		.purple		= "Purple",
		.turquoise	= "Turquoise",
		.orange		= "Orange",
		.yellow		= "Yellow",
		.white		= "White",
		.black		= "Black",
	},
};

extern Menu_pack menu_main[];
extern Menu_pack menu_config[];
extern Menu_pack menu_advance[];
extern Menu_pack menu_registryhack[];
extern Menu_pack menu_ad_cnf[];
extern Menu_pack menu_cpu[];
extern Menu_pack battery_cnf[];
extern Menu_pack menu_misc[];

char **TextAddrList[] = {
	&(recovery_str.Disabled),//Disabled
	&(recovery_str.Enabled),//Enabled
	&(recovery_str.Default),//Default	

	&recovery_str.region_list.Japan,//Japan
	&recovery_str.region_list.America,//America
	&recovery_str.region_list.Europe,//Europe
	&recovery_str.region_list.Korea,//Korea
	&recovery_str.region_list.UnitedKingdom,//United Kingdom
	&recovery_str.region_list.Mexico,//Mexico
	&recovery_str.region_list.Australia,//Australia/New Zealand
	&recovery_str.region_list.East,//East
	&recovery_str.region_list.Taiwan,//Taiwan
	&recovery_str.region_list.Russia,//Russia
	&recovery_str.region_list.China,//China
	&recovery_str.region_list.DebugI,//Debug Type I
	&recovery_str.region_list.DebugII,//Debug Type II

	&recovery_str.umd_mode_list.Normal,//Normal -UMD required-
	&recovery_str.umd_mode_list.OE,//OE isofs legacy -NO UMD-
	&recovery_str.umd_mode_list.M33,//M33 driver -NO UMD-
	&recovery_str.umd_mode_list.NP9660,//Sony NP9660 -NO UMD-
	&recovery_str.umd_mode_list.ME,//ME driver
	&recovery_str.umd_mode_list.INFERNO,//Inferno driver

	&(menu_main[0].path),//Toggle USB
	&(menu_main[1].path),//Configuration
	&(recovery_str.RunProgram),//Run program at
	&(menu_main[3].path),//Advanced
	&(menu_main[4].path),//CPU Speed
	&(recovery_str.Plugins),//Plugins
	&(menu_main[6].path),//Registry hacks

	&(menu_advance[0].path),//Advanced configuration
	&(menu_advance[1].path),//Battery configuration
	&(menu_advance[6].path),//Format flash1 and reset settings
	&(menu_advance[7].path),//Format flash2

	&(battery_cnf[0].path),//Battery Serial
	&(battery_cnf[1].path),//Make Jigkick Battery
	&(battery_cnf[2].path),//Make AutoBoot Battery
	&(battery_cnf[3].path),//Make Normal Battery

	&(recovery_str.Exit),//Exit
	&(recovery_str.Back),//Back

	&(recovery_str.UsbEnabled),//USB enabled
	&(recovery_str.UsbDisabled),//USB disabled
	&(recovery_str.Formatting),//Formatting
	&(recovery_str.Currently),//currently

	&(menu_ad_cnf[0].path),//execute opn
	&(menu_ad_cnf[1].path),//execute pboot
	&(menu_ad_cnf[2].path),//High Memory Layout
	&(menu_ad_cnf[3].path),//Inferno ISO Cache
	&(menu_ad_cnf[4].path),//Inferno Cache Size
	&(menu_ad_cnf[5].path),//Inferno Cache Number
	&(menu_ad_cnf[6].path),//Inferno Cache Policy
	&(menu_ad_cnf[7].path),//XMB  plugins
	&(menu_ad_cnf[8].path),//GAME plugins
	&(menu_ad_cnf[9].path),//POPS plugins

	&(menu_config[0].path),//Skip Sony logo
	&(menu_config[1].path),//Skip Gameboot
	&(menu_config[2].path),//Hide corrupt icons
	&(menu_config[9].path),//Mount flash as read only
//	&(menu_config[2].path),//Game folder homebrew
	&(recovery_str.AutoRun),//Autorun program at
	&(menu_config[4].path),//UMD Mode
	&(menu_config[5].path),//Hide mac
	&(menu_config[6].path),//Fake region
	&(menu_config[7].path),//Use VshMenu
	&(menu_config[8].path),//XMB Usb Device
	&(recovery_str.UsbCharge),//Charge battery when USB cable plugged
	&(recovery_str.SlimColor),//Use Slim Color
	&(menu_config[11].path),//Use M33 network update
	&(menu_config[12].path),//Hide PIC0.PNG and PIC1.PNG in game menu
	&(menu_config[13].path),//Use version.txt
	&(menu_config[14].path),//NoDRM Engine
	&(menu_config[15].path),//Hide CFW Dirs from game
	&(menu_config[16].path),//Speed up MS access

	&(recovery_str.UmdDisc),//UMD Disc
	&(recovery_str.MemoryStick),//Memory Stick

	&(menu_cpu[0].path),//Speed in XMB
	&(menu_cpu[1].path),//Speed in UMD/ISO

	&(menu_registryhack[0].path),//Button assign
	&(menu_registryhack[1].path),//Activate WMA
	&(menu_registryhack[2].path),//Activate Flash Player

	&(recovery_str.registry_list.Button_o),//O is enter
	&(recovery_str.registry_list.Button_x),//X is enter
	&(recovery_str.registry_list.Wma_already),//WMA was already activated.
	&(recovery_str.registry_list.Wma_on),//Activating WMA...
	&(recovery_str.registry_list.FPlayer_already),//Flash player was already activated.
	&(recovery_str.registry_list.FPlayer_on),//Activating Flash Player...
	&(recovery_str.Exitting),//Exiting recovery

	&(recovery_str.MainMenu),//Main menu

//	&recovery_str.folder_name_list.Folder150,//1.50 Kernel
//	&recovery_str.folder_name_list.Folder3xx,//5.XX Kernel
	&(recovery_str.Reloading),//Reloading
	&(recovery_str.Writing),//Writing

	&(recovery_str.Always),//ALWAYS
	&(recovery_str.Never),//NEVER

	&(recovery_str.Unknown),//Unknown
	&(recovery_str.Wait),//Please wait
	&(recovery_str.Done),//Done
	&(recovery_str.Failed),//Failed
	&(recovery_str.Saving),//Saving
	&(recovery_str.UmdVideoPatch),//UmdVideo Patch
	
	&(recovery_str.Setting),//Setting

	&(menu_registryhack[3].path),//Backup NetConfig
	&(menu_registryhack[4].path),//Restore NetConfig
	&(recovery_str.HiddenStorage),//Hidden Storage
	&(menu_main[7].path),//Misc
	&(menu_misc[0].path),//Recovery Text Color
	&(menu_misc[1].path),//VSH Menu Back Color

	&recovery_str.color_list.green,// Color Green
	&recovery_str.color_list.red,// Color Red
	&recovery_str.color_list.blue,// Color Blue
	&recovery_str.color_list.grey,// Color Grey
	&recovery_str.color_list.pink,// Color Pink
	&recovery_str.color_list.purple,// Color Purple
	&recovery_str.color_list.turquoise,// Color Turquoise
	&recovery_str.color_list.orange,// Color Orange
	&recovery_str.color_list.yellow,// Color Yellow
	&recovery_str.color_list.white,// Color White
	&recovery_str.color_list.black,// Color Black

	NULL
};

static const char *text_list[] = {
	"ms0:/seplugins/%s_%s",
	"ef0:/seplugins/%s_%s",
	"flash1:/%s_%s",
};

char text_buff[0x1000];
void init_language_file()
{
	u32 param;
	getRegistryValue("/CONFIG/SYSTEM/XMB", "language", &param);

	const char *bridge = NULL;
	switch( param ) {
		case PSP_SYSTEMPARAM_LANGUAGE_JAPANESE:
			bridge = "ja";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_ENGLISH:
			bridge = "en";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_FRENCH:
			bridge = "fr";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_SPANISH:
			bridge = "es";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_GERMAN:
			bridge = "de";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_ITALIAN:
			bridge = "it";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_DUTCH:
			bridge = "nl";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_PORTUGUESE:
			bridge = "pt";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN:
			bridge = "ru";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_KOREAN:
			bridge = "ko";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_CHINESE_TRADITIONAL:
			bridge = "ch2";
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_CHINESE_SIMPLIFIED:
			bridge = "ch1";
			break;
		default:
			return;
			break;
	}

	int i;
	char path_buff[256];

	for(i=0;i<(sizeof(text_list)/sizeof(char*));i++)
	{
		my_sprintf( path_buff, text_list[i] , bridge, "ftable.bin" );
		if( myDebugLoadExtraFont( path_buff ) >= 0)
		{
			break;
		}
	}

	memset( text_buff, 0, 0x1000 );
	int size = 0;
	for(i=0;i<(sizeof(text_list)/sizeof(char*));i++)
	{
		my_sprintf( path_buff, text_list[i] , bridge, "recovery.txt" );
		size = ReadFile( path_buff , text_buff , 0x1000 );
		 if( size > 0 ) break;
	}

	if( size > 0 )
	{

		char *token;
		char *saveptr1 = NULL;
		char *** write_pointer = TextAddrList;

		for(token = my_strtok_r( text_buff , "\r\n", &saveptr1);
			token != NULL;
			token = my_strtok_r(NULL, "\r\n", &saveptr1)) 	
		{
			**write_pointer = token;
			write_pointer ++;
	
			if( ! *write_pointer )
				break;
		}
	}

}
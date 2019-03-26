/*------------------------------------------------------

------------------------------------------------------*/

#ifndef _CFW_RECOVERY_PACK_
#define _CFW_RECOVERY_PACK_


#include "main.h"
#include "action.h"

//u8 kari=0;

static const Menu_pack menu_ad_cnf[] =
{
//	{"Plain modules in UMD/ISO",2,0,0,0,0,&config.umdactivatedplaincheck},
//	{"Excute BOOT.BIN in UMD/ISO", TMENU_SWITCH ,&config.executebootbin},
	{ &recovery_str.advanced_config_list.opnssmp , TMENU_SWITCH2 ,&config.freeumdregion},

	{ &recovery_str.Xmb , TMENU_SWITCH2 ,&config.plugvsh },
	{ &recovery_str.Game, TMENU_SWITCH2 ,&config.pluggame },
	{ &recovery_str.Pops, TMENU_SWITCH2 ,&config.plugpop },
	{NULL,0,"Advanced Config"}

};

static const Menu_pack battery_cnf[] =
{
	{ &recovery_str.battery_list.Serial		, TMENU_FUNCTION_VALUE , backup_eeprom },
	{ &recovery_str.battery_list.Jigkick	, TMENU_FUNCTION, make_JigkickBattery },
	{ &recovery_str.battery_list.Autoboot	, TMENU_FUNCTION, make_AutobootBattery },
	{ &recovery_str.battery_list.Normal		, TMENU_FUNCTION, make_NormalBattery },
	{NULL,0,"Battery Config"}

};

static const Menu_pack menu_advance[] =
{
	{ &recovery_str.advanced_config_list.Title	, TMENU_SUB_MENU, menu_ad_cnf },
	{ &recovery_str.battery_list.Title			, TMENU_SUB_MENU, battery_cnf },
	{ &recovery_str.UsbToggle	, TMENU_FUNCTION, toggle_usb_flash0 },
	{ &recovery_str.UsbToggle	, TMENU_FUNCTION, toggle_usb_flash1 },
	{ &recovery_str.UsbToggle	, TMENU_FUNCTION, toggle_usb_flash2 },
	{ &recovery_str.UsbToggle	, TMENU_FUNCTION, toggle_usb_flash3 },
	{ &recovery_str.advanced_list.Format , TMENU_FUNCTION , format_flash1 },
	{ &recovery_str.advanced_list.Format2, TMENU_FUNCTION , format_flash2 },
	{NULL,0,"Advanced"}

};

static const Menu_pack menu_registryhack[] =
{
	{ &recovery_str.registry_list.Button , TMENU_FUNCTION_VALUE ,swap_key},
	{ &recovery_str.registry_list.Wma	, TMENU_FUNCTION ,activate_wma},
	{ &recovery_str.registry_list.FPlayer,TMENU_FUNCTION ,activate_flash},
	{NULL,0,"Registry hack"}
};


static Menu_pack menu_cpu[] =
{
	{ &recovery_str.Xmb		,TMENU_FUNCTION_VALUE , vsh_clock},
	{ &recovery_str.Game	,TMENU_FUNCTION_VALUE , game_clock},
	{NULL,0,"CPU Speed"}
};

/*
static const u8 menu_cpu_s = 3;


static const Menu_pack menu_main[] =
{
	{"Toggle USB"		,0,0,NULL,exiting},
	{"Configuration"	,1,1,menu_config,0,0,&menu_config_s},
	{"Run program at /PSP/GAME/RECOVERY/EBOOT.PBP"	,0,0,0,run_game},
	{"Advanced"			,1,2,menu_advance,0,0,&menu_advance_s},
	{"CPU Speed"		,1,4,menu_cpu,0,0,&menu_cpu_s},
	{"Plugins"			,0,0,0,menu_plugin,0,5},
	{"Registry hacks"	,1,6,menu_registryhack,0,0,&menu_registryhack_s},
	{ NULL }

};
static const u8 menu_main_s = 7;
*/

static Menu_pack menu_config[] =
{
	{ &recovery_str.configration_list.SkipLogo		, TMENU_SWITCH ,&config.skiplogo},
	{ &recovery_str.configration_list.HideCurrupt	, TMENU_SWITCH ,&config.hidecorrupt},
	{ &recovery_str.configration_list.GameFolder	, TMENU_FUNCTION_VALUE , folder_change },
	{ &recovery_str.configration_list.AutRunProgram	, TMENU_SWITCH ,&config.startupprog},
	{ &recovery_str.configration_list.UmdMode		, TMENU_FUNCTION_VALUE , umdmode_change },
	{ &recovery_str.configration_list.HideMac		, TMENU_SWITCH ,&config.umdactivatedplaincheck},
	{ &recovery_str.configration_list.FakeRegion	, TMENU_FUNCTION_VALUE , region_change },
	{ &recovery_str.configration_list.VshMenu		, TMENU_SWITCH2 , &config.novshmenu },
	{ &recovery_str.configration_list.UsbDevice		, TMENU_FUNCTION_VALUE , usb_change },
	{ &recovery_str.configration_list.UsbCharge		, TMENU_SWITCH,&config.usbcharge },
	{ &recovery_str.configration_list.NetworkUpdate	, TMENU_SWITCH2 ,&config.netupdate },
	{ &recovery_str.configration_list.HidePng		, TMENU_SWITCH , &config.hidepng },
	{ &recovery_str.configration_list.versiontxt	, TMENU_SWITCH , &config.versiontxt },
	{ &recovery_str.configration_list.speedup_ms	, TMENU_FUNCTION_VALUE , ms_speed_change },
	{ NULL , 0 ,"Configuration"}
};

//char *test = "Plugins ->";
static Menu_pack menu_main[] =
{
	{ &recovery_str.UsbToggle				, TMENU_FUNCTION , toggle_usb_ms },
	{ &recovery_str.configration_list.Title	, TMENU_SUB_MENU , menu_config},
	{ &recovery_str.RunProgram				, TMENU_FUNCTION ,run_game},
	{ &recovery_str.advanced_list.Title		, TMENU_SUB_MENU , menu_advance },
	{ &recovery_str.cpu_speed_list.Title	, TMENU_SUB_MENU , menu_cpu },
	{ &recovery_str.Plugin					, TMENU_FUNCTION , plugin_manager },
	{ &recovery_str.registry_list.Title		, TMENU_SUB_MENU , menu_registryhack },
	{ NULL ,0,"Main menu"}
};



/*
static const char * menu_toggleusb[] =
{
	"ms0:",
	"flash0:",
	"flash1:"
};

static const u8 menu_toggleusb_s = 3;

*/
#endif

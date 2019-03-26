/*------------------------------------------------------

------------------------------------------------------*/

#ifndef _CFW_RECOVERY_PACK_
#define _CFW_RECOVERY_PACK_


#include "main.h"
#include "action.h"

extern int psp_model;

//u8 kari=0;

const Menu_pack menu_ad_cnf[] =
{
//	{"Plain modules in UMD/ISO",2,0,0,0,0,&config.umdactivatedplaincheck},
	{"Execute OPNSSMP.bin in ISO", TMENU_SWITCH2 ,&config.executeopnssmp},
//	{"Execute BOOT.BIN in UMD/ISO", TMENU_SWITCH ,&config.executebootbin},
	{"Execute PBOOT.PBP with ISO", TMENU_SWITCH ,&config.execute_pboot},
//	{"Use syscall patch", TMENU_SWITCH2 ,&config.umdactivatedplaincheck},
	{"Unlock Extra Memory", TMENU_SWITCH, &config.high_memory},
	{"Inferno ISO Cache", TMENU_SWITCH ,&config.iso_cache},
	{"Inferno Cache Size", TMENU_FUNCTION_VALUE ,iso_cache_total_size},
	{"Inferno Cache Number", TMENU_FUNCTION_VALUE ,iso_cache_num},
	{"Inferno Cache Policy", TMENU_FUNCTION_VALUE ,iso_cache_policy},
	{"XMB  Plugins", TMENU_SWITCH2 ,&config.plugvsh },
	{"GAME Plugins", TMENU_SWITCH2 ,&config.pluggame },
	{"POPS Plugins", TMENU_SWITCH2 ,&config.plugpop },
	{NULL,0, "" }
};

const Menu_pack battery_cnf[] =
{
	{"Battery Serial", TMENU_FUNCTION_VALUE , backup_eeprom },
	{"Make Jigkick Battery"	, TMENU_FUNCTION, make_JigkickBattery },
	{"Make AutoBoot Battery"	, TMENU_FUNCTION, make_AutobootBattery },
	{"Make Normal Battery"	, TMENU_FUNCTION, make_NormalBattery },
	{NULL,0,""}
};

const Menu_pack menu_advance[] =
{
	{"Advanced Config"		, TMENU_SUB_MENU, (void *)menu_ad_cnf },
	{"Battery Config"		, TMENU_SUB_MENU, (void *)battery_cnf },
	{ ""/*"Toggle USB (flash0)"*/	, TMENU_FUNCTION_DISPLAY, toggle_usb_flash0 },
	{ ""/*"Toggle USB (flash1)"*/	, TMENU_FUNCTION_DISPLAY, toggle_usb_flash1 },
	{ ""/*"Toggle USB (flash2)"*/	, TMENU_FUNCTION_DISPLAY, toggle_usb_flash2 },
	{ ""/*"Toggle USB (flash3)"*/	, TMENU_FUNCTION_DISPLAY, toggle_usb_flash3 },
	{"Format flash1 and reset settings", TMENU_FUNCTION , format_flash1 },
	{"Format flash2", TMENU_FUNCTION , format_flash2 },
	{NULL,0,""}
};

const Menu_pack menu_registryhack[] =
{
	{"Button assign"		, TMENU_FUNCTION_VALUE ,swap_key},
	{"Activate WMA"			, TMENU_FUNCTION ,activate_wma},
	{"Activate Flash Player", TMENU_FUNCTION ,activate_flash},
	{"Backup NetConfig"		, TMENU_FUNCTION ,netcnf_backup},
	{"Restore NetConfig"	, TMENU_FUNCTION ,netcnf_restore},
	{NULL,0,""}
};


Menu_pack menu_cpu[] =
{
	{"Speed in VSH ",TMENU_FUNCTION_VALUE , vsh_clock},
	{"Speed in GAME",TMENU_FUNCTION_VALUE , game_clock},
	{NULL,0,""}
};

Menu_pack menu_misc[] =
{
	{"Recovery Text color",TMENU_FUNCTION_VALUE , color_change},
	{"VSH Menu Back color",TMENU_FUNCTION_VALUE , vsh_color_change},
	{NULL,0,""}
};

Menu_pack menu_config[] =
{
	{"Skip Sony logo"		, TMENU_SWITCH ,&config.skiplogo},
	{"Skip Gameboot"		, TMENU_SWITCH, &config.skipgameboot},
	{"Hide corrupt icons"	, TMENU_SWITCH ,&config.hidecorrupt},
//	{"GAME folder homebrew"	, TMENU_FUNCTION_VALUE , folder_change },
	{""/*"Autorun program at /PSP/GAME/BOOT/EBOOT.PBP"*/, TMENU_FUNCTION_VALUE_DISPLAY , autorun_umdvideo_change },
	{"UMD Mode"				, TMENU_FUNCTION_VALUE , umdmode_change },
	{"Hide MAC Address", TMENU_SWITCH ,&config.umdactivatedplaincheck},
	{"Fake region"			, TMENU_FUNCTION_VALUE , region_change },
	{"Use VshMenu"			, TMENU_SWITCH2 , &config.novshmenu },
	{"XMB Usb Device"		, TMENU_FUNCTION_VALUE , usb_change },
	{"Protect Flash in Usb Mount"	, TMENU_SWITCH2 , &config.usbprotect },
	{""/*"Battery charge from USB"*/,TMENU_FUNCTION_VALUE_DISPLAY, usbcharge_slimcolor_change },
	{"Use network update", TMENU_SWITCH2 ,&config.netupdate },
	{"Hide PIC0 & PIC1.PNG in XMB", TMENU_SWITCH , &config.hidepng },
	{"Use version.txt", TMENU_SWITCH , &config.versiontxt },
	{"NoDRM Engine", TMENU_SWITCH2 ,&config.usenodrm},
	{"Hide CFW Dirs from game", TMENU_SWITCH2 ,&config.hidecfwdirs},	
	{"Speed up MS access", TMENU_FUNCTION_VALUE , ms_speed_change },
	{ NULL , 0 ,""}
};

Menu_pack menu_main[] =
{
	{"Toggle USB"		, TMENU_FUNCTION, toggle_usb_ms },
	{"Configuration"	, TMENU_SUB_MENU, menu_config},
	{""/*"Run program at"*/	, TMENU_FUNCTION_DISPLAY, run_game},
	{"Advanced"	, TMENU_SUB_MENU, (void *)menu_advance },
	{"CPU Speed"	, TMENU_SUB_MENU, menu_cpu },
	{""/*"Plugins ->"*/	, TMENU_FUNCTION_DISPLAY, plugin_manager },
	{"Registry hacks", TMENU_SUB_MENU, (void *)menu_registryhack },
	{"Misc", TMENU_SUB_MENU, (void *)menu_misc },
	{ NULL , 0, ""}
};

/*
static const char * menu_toggleusb[] =
{
	"ms0:",
	"flash0:",
	"flash1:"
};

*/
#endif

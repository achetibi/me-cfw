#ifndef _CFW_RECOVERY_PACK_
#define _CFW_RECOVERY_PACK_

#include "main.h"
#include "action.h"

extern int fw_install();
extern int dump_nand_start();
extern int restore_nand_start();

Menu_pack menu_install_confirm[] =
{
	{"Continue"		, TMENU_FUNCTION_RET , fw_install },
	{"Cancel"		, TMENU_FUNCTION , NULL },
	{ NULL ,0,""}
};

Menu_pack menu_restore_confirm[] =
{
	{"Continue"		, TMENU_FUNCTION_RET , restore_nand_start },
	{"Cancel"		, TMENU_FUNCTION , NULL },
	{ NULL ,0,""}
};

Menu_pack menu_nand[] =
{
	{"Dump NAND"		, TMENU_FUNCTION_RET , dump_nand_start },
	{"Restore NAND"		, TMENU_SUB_MENU , menu_restore_confirm },
	{"Back"		, TMENU_FUNCTION , NULL },
	{ NULL ,0,""}
};

#if _PSP_FW_VERSION == 660
Menu_pack menu_main[] =
{
    {"Install 6.60 ME"		, TMENU_SUB_MENU , menu_install_confirm },
    {"Install 6.60 OFW"		, TMENU_SUB_MENU , menu_install_confirm},
    {"NAND Operations"		, TMENU_SUB_MENU , menu_nand },
    {"Hardware Info"		, TMENU_FUNCTION , HardwareInfo },
    {"Test ME"				, TMENU_FUNCTION , TestME },
    {"Shutdown"				, TMENU_FUNCTION , Shutdown },
    {"Reboot Device"		, TMENU_FUNCTION , Reboot },
    { NULL ,0,""}
};
#elif _PSP_FW_VERSION == 661
Menu_pack menu_main[] =
{
    {"Install 6.61 ME"		, TMENU_SUB_MENU , menu_install_confirm },
    {"Install 6.61 OFW"		, TMENU_SUB_MENU , menu_install_confirm},
    {"NAND Operations"		, TMENU_SUB_MENU , menu_nand },
    {"Hardware Info"		, TMENU_FUNCTION , HardwareInfo },
    {"Test ME"				, TMENU_FUNCTION , TestME },
    {"Shutdown"				, TMENU_FUNCTION , Shutdown },
    {"Reboot Device"		, TMENU_FUNCTION , Reboot },
    { NULL ,0,""}
};
#else
#error menu_struct.h
#endif

#endif

#ifndef _CFW_RECOVERY_PACK_
#define _CFW_RECOVERY_PACK_

#include "main.h"
#include "action.h"

void install_iplloader();
void unpack();

#if _PSP_FW_VERSION == 660
Menu_pack menu_main[] =
{
	{"Install iplloader on MS boot sector",		TMENU_FUNCTION , install_iplloader },
	{"Install 6.60ME (01g + 02g model)",		TMENU_FUNCTION , unpack },
	{"Exit",									TMENU_FUNCTION , Exit },
	{ NULL ,0,""}
};

#elif _PSP_FW_VERSION == 661
Menu_pack menu_main[] =
{
	{"Install iplloader on MS boot sector",		TMENU_FUNCTION , install_iplloader },
	{"Install 6.61ME (01g + 02g model)",		TMENU_FUNCTION , unpack },
	{"Exit",									TMENU_FUNCTION , Exit },
	{ NULL ,0,""}
};

#else
#error devkit_ver
#endif

#endif

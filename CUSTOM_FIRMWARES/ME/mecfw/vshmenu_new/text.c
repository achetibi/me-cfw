
#include <pspsdk.h>
#include <psputility_sysparam.h>

#include "text.h"

#include "common.h"

MenuText vshmenu_str = {
	.Enabled		= "Enabled",
	.Disabled		= "Disabled",
	.Default		= "Default",
	.None			= "NONE",
	.Back			= "Back",

	.UmdDisc		= "UMD Disc",
	.MemoryStick	= "Memory Stick",
	.HiddenStorage	= "Hidden Storage",

	.umd_mode_list	= {
		.Normal		= "Normal",
		.OE			= "OE isofs",
		.M33		= "M33 driver",
		.NP9660		= "Sony NP9660",
		.ME			= "ME driver",
		.INFERNO	= "Inferno",
	},
	
	.color_list	= {
		.red	= "RED",
		.green	= "GREEN",
		.blue	= "BLUE",
		.pink	= "PINK",
		.purple	= "PURPLE",
		.orange	= "ORANGE",
		.yellow	= "YELLOW",
		.black	= "BLACK",
		.white	= "WHITE",
		.black	= "BLACK",
	},
};

extern Menu_pack main_menu[];

char **TextAddrList[] = {
	&(vshmenu_str.Disabled),//Disabled
	&(vshmenu_str.Enabled),//Enabled
	&(vshmenu_str.Default),//Default
	&(vshmenu_str.None),//None	
	&(vshmenu_str.Back),//Back

	&(vshmenu_str.UmdDisc),//UMD Disc
	&(vshmenu_str.MemoryStick),//Memory Stick
	&(vshmenu_str.HiddenStorage),//Hidden Storage

	&(vshmenu_str.umd_mode_list.Normal),//Normal
	&(vshmenu_str.umd_mode_list.OE),//OE isofs legacy
	&(vshmenu_str.umd_mode_list.M33),//M33 driver
	&(vshmenu_str.umd_mode_list.NP9660),//Sony NP9660
	&(vshmenu_str.umd_mode_list.ME),//ME driver
	&(vshmenu_str.umd_mode_list.INFERNO),//Inferno driver
	
	&(vshmenu_str.color_list.red),//RED
	&(vshmenu_str.color_list.green),//GREEN
	&(vshmenu_str.color_list.blue),//BLUE
	&(vshmenu_str.color_list.pink),//PINK
	&(vshmenu_str.color_list.purple),//PURPLE
	&(vshmenu_str.color_list.orange),//ORANGE
	&(vshmenu_str.color_list.yellow),//YELLOW
	&(vshmenu_str.color_list.black),//BLACK
	&(vshmenu_str.color_list.white),//WHITE

	&(main_menu[0].path),//CPU CLOCK XMB
	&(main_menu[1].path),//CPU CLOCK GAME
	&(main_menu[2].path),//USB DEVICE
	&(main_menu[3].path),//UMD ISO MODE
	&(main_menu[4].path),//ISO VIDEO MOUNT
	&(main_menu[5].path),//BACK COLOR
	&(main_menu[6].path),//XMB  PLUGINS
	&(main_menu[7].path),//GAME PLUGINS
	&(main_menu[8].path),//POPS PLUGINS
	&(main_menu[9].path),//PLUGINS MANAGER
	&(main_menu[10].path),//ENTER RECOVERY
	&(main_menu[11].path),//SHUTDOWN DEVICE
	&(main_menu[12].path),//SUSPEND DEVICE
	&(main_menu[13].path),//RESET DEVICE
	&(main_menu[14].path),//RESET VSH
	&(main_menu[15].path),//EXIT
	NULL
};

static int ReadFile(const char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);
	
	sceIoClose(fd);
	return read;
}

static const char *font_list[] = {
	"ms0:/seplugins/%s_%s",
	"ef0:/seplugins/%s_%s",
//	"flash1:/%s_%s"
};

char text_buff[0x1000];

void init_font_table()
{
	int system_language;
	if(sceUtilityGetSystemParamInt( PSP_SYSTEMPARAM_ID_INT_LANGUAGE , &system_language ) < 0)
	{
		return;
	}

	const char *bridge = NULL;
	switch( system_language ) {
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

	for(i=0;i<(sizeof(font_list)/sizeof(char*));i++)
	{
		scePaf_sprintf( path_buff, font_list[i] , bridge, "ftable.bin" );
		if( load_font( path_buff ) >= 0 )
		{
			break;
		}
	}

	memset( text_buff, 0, 0x1000 );
	int size = 0;
	for(i=0;i<(sizeof(font_list)/sizeof(char*));i++)
	{
		scePaf_sprintf( path_buff, font_list[i] , bridge, "vshmenu.txt" );
		size = ReadFile( path_buff , text_buff , 0x1000 );
		if( size > 0 )
		{
			char *token;
			char *saveptr1 = NULL;
			char *** write_pointer = TextAddrList;

			for(token = scePaf_strtok_r( text_buff , "\r\n", &saveptr1);
				token != NULL;
				token = scePaf_strtok_r(NULL, "\r\n", &saveptr1))
			{
				**write_pointer = token;
				write_pointer ++;
	
				if( ! *write_pointer )
					break;
			}

			break;
		}
	}
}
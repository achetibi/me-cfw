#include <pspsdk.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <systemctrl.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <vlf.h>

#include "main.h"
#include "trans.h"
#include "common.h"
#include "install.h"
#include "battery.h"
#include "vlfutils.h"
#include "../kpspident/main.h"

PSP_MODULE_INFO("VResurrection_Manager", 0x200, 2, 5);
PSP_MAIN_THREAD_ATTR(0);

#define VLF_ITEMS 20
#define SMALL_BUFFER_SIZE 2100000
#define STRING (char *)g_messages
#define NELEMS(a) sizeof(a) / sizeof(a[0])

extern const char **g_messages;
extern int showenter_prev;

u8 *big_buffer, *sm_buffer1, *sm_buffer2;
int BIG_BUFFER_SIZE, mode = 0, selitem;

char *ebootpath;
char file_path[128];

/*****************************************************************************/

int OnBackToMainMenu(int enter)
{
	if(!enter)
	{
		if(mode == 0x00000001){mode = 0x00000000;}
		else if(mode == 0x00000002){mode = 0x00000000;}
		else if(mode == 0x00000101){mode = 0x00000001;}
		else if(mode == 0x00000201){mode = 0x00000001;}
		else if(mode == 0x00000102){mode = 0x00000002;}
		else if(mode == 0x00000202){mode = 0x00000002;}
		else if(mode == 0x00000302){mode = 0x00000002;}
		else{mode = 0x00000000;}

		if(mode == 0x00000000)
		{
			ResetScreen(1, 0, selitem);
		}
		else
		{
			ResetScreen(1, 1, selitem);
		}

		AddBackgroundHandler(PSP_CTRL_SELECT, SetBackground, NULL);
		vlfGuiSetRectangleFade(0, VLF_TITLEBAR_HEIGHT, 480, 272 - VLF_TITLEBAR_HEIGHT, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0, NULL, NULL, 0);
	}

	return VLF_EV_RET_NOTHING;
}

int OnMainMenuSelect(int sel)
{
	ResetScreen(0, 0, 0);
	selitem = sel;

	// Main Menu Select
	if(mode == 0x00000000)
	{
		switch (sel)
		{
			case 0: // Show Install Options
				mode = 0x00000001;
				ResetScreen(1, 1, 0);
				break;

			case 1: // Show Battery Options
				mode = 0x00000002;
				ResetScreen(1, 1, 0);
				break;

			case 2: // Exit to XMB
				AddWaitIcon();
				sctrlKernelExitVSH(NULL);
				break;
		}
	}

	else if(mode == 0x00000001)
	{
		switch (sel)
		{
			case 0: // Install DC8
				mode = 0x00000101;
				Install(0, 0x01F4);
				break;

			case 1: // Install DC9
				mode = 0x00000201;
				Install(1, 0x0294);
				break;
		}
	}
	
	else if(mode == 0x00000002)
	{
		switch (sel)
		{
			case 0: // Make Normal Battery
				mode = 0x00000102;
				SetBatterySerial(0x5241, 0x4E44);
				break;

			case 1: // Make Pandora Battery
				mode = 0x00000202;
				SetBatterySerial(0xFFFF, 0xFFFF);
				break;
				
			case 2: // Make Autoboot Battery
				mode = 0x00000302;
				SetBatterySerial(0x0000, 0x0000);
				break;
		}
	}

	return 0;
}

void MainMenu(int sel)
{
	int i;

	if(mode == 0x00000000)
	{
		char *items[] =
		{
			STRING[STR_INSTALL],
			STRING[STR_BATTERY],
			STRING[STR_EXIT],
		};
		
		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], unicode);
		}

		SetTitle("update_plugin", "tex_update_icon", STRING[STR_DESPERTAR_DEL_CEMENTERIO], "");
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	
	else if(mode == 0x00000001)
	{
		
		char *items[] = { "8", "9" };
		char *items_tmp[NELEMS(items)];
		
		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			sprintf(unicode, STRING[STR_DESPERTAR_DEL_CEMENTERIO], items[i]);
			items_tmp[i] = unicode;
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], items_tmp[i]);
		}

		selitem = 0;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_SELECT_YOUR_VERSION]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	
	else if(mode == 0x00000002)
	{
		
		char *items[] =
		{
			STRING[STR_CONVERT_TO_NORMAL],
			STRING[STR_CONVERT_TO_PANDORA],
			STRING[STR_CONVERT_TO_AUTOBOOT],
		};
		
		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], unicode);
		}

		selitem = 1;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_BATTERY_OPERATIONS]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	
	if (!showenter_prev)
	{
		vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, OnBackToMainMenu);
		showenter_prev = 1;
	}

	vlfGuiSetRectangleFade(0, VLF_TITLEBAR_HEIGHT, 480, 272 - VLF_TITLEBAR_HEIGHT, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0, NULL, NULL, 0);
}

/***************************************************************************************************/

int app_main(SceSize args, void *argp)
{
	sm_buffer1 = malloc64(SMALL_BUFFER_SIZE);
	sm_buffer2 = malloc64(SMALL_BUFFER_SIZE);
	BIG_BUFFER_SIZE = (25*1024*1024);
	big_buffer = malloc64(BIG_BUFFER_SIZE);

	if (!big_buffer)
	{
		BIG_BUFFER_SIZE = (10*1024*1024);
		big_buffer = malloc64(BIG_BUFFER_SIZE);
	}

	if (!sm_buffer1 || !sm_buffer2 || !big_buffer)
	{
		asm("break\n");
		while (1);
	}

	vpl_init();
	select_language();
	vlfGuiSystemSetup(1, 1, 0);
	SetBackground(NULL);

	if(sceKernelDevkitVersion() <= 0x03070110)
	{
		ErrorReturn(0, 1, STRING[STR_YOUR_CURRENT_FIRMWARE_IS_UNSUPPORTED]);
		sceKernelExitGame();
	}

	ResetScreen(1, 0, 0);
	AddBackgroundHandler(PSP_CTRL_SELECT, SetBackground, NULL);

	while(1)
	{
		vlfGuiDrawFrame();
	}

   	return 0;
}
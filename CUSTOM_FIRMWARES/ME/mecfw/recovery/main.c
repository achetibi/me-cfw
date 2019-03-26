/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - Basic ELF template
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * $Id: main.c 1888 2006-05-01 08:47:04Z tyranid $
 * $HeadURL$
 */
#include <pspkernel.h>
//#include <pspdebug.h>
#include <pspctrl.h>
#include <psppower.h>
#include <psploadexec_kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <pspvshbridge.h>
#include "systemctrl_me.h"
#include "text.h"
#include "action.h"

/* Define the module info section */
PSP_MODULE_INFO("Recovery Menu", 0x800, 1, 0);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VSH);//| PSP_THREAD_ATTR_VFPU 

SEConfig config;
SEConfig backup_config;

VshConfig config_mini;
VshConfig config_mini_backup;

int psp_model = 0;
u32 PSP_CTRL_BACK = PSP_CTRL_CIRCLE; //default
u32 PSP_CTRL_ENTER = PSP_CTRL_CROSS; //default

#include "menu_struct.h"

char *create_eeprom_str();

u32 ctrlRead()
{
	static u32 cur_buttons = 0xFFFFFFFF;
	SceCtrlData ctl;
	u32 buttons;
	u32 button_on;

	sceCtrlPeekBufferPositive(&ctl, 1);

	buttons     = ctl.Buttons;
	button_on   = ~cur_buttons & buttons;
	cur_buttons = buttons;

	return button_on;
}

static void load_config()
{
	sctrlSEGetConfig(&config);
	memcpy(&backup_config, &config, sizeof(SEConfig));

	sctrlSEGetMiniConfig(&config_mini);
	memcpy(&config_mini_backup, &config_mini, sizeof(VshConfig));
}

void save_config()
{
	if(memcmp(&backup_config, &config, sizeof(SEConfig)) != 0)
	{
		sctrlSESetConfig(&config);
	}

	if(memcmp(&config_mini_backup, &config_mini, sizeof(VshConfig)) != 0)
	{
		sctrlSESetMiniConfig(&config_mini);
	}
}

u32 wakeup_timer()
{
	scePowerTick(0);
	return 10*1000*1000;
}

#include "kernel/bridge.h"

int LoadStartModuleBuffer(void *buff, int size, void* flag)
{
	SceUID mod = vshKernelLoadModuleBufferVSH(size, buff, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, (flag == NULL) ? 0 : 4 , flag, NULL, NULL);
}

int main()
{
	load_config();	
	psp_model = vshKernelGetModel();

	u32 enter;
	
	if (getRegistryValue("/CONFIG/SYSTEM/XMB", "button_assign", &enter))
	{
		if(enter == 0)
		{
			PSP_CTRL_BACK = PSP_CTRL_CROSS;
			PSP_CTRL_ENTER = PSP_CTRL_CIRCLE;
		}
		else
		{
			PSP_CTRL_BACK = PSP_CTRL_CIRCLE;
			PSP_CTRL_ENTER = PSP_CTRL_CROSS;
		}
	}

	color_change(FUNC_CHANGE_VALUE, config_mini.recovery_color);

	if (LoadStartModuleBuffer(bridge, size_bridge, NULL) >= 0)
	{
		create_eeprom_str();
	}

	SceUID id = sceKernelCreateVTimer("", NULL);
	if(id >= 0)
	{
		sceKernelStartVTimer(id);	
		sceKernelSetVTimerHandlerWide(id, 2*1000*1000, wakeup_timer, NULL);
	}

	myDebugScreenInit();
	draw_init();

	init_language_file();

	DrawMenu(menu_main, 0, recovery_str.MainMenu);
	send_msg(recovery_str.Exitting);

	config_mini.recovery_color = (u32)color_change(FUNC_GET_INT, 0);

	disable_usb();
	save_config();

//	sceKernelExitGame();
	vshKernelExitVSHVSH(NULL);

	return 0;
}

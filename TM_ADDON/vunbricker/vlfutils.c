#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsuspend.h>
#include <psppower.h>
#include <pspctrl.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <vlf.h>

#include "dcman.h"
#include "trans.h"
#include "common.h"
#include "vlfutils.h"

#include "../kpspident/main.h"
#include "../includes/wallpapers/wallpapers.h"

extern const char **g_messages;
extern u8 *sm_buffer2;

int new_x, new_y;
int st_align = 0;
int custom_bg = 0;
int last_time = 0;
int page_control = 0;
int showback_prev = 0;
int showenter_prev = 0;
int last_percentage = 0;

char pg_text[32];
char st_text[256];

#define VLF_ITEMS 20
VlfText vlf_texts[VLF_ITEMS];
VlfSpin vlf_spins[VLF_ITEMS];

VlfText status = NULL;
VlfText title_text = NULL;
VlfText vlf_progresstext = NULL;
VlfPicture title_picture = NULL;
VlfProgressBar vlf_progressbar = NULL;
VlfShadowedPicture waiticon = NULL;

int OnErrorReturn(void *param)
{
	int pg_ctrl = *(int *)param;
	page_control = pg_ctrl;
	dcSetCancelMode(0);

	vlfGuiMessageDialog(st_text, VLF_MD_TYPE_ERROR | VLF_MD_BUTTONS_NONE);
	OnBackToMainMenu(0);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

void ErrorReturn(int handler, int pg_ctrl, char *fmt, ...)
{
	va_list list;

	va_start(list, fmt);
	vsprintf(st_text, fmt, list);
	va_end(list);

	if (handler)
	{
		vlfGuiAddEventHandler(0, -1, OnErrorReturn, &pg_ctrl);
		sceKernelExitDeleteThread(0);
	}
	else
	{
		dcSetCancelMode(0);
		page_control = pg_ctrl;
		sceKernelVolatileMemUnlock(0);
		vlfGuiMessageDialog(st_text, VLF_MD_TYPE_ERROR|VLF_MD_BUTTONS_NONE);
		OnBackToMainMenu(0);
	}
}

int DoStatusUpdate(void *param)
{
	vlfGuiSetText(status, st_text);
	vlfGuiSetTextXY(status, new_x, new_y);
	vlfGuiSetTextAlignment(status, st_align);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

void SetStatus(int x, int y, int alignment, char *fmt, ...)
{
	va_list list;
	char msg[256];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	if (status == NULL)
	{
		strcpy(st_text, msg);
		status = vlfGuiAddUnicodeText(x, y, st_text);
		vlfGuiSetTextAlignment(status, alignment);
	}
	else
	{
		new_x = x;
		new_y = y;
		st_align = alignment;
		UTF82Unicode(msg, st_text);
		vlfGuiAddEventHandler(0, -2, DoStatusUpdate, NULL);
	}
}

int DoProgressUpdate(void *param)
{
	vlfGuiProgressBarSetProgress(vlf_progressbar, last_percentage);
	vlfGuiSetText(vlf_progresstext, pg_text);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

void SetProgress(int percentage, int force)
{
	int st =  sceKernelGetSystemTimeLow();
	
	if (force || (percentage > last_percentage && st >= (last_time+520000)))
	{
		sprintf(pg_text, "%d%%", percentage);
		last_percentage = percentage;
		last_time = st;
		vlfGuiAddEventHandler(0, -2, DoProgressUpdate, NULL);
	}
}

void SetGenericProgress(int value, int max, int force)
{
	u32 prog = ((100 * value) / max);
	SetProgress(prog, force);
}

void SetInstallProgress(int value, int max, int force, int ofw)
{
	u32 prog = (((ofw ? 95 : 93) * value) / max) + 4;
	SetProgress(prog, force);
}

void InitProgress(int pb, int pt, int st, int x, int y, int alignement, char *fmt, ...)
{
	char msg[256];	
	va_list list;

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);
	
	last_time = 0;
	last_percentage = 0;

	if(status != NULL)
	{
		vlfGuiRemoveText(status);
		status = NULL;
	}

	if(vlf_progresstext != NULL)
	{
		vlfGuiRemoveText(vlf_progresstext);
		vlf_progresstext = NULL;
	}

	if(vlf_progressbar != NULL)
	{
		vlfGuiRemoveProgressBar(vlf_progressbar);
		vlf_progressbar = NULL;
	}
	
	if(st)
	{
		status = vlfGuiAddUnicodeText(x, y, msg);
		vlfGuiSetTextAlignment(status, alignement);
	}
	
	if(pb)
	{
		vlf_progressbar = vlfGuiAddProgressBar(136);
	}
	
	if(pt)
	{
		vlf_progresstext = vlfGuiAddText(240, 148, "0%");
		vlfGuiSetTextAlignment(vlf_progresstext, VLF_ALIGNMENT_CENTER);
	}
}

void ResetScreen(int showmenu, int showback, int sel)
{
	int i;

	for(i = 0; i < VLF_ITEMS; i++)
	{
		if(vlf_texts[i] != NULL)
		{
			vlfGuiRemoveText(vlf_texts[i]);
			vlf_texts[i] = NULL;
		}
		
		if(vlf_spins[i] != NULL)
		{
			vlfGuiRemoveSpinControl(vlf_spins[i]);
			vlf_spins[i] = NULL;
		}
	}

	if(vlf_progressbar != NULL)
	{
		vlfGuiRemoveProgressBar(vlf_progressbar);
		vlf_progressbar = NULL;
	}

	if(vlf_progresstext != NULL)
	{
		vlfGuiRemoveText(vlf_progresstext);
		vlf_progresstext = NULL;
	}

	if(status != NULL)
	{
		vlfGuiRemoveText(status);
		status = NULL;
	}

	vlfGuiCancelCentralMenu();

	if((vlfGuiGetButtonConfig() && showback_prev) || (!vlfGuiGetButtonConfig() && showenter_prev))
	{
		vlfGuiCancelBottomDialog();
		showback_prev = 0;
		showenter_prev = 0;
	}

	if(showmenu)
	{
		MainMenu(sel);
	}

	if(showback && !showback_prev)
	{
		vlfGuiBottomDialog(VLF_DI_BACK, -1, 1, 0, VLF_DEFAULT, OnBackToMainMenu);
		showback_prev = 1;
	}
}

void AddWaitIcon()
{
	if(waiticon != NULL)
	{
		vlfGuiRemoveShadowedPicture(waiticon);
		waiticon = NULL;
	}

	waiticon = vlfGuiAddWaitIcon();
}

void RemoveWaitIcon()
{
	if(waiticon != NULL)
	{
		vlfGuiRemoveShadowedPicture(waiticon);
		waiticon = NULL;
	}
}

int OnShutdownOrReboot(int enter)
{
	AddWaitIcon();
	enter ^= vlfGuiGetButtonConfig();
	
	if (enter)
	{
		scePowerRequestStandby();
	}
	else
	{
		scePowerRequestColdReset(0);
	}

	return VLF_EV_RET_REMOVE_HANDLERS;
}

void AddShutdownRebootBD(int bd)
{
	char reboot[128];
	char shutdown[128];
	UTF82Unicode(STRING[STR_REBOOT], reboot);
	UTF82Unicode(STRING[STR_SHUTDOWN], shutdown);

	vlfGuiCustomBottomDialog((bd) ? NULL : reboot, shutdown, 0, vlfGuiGetButtonConfig(), 90, OnShutdownOrReboot);
}

void SetTitle(char *src, char *name, char *fmt, ...)
{
	va_list list;
	char msg[256];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	if (title_text !=  NULL)
	{
		vlfGuiRemoveText(title_text);
		title_text = NULL;
	}

	if (title_picture !=  NULL)
	{
		vlfGuiRemovePicture(title_picture);
		title_picture = NULL;
	}

	title_text = vlfGuiAddUnicodeText(0, 0, msg);
	title_picture = vlfGuiAddPictureResource(src, name, 4, -2);

	vlfGuiSetTitleBarEx(title_text, title_picture, 1, 0, 0);
}

void RemoveBackgroundHandler(int (* func)(void *), void *param)
{
	if (vlfGuiIsEventRegistered(func))
	{
		vlfGuiRemoveEventHandlerEx(func, param);
	}
}

void AddBackgroundHandler(int button, int (* func)(void *), void *param)
{
	if (!vlfGuiIsEventRegistered(func))
	{
		vlfGuiAddEventHandler(button, 0, func, param);
	}
}

int SetBackground(void *param)
{
	int size, btn = pspGetKeyPress(0, 1000);

	if (btn & PSP_CTRL_LTRIGGER)
	{
		if (custom_bg)
		{
			custom_bg = 0;
		}
		else
		{
			custom_bg = 1;
		}
	}

	if(btn & PSP_CTRL_RTRIGGER)
	{
		vlfGuiSetBackgroundSystem(1);
		return 0;
	}

	if(custom_bg == 0)
	{
		int rand = Rand(0, size_wallpapers / 6176);
		vlfGuiSetBackgroundFileBuffer(wallpapers + (rand * 6176), 6176, 1);
		return 1;
	}
	else
	{
		custom_bg = 1;

		if(FileExists("flash0:/vsh/resource/01-12.bmp"))
		{
			size = GetFileSize("flash0:/vsh/resource/01-12.bmp");
			int size2 = GetFileSize("flash0:/vsh/resource/13-27.bmp");
			int rand = Rand(0, (size + size2) / 6176);

			if(rand < size / 6176)
			{
				ReadFile("flash0:/vsh/resource/01-12.bmp", (rand * 6176), sm_buffer2, 6176);
			}
			else
			{
				ReadFile("flash0:/vsh/resource/13-27.bmp", ((rand * 6176) - size), sm_buffer2, 6176);
			}

			vlfGuiSetBackgroundFileBuffer(sm_buffer2, 6176, 1);

			return 1;
		}
		else
		{
			vlfGuiSetBackgroundSystem(1);
			return 0;
		}
	}

	return -1;
}

VlfText vlfGuiAddUnicodeText(int x, int y, char *fmt, ...)
{
	va_list list;
	char text_utf8[512];
	char text_unicode[512];

	va_start(list, fmt);
	vsprintf(text_utf8, fmt, list);
	va_end(list);

	UTF82Unicode(text_utf8, text_unicode);
	return vlfGuiAddText(x, y, text_unicode);
}
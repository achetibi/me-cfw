#include <pspsdk.h>
#include <pspkernel.h>
#include <psppower.h>
#include <pspctrl.h>
#include <pspwlan.h>
#include <psprtc.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <vlf.h>

#include "dcman.h"
#include "nand.h"
#include "trans.h"
#include "common.h"
#include "vlfutils.h"
#include "idsregeneration.h"
#include "../kpspident/main.h"

extern u8 *big_buffer;
extern u8 *sm_buffer1;
extern char st_text[256];
extern char file_path[64];
extern int BIG_BUFFER_SIZE;
extern const char **g_messages;

#define VLF_ITEMS 20
extern VlfText vlf_texts[VLF_ITEMS];
extern VlfSpin vlf_spins[VLF_ITEMS];

extern int flash_sizes[4];
extern int totalflash_size;
extern int showback_prev;
extern int showenter_prev;
extern int selected_region;
extern int macrandom;
extern int sel_reg;
extern int selitem;
extern int mode;

u8 mac[6];
int selected_spin;

SceUID mac_thid = -1;
SceUID phformat_cb;

#define NELEMS(a) sizeof(a) / sizeof(a[0])

int OnDumpNandComplete(void *param)
{
	int nbb = *(int *)param;

	dcSetCancelMode(0);
	ResetScreen(0, 1, 0);
	
	SetStatus(nbb ? 80 : 240, nbb ? 100 : 120, nbb ? VLF_ALIGNMENT_LEFT : VLF_ALIGNMENT_CENTER, st_text);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int DumpNandThread(SceSize args, void *argp)
{
	SceUID fd;
	u32 pagesize, ppb, totalblocks, extrasize, blocksize, totalpages;
	int nbfit;
	int i, j;
	int badblocks[28], nbb = 0;

	dcSetCancelMode(1);
	dcGetNandInfo(&pagesize, &ppb, &totalblocks);
	extrasize = pagesize / 32;
	blocksize = (pagesize+extrasize) * ppb;
	totalpages = totalblocks * ppb;
	nbfit = BIG_BUFFER_SIZE / blocksize;

	fd = sceIoOpen(file_path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_CREATING_NAND_DUMP_BIN], fd, pspNandGetScramble());
	}

	for (i = 0; i < totalpages; )
	{
		u8 *p;
		memset(big_buffer, 0xff, nbfit * blocksize);
		p = big_buffer;

		for (j = 0; j < nbfit && i < totalpages; j++)
		{
			dcLockNand(0);
			
			if (dcReadNandBlock(i, p) == -1)
			{
				if (nbb < 28)
				{
					badblocks[nbb++] = i / ppb;
				}
			}

			dcUnlockNand();

			i += ppb;
			p += (528 * ppb);

			SetGenericProgress(i, totalpages, 0);
			scePowerTick(0);
		}		

		sceIoWrite(fd, big_buffer, j * blocksize);
	}

	sceIoClose(fd);
	SetProgress(100, 1);

	strcpy(st_text, STRING[STR_DUMP_IS_COMPLETE]);
	if (nbb > 0)
	{
		strcat(st_text, STRING[STR_BAD_BLOCK_WERE_FOUND]);

		for (i = 0; i < nbb; i++)
		{
			if (i && (i % 7) == 0)
			{
				strcat(st_text, "\n");
			}
			
			sprintf(st_text + strlen(st_text), "%d", badblocks[i]);

			if (i == (nbb - 1))
			{
				strcat(st_text, ".");
			}
			else
			{
				strcat(st_text, ", ");
			}			
		}
	}

	vlfGuiAddEventHandler(0, 700000, OnDumpNandComplete, &nbb);

	return sceKernelExitDeleteThread(0);
}

void DumpNand(char *fmt, ...)
{
	u32 nandsize;
	va_list list;
	va_start(list, fmt);
	vsprintf(file_path, fmt, list);
	va_end(list);

	if (FileExists(file_path))
	{
		if (vlfGuiMessageDialog(STRING[STR_AN_EXISTING_NAND_DUMP_WAS_FOUND], VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_YESNO | VLF_MD_INITIAL_CURSOR_NO) != 1)
		{
			OnBackToMainMenu(0);
			return;
		}

		sceIoRemove(file_path);
	}

	dcGetHardwareInfo(NULL, NULL, NULL, NULL, NULL, NULL, &nandsize);
	if (pspGetMsFreeSpace() < nandsize + 0x400000)
	{
		ErrorReturn(0, 0, STRING[STR_INSUFFICIENT_FREE_MS_SPACE]);
		return;
	}

	SetTitle("update_plugin", "tex_update_icon", STRING[STR_DUMP_NAND]);
	InitProgress(1, 1, 1, 80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_DUMPING_NAND]);
	RemoveBackgroundHandler(SetBackground, NULL);

	SceUID thid = sceKernelCreateThread("DumpNandThread", DumpNandThread, 0x18, 0x10000, 0, NULL);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 0, NULL);
	}
}

int OnRestoreNandComplete(void *param)
{
	dcSetCancelMode(0);
	ResetScreen(0, 0, 0);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_NAND_RESTORE_IS_COMPLETE]);
	AddShutdownRebootBD(0);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int RestoreNandThread(SceSize args, void *argp)
{
	u32 pagesize, ppb, totalblocks, extrasize, blocksize, totalpages, totalsize;
	int nbfit;
	SceUID fd;
	int i, j, k;
	int n, m;
	u32 ppn = 0;
	u8 *user, *spare;
	u8 *p, *q, *r;
	int error = 0;
	
	dcSetCancelMode(1);
	dcGetNandInfo(&pagesize, &ppb, &totalblocks);

	extrasize = pagesize / 32;
	blocksize = (pagesize+extrasize) * ppb;
	totalpages = totalblocks * ppb;
	totalsize = totalpages * (pagesize + extrasize);
	nbfit = BIG_BUFFER_SIZE / blocksize;

	user = malloc64(ppb * pagesize);
	spare = malloc64(ppb * extrasize);

	if (totalsize != (33 * 1024 * 1024) && totalsize != (66 * 1024 * 1024))
	{
		ErrorReturn(1, 0, STRING[STR_NAND_INFO_NOT_EXPECTED]);
	}

	fd = sceIoOpen(file_path, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_OPENING_NAND_DUMP_BIN], fd, pspNandGetScramble());
	}
	
	n = totalblocks / nbfit;
	
	if ((totalblocks % nbfit) != 0)
	{
		n++;
	}

	dcLockNand(1);
	
	for (i = 0; i < n; i++)
	{
		sceIoRead(fd, big_buffer, nbfit * blocksize);
		p = big_buffer;

		if (i == (n-1))
		{
			m = totalblocks % nbfit;
			if (m == 0)
			{
				m = nbfit;
			}
		}
		else
		{
			m = nbfit;
		}

		for (j = 0; j < m; j++)
		{
			q = user;
			r = spare;
			
			for (k = 0; k < 32; k++)
			{
				memcpy(q, p, 512);
				memcpy(r, p + 512, 16);

				p += 528;
				q += 512;
				r += 16;
			}

			if (ppn >= totalpages)
			{
				dcUnlockNand();
				ErrorReturn(1, 0, STRING[STR_RESTORE_BREAK]);
			}
			
			if (1)
			{
				if (dcEraseNandBlock(ppn) >= 0)
				{
					if (dcWriteNandBlock(ppn, user, spare) < 0)
					{
						error++;
					}
				}
				else
				{
					error++;
				}			
			}

			if (error > 100)
			{
				dcUnlockNand();
				ErrorReturn(1, 0, STRING[STR_THERE_ARE_BEING_TOO_MANY_ERRORS]);
			}

			ppn += 32;
			SetGenericProgress(ppn, totalpages, 0);
		}
	}

	dcUnlockNand();

	sceIoClose(fd);
	free(user);
	free(spare);
	
	SetProgress(100, 1);

	vlfGuiAddEventHandler(0, 700000, OnRestoreNandComplete, NULL);

	return sceKernelExitDeleteThread(0);
}

void RestoreNand(char *fmt, ...)
{
	u32 dummy, nandsize;
	u64 dummy64;
	int size;
	va_list list;
	va_start(list, fmt);
	vsprintf(file_path, fmt, list);
	va_end(list);
	
	dcGetHardwareInfo(&dummy, &dummy, &dummy, &dummy, &dummy64, &dummy, &nandsize);
	size = GetFileSize(file_path);
	nandsize += (nandsize / 32);
	
	if (!FileExists(file_path))
	{
		ErrorReturn(0, 0, STRING[STR_NAND_DUMP_NOT_FOUND_AT_ROOT], pspNandGetScramble());
		return;
	}
	
	if (size != nandsize)
	{
		ErrorReturn(0, 0, STRING[STR_NAND_DUMP_HAS_NOT_THE_CORRECT_SIZE], pspNandGetScramble());
		return;
	}
	
	if (vlfGuiMessageDialog(STRING[STR_PHYSICAL_NAND_RESTORE_WARNING], VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_YESNO | VLF_MD_INITIAL_CURSOR_NO) != 1)
	{
		OnBackToMainMenu(0);
		return;
	}

	SetTitle("update_plugin", "tex_update_icon", STRING[STR_RESTORE_NAND]);
	InitProgress(1, 1, 1, 80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_RESTORING_NAND]);
	RemoveBackgroundHandler(SetBackground, NULL);

	SceUID thid = sceKernelCreateThread("RestoreNandThread", RestoreNandThread, 0x18, 0x10000, 0, NULL);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 0, NULL);
	}
}

int OnCheckNandThread(void *param)
{
	dcSetCancelMode(0);
	ResetScreen(0, 1, 0);
	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, st_text);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int CheckNandThread(SceSize args, void *argp)
{
	u32 pagesize, ppb, totalblocks, extrasize, blocksize, totalpages;
	int nbfit;
	int i, j;
	int badblocks[28], nbb = 0;

	dcSetCancelMode(1);
	dcGetNandInfo(&pagesize, &ppb, &totalblocks);
	extrasize = pagesize / 32;
	blocksize = (pagesize + extrasize) * ppb;
	totalpages = totalblocks * ppb;
	nbfit = BIG_BUFFER_SIZE / blocksize;

	for (i = 0; i < totalpages; )
	{
		u8 *p;
		memset(big_buffer, 0xff, nbfit * blocksize);

		p = big_buffer;
		for (j = 0; j < nbfit && i < totalpages; j++)
		{
			dcLockNand(0);
			
			if (dcReadNandBlock(i, p) == -1)
			{
				if (nbb < 28)
				{
					badblocks[nbb++] = i / ppb;
				}
			}

			dcUnlockNand();

			i += ppb;
			p += (528 * ppb);

			SetGenericProgress(i, totalpages, 0);
			scePowerTick(0);
		}		
	}
	SetProgress(100, 1);

	strcpy(st_text, STRING[STR_VERIFICATION_IS_COMPLETE]);

	if (nbb > 0)
	{
		strcat(st_text, STRING[STR_BAD_BLOCK_WERE_FOUND]);

		for (i = 0; i < nbb; i++)
		{
			if (i && (i % 7) == 0)
			{
				strcat(st_text, "\n");
			}
			
			sprintf(st_text + strlen(st_text), "%d", badblocks[i]);

			if (i == (nbb - 1))
			{
				strcat(st_text, ".");
			}
			else
			{
				strcat(st_text, ", ");
			}			
		}
	}
	else
	{
		strcat(st_text, STRING[STR_NO_BAD_BLOCK_WERE_FOUND]);
	}

	vlfGuiAddEventHandler(0, 700000, OnCheckNandThread, NULL);

	return sceKernelExitDeleteThread(0);
}

void CheckNand()
{
	SetTitle("update_plugin", "tex_update_icon", STRING[STR_CHECK_NAND]);
	InitProgress(1, 1, 1, 80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_VERIFYING_NAND]);
	RemoveBackgroundHandler(SetBackground, NULL);

	SceUID thid = sceKernelCreateThread("CheckNandThread", CheckNandThread, 0x18, 0x10000, 0, NULL);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 0, NULL);
	}
}

int PhysicalFormatCallback(int count, u32 arg, void *param)
{
	u16 value, max;
	
	value = arg >> 16;
	max = arg &0xFFFF;

	SetProgress((95 * value) / max, 0);
	scePowerTick(0);
	
	return 0;
}

int CallbackThread(SceSize args, void *argp)
{
	phformat_cb = sceKernelCreateCallback("PhFormatProgressCallback", (void *)PhysicalFormatCallback, NULL);
	sceKernelSleepThreadCB();

	return 0;
}

int LoadFormatModules()
{
	SceUID mod = sceKernelLoadModule("flash0:/kd/emc_sm_updater.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
	{
		return mod;
	}

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
		{
			return mod;
		}
	}

	if (pspKernelGetModel() == 0)
	{
		dcPatchModule("sceNAND_Updater_Driver", 1, 0x0D7E, 0xAC60);
	}
	else
	{
		dcPatchModule("sceNAND_Updater_Driver", 1, 0x0D7E, 0xAC64);
	}

	mod = sceKernelLoadModule("flash0:/kd/lfatfs_updater.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
	{
		return mod;
	}

	dcPatchModuleString("sceLFatFs_Updater_Driver", "flash", "flach");

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
		{
			return mod;
		}
	}
	
	mod = sceKernelLoadModule("flash0:/kd/lflash_fatfmt_updater.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
	{
		return mod;
	}

	sceKernelDelayThread(10000);

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
		{
			return mod;
		}
	}
	
	mod = sceKernelLoadModule("flash0:/kd/lflash_fdisk.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
	{
		return mod;
	}

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
		{
			return mod;
		}
	}
	
	sceKernelDelayThread(10000);
	
	return 0;
}

int OnFormatNandComplete(void *param)
{
	dcSetCancelMode(0);
	ResetScreen(0, 1, 0);

	SetStatus(240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_FORMAT_IS_COMPLETE]);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int FormatNandThread(SceSize args, void *argp)
{
	int res;
	char *argv[12];
	char part0[10], part1[10], part2[10], part3[10];
	
	dcSetCancelMode(1);

	if (LoadFormatModules() < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_LOADING_MODULES]);
	}
	sceKernelDelayThread(1200000);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_UNASSIGN_FLASH]);
	UnassignFlash();
	sceKernelDelayThread(1200000);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_PHYSICAL_FORMAT]);
	dcRegisterPhysicalFormatCallback(phformat_cb);

	res = sceIoDevctl("lflach0:", 0x03d802, 0, 0, 0, 0);
	dcUnregisterPhysicalFormatCallback();

	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_PHYSICAL_FORMAT], res);
	}
	
	SetProgress(95, 1);
	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_CREATING_PARTITION]);

	sprintf(part0, "%d", flash_sizes[0] / 32);
	sprintf(part1, "%d", flash_sizes[1] / 32);
	sprintf(part2, "%d", flash_sizes[2] / 32);
	sprintf(part3, "%d", flash_sizes[3] / 32);

	argv[0] = "fdisk";
	argv[1] = "-H";
	argv[2] = "2";
	argv[3] = "-S";
	argv[4] = "32";
	argv[5] = part0;
	argv[6] = part1;
	argv[7] = part2;
	argv[8] = part3;
	argv[9] = "0";
	argv[10] = "0";
	argv[11] = "lflach0:0";

	res = dcLflashStartFDisk(12, argv);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FDISK], res);
	}
	
	sceKernelDelayThread(1400000);
	SetProgress(96, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_LOGICAL_FORMAT_FLASH_PART], 0);

	argv[0] = "fatfmt";
	argv[1] = "lflach0:0,0";

	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_FORMAT_FLASH_FAILED], 0, res);
	}

	sceKernelDelayThread(1200000);
	SetProgress(97, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_LOGICAL_FORMAT_FLASH_PART], 1);
	
	argv[1] = "lflach0:0,1";

	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_FORMAT_FLASH_FAILED], 1, res);
	}

	sceKernelDelayThread(1200000);
	SetProgress(98, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_LOGICAL_FORMAT_FLASH_PART], 2);

	argv[1] = "lflach0:0,2";
	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_FORMAT_FLASH_FAILED], 2, res);
	}

	sceKernelDelayThread(1200000);
	SetProgress(99, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_LOGICAL_FORMAT_FLASH_PART], 3);

	argv[1] = "lflach0:0,3";
	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_FORMAT_FLASH_FAILED], 3, res);
	}
	
	SetProgress(100, 1);
	vlfGuiAddEventHandler(0, 700000, OnFormatNandComplete, NULL);

	return sceKernelExitDeleteThread(0);
}

int OnSpinSelectDown(void *param)
{
	if (selected_spin != 3)
	{
		int sum;

		vlfGuiSetSpinState(vlf_spins[selected_spin], VLF_SPIN_STATE_NOT_FOCUS);
		vlfGuiSetSpinState(vlf_spins[selected_spin+1], VLF_SPIN_STATE_FOCUS);
		
		selected_spin++;
		sum = flash_sizes[0] + flash_sizes[1] + flash_sizes[2] + flash_sizes[3];
		vlfGuiSetIntegerSpinMinMax(vlf_spins[selected_spin], 96, flash_sizes[selected_spin]+totalflash_size-sum);
	}

	return VLF_EV_RET_REMOVE_EVENT;
}

int OnSpinSelectUp(void *param)
{
	if (selected_spin != 0)
	{
		int sum;

		vlfGuiSetSpinState(vlf_spins[selected_spin], VLF_SPIN_STATE_NOT_FOCUS);
		vlfGuiSetSpinState(vlf_spins[selected_spin-1], VLF_SPIN_STATE_FOCUS);
		
		selected_spin--;
		sum = flash_sizes[0] + flash_sizes[1] + flash_sizes[2] + flash_sizes[3];
		vlfGuiSetIntegerSpinMinMax(vlf_spins[selected_spin], 96, flash_sizes[selected_spin]+totalflash_size-sum);
	}

	return VLF_EV_RET_REMOVE_EVENT;
}

int OnSpinEdited(int enter)
{
	vlfGuiSetSpinState(vlf_spins[selected_spin], VLF_SPIN_STATE_FOCUS);
	if((vlfGuiGetButtonConfig() && showback_prev) || (!vlfGuiGetButtonConfig() && showenter_prev))
	{
		vlfGuiCancelBottomDialog();
		showback_prev = 0;
		showenter_prev = 0;
	}

	vlfGuiBottomDialog(VLF_DI_BACK, VLF_DI_EDIT, 1, 0, VLF_DEFAULT, OnEditSpinOrBack);
	vlfGuiAddEventHandler(PSP_CTRL_UP, 0, OnSpinSelectUp, NULL);
	vlfGuiAddEventHandler(PSP_CTRL_DOWN, 0, OnSpinSelectDown, NULL);
	vlfGuiSetPageControlEnable(1);
	showenter_prev = 1;
	showback_prev = 1;

	if (enter)
	{
		
		vlfGuiGetIntegerSpinValue(vlf_spins[selected_spin], &flash_sizes[selected_spin]);
	}
	else
	{
		vlfGuiSetIntegerSpinValue(vlf_spins[selected_spin], flash_sizes[selected_spin]);
	}

	return VLF_EV_RET_NOTHING;
}

int OnEditSpinOrBack(int enter)
{
	vlfGuiRemoveEventHandler(OnSpinSelectUp);
	vlfGuiRemoveEventHandler(OnSpinSelectDown);
	
	if((vlfGuiGetButtonConfig() && showback_prev) || (!vlfGuiGetButtonConfig() && showenter_prev))
	{
		vlfGuiCancelBottomDialog();
		showback_prev = 0;
		showenter_prev = 0;
	}

	if (enter)
	{
		if (mode == 0x00010403)
		{	
			vlfGuiSetPageControlEnable(0);
			vlfGuiBottomDialog(VLF_DI_CANCEL, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, OnSpinEdited);
			vlfGuiSetSpinState(vlf_spins[selected_spin], VLF_SPIN_STATE_ACTIVE);
			showenter_prev = 1;
			showback_prev = 1;
		}
		else if (mode == 0x00020403)
		{
			vlfGuiSetPageControlEnable(0);
			InitProgress(1, 1, 1, 80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_LOADING_MODULES]);
			RemoveBackgroundHandler(SetBackground, NULL);

			SceUID thid = sceKernelCreateThread("FormatNandThread", FormatNandThread, 0x18, 0x10000, 0, NULL);
			if (thid >= 0)
			{
				sceKernelStartThread(thid, 0, NULL);
			}
		}
		
	}
	else
	{
		OnBackToMainMenu(0);
	}

	return VLF_EV_RET_NOTHING;
}

void FormatNandPage()
{
	SetTitle("update_plugin", "tex_update_icon", STRING[STR_FORMAT_FLASH]);
	vlfGuiSetRectangleFade(0, VLF_TITLEBAR_HEIGHT, 480, 272 - VLF_TITLEBAR_HEIGHT, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0, NULL, NULL, 0);

	if (mode == 0x00010403)
	{
		vlf_texts[0] = vlfGuiAddUnicodeText(110, 80,  STRING[STR_FLASH_PART_SIZE], 0);
		vlf_texts[1] = vlfGuiAddUnicodeText(110, 105, STRING[STR_FLASH_PART_SIZE], 1);
		vlf_texts[2] = vlfGuiAddUnicodeText(110, 130, STRING[STR_FLASH_PART_SIZE], 2);
		vlf_texts[3] = vlfGuiAddUnicodeText(110, 155, STRING[STR_FLASH_PART_SIZE], 3);
		
		vlf_spins[0] = vlfGuiAddIntegerSpinControl(240, 80,  96, totalflash_size, flash_sizes[0], 32, 0, 50, VLF_SPIN_STATE_FOCUS, NULL, " KB");
		vlf_spins[1] = vlfGuiAddIntegerSpinControl(240, 105, 96, totalflash_size, flash_sizes[1], 32, 0, 50, VLF_SPIN_STATE_NOT_FOCUS, NULL, " KB");
		vlf_spins[2] = vlfGuiAddIntegerSpinControl(240, 130, 96, totalflash_size, flash_sizes[2], 32, 0, 50, VLF_SPIN_STATE_NOT_FOCUS, NULL, " KB");
		vlf_spins[3] = vlfGuiAddIntegerSpinControl(240, 155, 96, totalflash_size, flash_sizes[3], 32, 0, 50, VLF_SPIN_STATE_NOT_FOCUS, NULL, " KB");
		
		
		int	sum = flash_sizes[0] + flash_sizes[1] + flash_sizes[2] + flash_sizes[3];

		vlfGuiSetIntegerSpinMinMax(vlf_spins[0], 96, flash_sizes[0]+totalflash_size-sum);
		vlfGuiSetIntegerSpinMinMax(vlf_spins[1], 96, flash_sizes[1]+totalflash_size-sum);
		vlfGuiSetIntegerSpinMinMax(vlf_spins[2], 96, flash_sizes[2]+totalflash_size-sum);
		vlfGuiSetIntegerSpinMinMax(vlf_spins[3], 96, flash_sizes[3]+totalflash_size-sum);

		vlfGuiAddEventHandler(PSP_CTRL_UP, 0, OnSpinSelectUp, NULL);
		vlfGuiAddEventHandler(PSP_CTRL_DOWN, 0, OnSpinSelectDown, NULL);
		vlfGuiBottomDialog(-1, VLF_DI_EDIT, 1, 0, VLF_DEFAULT, OnEditSpinOrBack);
		showenter_prev = 1;
		selected_spin = 0;
	}
	else if (mode == 0x00020403)
	{	
		InitProgress(0, 0, 1, 240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_PRESS_BUTTON_TO_BEGIN_FORMAT], vlfGuiGetButtonConfig() ? "X" : "O");
		vlfGuiRemoveEventHandler(OnSpinSelectUp);
		vlfGuiRemoveEventHandler(OnSpinSelectDown);
		
		vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, OnEditSpinOrBack);
		showenter_prev = 1;
	}
}

int LoadRegeneration()
{
	SceUID mod = sceKernelLoadModule("flash0:/kd/idsregeneration.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
	{
		return mod;
	}

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
		{
			return mod;
		}
	}

	return 0;
}

int VerifyCertificates(u8 *buf)
{
	int i;
	int res;

	for (i = 0; i < 3; i++)
	{
		res = dcIdStorageReadLeaf(0x100 + i, buf + (i * 0x200));
		if (res < 0)
		{
			res = dcIdStorageReadLeaf(0x120 + i, buf + (i * 0x200));
			if (res < 0)
			{
				return res;
			}
		}
	}

	u8 *certStart = &buf[0x38];
	
	for(i = 0; i < 6; i++)
	{
		res = dcKirkCmd(NULL, 0, &certStart[0xB8 * i], 0xB8, 0x12);
		if (res	!= 0)
		{
			return (0xC0000000 | (i << 24) | res);
		}		
	}

	return 0;
}

int OnUpdateMacAddress(void *param)
{
	macrandom = 0;
	vlfGuiSetTextF(vlf_texts[1], STRING[STR_PSP_INFORMATIONS_MAC_ADDRESS], mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int GetMacThread(SceSize args, void *argp)
{
	int res, i;

	res = dcQueryRealMacAddress(mac);
	if (res < 0)
	{
		//switch was off during all DC execution, and has been just turned on
		// Wait some seconds for wlanchipinit thread initialization
		AddWaitIcon();

		for (i = 0; i < 7; i++)
		{
			vlfGuiDelayAllEvents(1000000);
			sceKernelDelayThread(1000000);

			res = dcQueryRealMacAddress(mac);
			if (res >= 0 || (res < 0 && sceWlanGetSwitchState() == 0))
			{
				break;
			}
		}

		RemoveWaitIcon();
		sceKernelDelayThread(100000);

		if (res < 0)
		{
			if (sceWlanGetSwitchState() == 0)
			{
				ErrorReturn(1, 0, STRING[STR_THE_WLAN_SWITCH_IS_OFF]);
			}

			else
			{
				ErrorReturn(1, 0, STRING[STR_CANNOT_GET_REAL_MAC_ADDRESS]);
			}
		}
	}

	if (res >= 0)
	{
		vlfGuiAddEventHandler(0, -2, OnUpdateMacAddress, NULL);
	}

	mac_thid = -1;
	return sceKernelExitDeleteThread(0);
}

int OnCreateIdstorageComplete(void *param)
{
	dcSetCancelMode(0);
	ResetScreen(0, 1, 0);

	SetStatus(240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_IDSTORAGE_SUCCESFULLY_CREATED]);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int CreateIdstorageThread(SceSize args, void *argp)
{
	u8 setparam[8];
	u32 tachyon, baryon, pommel, mb, region;
	u64 fuseid;
	int i;
	IdsIndex *index;
	int n, res;

	dcSetCancelMode(1);	

	if (LoadRegeneration() < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_LOADING_MODULES]);
	}
	
	for(i = 0; i < 5; i++)
    {
		if (dcSysconReceiveSetParam(1, setparam) == 0)
        {
			break;
        }       
    }

	dcGetHardwareInfo(&tachyon, &baryon, &pommel, &mb, &fuseid, NULL, NULL);

	if (selected_region == 0)
	{
		region = 5;
	}
	else if (selected_region == 1)
	{
		region = 3;
	}
	else
	{
		region = 4;
	}

	if (idsRegenerationSetup(tachyon, baryon, pommel, mb, fuseid, region, setparam) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationSetup");
	}
	
	sceKernelDelayThread(2300000);
	SetProgress(5, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_FORMATTING_IDSTORAGE]);
	sceKernelDelayThread(100000);

	dcIdStorageUnformat();
	res = dcIdStorageFormat();
	dcIdStorageFlush();
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_FORMATTING_IDSTORAGE], res);
	}
	
	sceKernelDelayThread(2300000);
	SetProgress(11, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_CREATING_IDSTORAGE_INDEX]);
	sceKernelDelayThread(500000);
	
	index = (IdsIndex *)big_buffer;

	if (idsRegenerationGetIndex(index, &n) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetIndex");
	}
	
	for (i = 0; i < n; i++)
	{
		if (index[i].keyfirst == index[i].keyend)
		{
			res = dcIdStorageCreateLeaf(index[i].keyfirst);
			if (res < 0)
			{
				ErrorReturn(1, 0, STRING[STR_ERROR_CREATING_KEY], res, index[i].keyfirst);
			}
		}
		else
		{
			int j;
			int m = index[i].keyend - index[i].keyfirst + 1;
			u16 leaves[0x50];

			if (m <= 1 || m > 0x50)
			{
				ErrorReturn(1, 0, STRING[STR_UGLY_BUG_IN_IDSREGENERATIONGETINDEX]); 
			}

			for (j = 0; j < m; j++)
			{
				leaves[j] = index[i].keyfirst + j;
			}

			res = dcIdStorageCreateAtomicLeaves(leaves, m);
			if (res < 0)
			{
				ErrorReturn(1, 0, STRING[STR_ERROR_CREATING_KEYS], res, index[i].keyfirst, index[i].keyend);
			}
		}
	}
	
	sceKernelDelayThread(2500000);
	SetProgress(17, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_GENERATING_CERTIFICATES_AND_UMD_KEYS]);
	sceKernelDelayThread(300000);

	res = idsRegenerationCreateCertificatesAndUMDKeys(big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_GENERATING_CERTIFICATES_AND_UMD_KEYS], res);
	}
	
	for (i = 0; i < 0x20; i++)
	{
		res = dcIdStorageWriteLeaf(0x100 + i, big_buffer+(0x200 * i));
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x100 + i);
		}

		res = dcIdStorageWriteLeaf(0x120 + i, big_buffer+(0x200 * i));
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x120 + i);
		}
	}

    res = dcIdStorageFlush();
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_SCEIDSTORAGEFLUSH_FAILED], res);
	}
	
	sceKernelDelayThread(2500000);
	SetProgress(23, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_VERIFYING_CERTIFICATES]);
	sceKernelDelayThread(400000);

	res = VerifyCertificates(big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_CERTIFICATES_VERIFICATION_FAILED], res);
	}

	sceKernelDelayThread(2500000);
	SetProgress(28, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_CREATING_OTHER_KEYS]);
	sceKernelDelayThread(800000);

	if (idsRegenerationGetHwConfigKeys(big_buffer) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetHwConfigKeys");
	}
	
	for (i = 0; i < 3; i++)
	{
		res = dcIdStorageWriteLeaf(0x0004 + i, big_buffer+(0x200 * i));
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0004 + i);
		}
	}

	sceKernelDelayThread(800000);
	SetProgress(33, 1);

	if (idsRegenerationGetMGKeys(big_buffer) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetMGKeys");
	}

	for (i = 0; i < 0x20; i++)
	{
		res = dcIdStorageWriteLeaf(0x0010+i, big_buffer+(0x200 * i));
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0010 + i);
		}
	}
	
	sceKernelDelayThread(800000);
	SetProgress(39, 1);

	if (idsRegenerationGetFactoryBadBlocksKey(big_buffer) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetFactoryBadBlocksKey");
	}

	res = dcIdStorageWriteLeaf(0x000F, big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x000F);
	}

	sceKernelDelayThread(700000);
	SetProgress(44, 1);

	if (idsRegenerationGetSerialKey(big_buffer) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetSerialKey");
	}
	
	res = dcIdStorageWriteLeaf(0x0050, big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0050);
	}

	sceKernelDelayThread(700000);
	SetProgress(49, 1);

	memset(big_buffer, 0, 0x200);
	memcpy(big_buffer, mac, 6);

	res = dcIdStorageWriteLeaf(0x0044, big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0044);
	}
	
	sceKernelDelayThread(700000);
	SetProgress(54, 1);

	if (idsRegenerationGetWlanKey(big_buffer) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetWlanKey");
	}

	res = dcIdStorageWriteLeaf(0x0045, big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0045);
	}

	sceKernelDelayThread(700000);
	SetProgress(59, 1);

	if (idsRegenerationGetUsbKeys(big_buffer) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetUsbKeys");
	}

	for (i = 0; i < 3; i++)
	{
		res = dcIdStorageWriteLeaf(0x0041+i, big_buffer+(0x200 * i));
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0041 + i);
		}
	}
	
	sceKernelDelayThread(800000);
	SetProgress(64, 1);

	if (idsRegenerationGetUnkKey140(big_buffer) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetUnkKey140");
	}

	res = dcIdStorageWriteLeaf(0x0140, big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0140);
	}

	sceKernelDelayThread(700000);
	SetProgress(69, 1);

	if (idsRegenerationGetMGKey40(big_buffer) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetMGKey40");
	}

	res = dcIdStorageWriteLeaf(0x0040, big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0040);
	}

	sceKernelDelayThread(700000);
	SetProgress(74, 1);

	if (idsRegenerationGetUnkKeys3X(big_buffer) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetUnkKeys3X");
	}
	
	for (i = 0; i < 0x10; i++)
	{
		res = dcIdStorageWriteLeaf(0x0030 + i, big_buffer+(0x200 * i));
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0030 + i);
		}
	}

	sceKernelDelayThread(800000);
	SetProgress(80, 1);

	res = idsRegenerationGetParentalLockKey(big_buffer);
	if (res > 0)
	{
		res = dcIdStorageWriteLeaf(0x0047, big_buffer);
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0047);
		}
	}

	else if (res == 0)
	{
		/* Doesn't apply to this psp */
	}
	
	else
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetParentalLockKey");
	}

	sceKernelDelayThread(800000);
	SetProgress(85, 1);

	res = idsRegenerationGenerateFactoryFirmwareKey(big_buffer);
	if (res > 0)
	{
		res = dcIdStorageWriteLeaf(0x0051, big_buffer);
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0051);
		}
	}

	else if (res == 0)
	{
		/* Doesn't apply to this psp */
	}
	
	else
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGenerateFactoryFirmwareKey");
	}

	sceKernelDelayThread(800000);
	SetProgress(89, 1);

	res = idsRegenerationGetLCDKey(big_buffer);
	if (res > 0)
	{
		res = dcIdStorageWriteLeaf(0x0008, big_buffer);
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0008);
		}
	}

	else if (res == 0)
	{
		/* Doesn't apply to this psp */
	}

	else
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetLCDKey");
	}
	
	sceKernelDelayThread(800000);
	SetProgress(93, 1);

	res = idsRegenerationGenerateCallibrationKey(big_buffer);
	if (res > 0)
	{
		res = dcIdStorageWriteLeaf(0x0007, big_buffer);
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0007);
		}
	}

	else if (res == 0)
	{
		/* Doesn't apply to this psp */
	}

	else
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGenerateCallibrationKey");
	}
	
	sceKernelDelayThread(800000);
	SetProgress(98, 1);

	res = idsRegenerationGetUnkKeys5253(big_buffer);
	if (res > 0)
	{
		for (i = 0; i < 2; i++)
		{
			res = dcIdStorageWriteLeaf(0x0052 + i, big_buffer+(0x200 * i));
			if (res < 0)
			{
				ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0052 + i);
			}
		}
	}
	
	else if (res == 0)
	{
		/* Doesn't apply to this psp */
	}

	else
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetUnkKeys5253");
	}

	res = idsRegenerationGetDefaultXMBColorKey(big_buffer);
	if (res > 0)
	{
		res = dcIdStorageWriteLeaf(0x0054, big_buffer);
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, 0x0054);
		}
	}
	
	else if (res == 0)
	{
		/* Doesn't apply to this psp */
	}

	else
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationGetDefaultXMBColorKey");
	}

	res = dcIdStorageFlush();
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_SCEIDSTORAGEFLUSH_FAILED], res);
	}
	
	sceKernelDelayThread(800000);
	SetProgress(100, 1);

	vlfGuiAddEventHandler(0, 1200000, OnCreateIdstorageComplete, NULL);
	
	return sceKernelExitDeleteThread(0);
	
}

int CreateIdstorage(int enter)
{
	if (enter)
	{
		if (mode == 0x02010403)
		{
			if (mac_thid >= 0)
			{
				return VLF_EV_RET_NOTHING;
			}

			if (sceWlanGetSwitchState() == 0)
			{
				ErrorReturn(0, 0, STRING[STR_THE_WLAN_SWITCH_IS_OFF]);
				return VLF_EV_RET_NOTHING;
			}

			else
			{
				mac_thid = sceKernelCreateThread("GetMacThread", GetMacThread, 0x18, 0x10000, 0, NULL);
				if (mac_thid >= 0)
				{
					sceKernelStartThread(mac_thid, 0, NULL);
				}
			}
		}
		else if (mode == 0x03010403)
		{
			ResetScreen(0, 0, 0);
			vlfGuiSetPageControlEnable(0);
			if (vlfGuiMessageDialog(STRING[STR_CREATE_IDSTORAGE_WARNING], VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_YESNO | VLF_MD_INITIAL_CURSOR_NO) != 1)
			{
				OnBackToMainMenu(0);
				return VLF_EV_RET_NOTHING;
			}

			InitProgress(1, 1, 1, 80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_LOADING_MODULES]);
			RemoveBackgroundHandler(SetBackground, NULL);

			SceUID thid = sceKernelCreateThread("CreateIdstorageThread", CreateIdstorageThread, 0x18, 0x10000, 0, NULL);
			if (thid >= 0)
			{
				sceKernelStartThread(thid, 0, NULL);
			}
		}
	}
	else
	{
		OnBackToMainMenu(0);
	}
	
	return VLF_EV_RET_NOTHING;
}

void CreateIdstoragePage()
{
	SetTitle("update_plugin", "tex_update_icon", STRING[STR_NEW_IDSTORAGE]);
	vlfGuiSetRectangleFade(0, VLF_TITLEBAR_HEIGHT, 480, 272 - VLF_TITLEBAR_HEIGHT, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0, NULL, NULL, 0);

	if (mode == 0x01010403)
	{
		selected_region = sel_reg;
	
		int i;
		char *items[] =
		{
			STRING[STR_REGION_EUROPE],
			STRING[STR_REGION_JAPAN],
			STRING[STR_REGION_AMERICA],
		};
		
		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], unicode);
		}

		vlf_texts[0] = vlfGuiAddUnicodeText(240, 60, STRING[STR_SELECT_REGION]);
		vlfGuiSetTextAlignment(vlf_texts[0], VLF_ALIGNMENT_CENTER);

		vlfGuiCentralMenu(NELEMS(items), items, selected_region, NULL, 0, 0);
	}
	else if (mode == 0x02010403)
	{
		int i;
		char *items[] =
		{
			STRING[STR_GET_REAL_MAC]
		};
		
		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], unicode);
		}
		
		vlf_texts[0] = vlfGuiAddUnicodeText(240, 80, STRING[STR_SKIP_THIS_STEP_TO_GENERATE_RANDOM_MAC]);
		vlfGuiSetTextAlignment(vlf_texts[0], VLF_ALIGNMENT_CENTER);
		
		if (macrandom)
		{
			vlf_texts[1] = vlfGuiAddUnicodeText(240, 180, STRING[STR_MAC_ADDRESS_RANDOM]);
		}
		else
		{
			vlf_texts[1] = vlfGuiAddUnicodeText(240, 180, STRING[STR_PSP_INFORMATIONS_MAC_ADDRESS], mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		}

		vlfGuiSetTextAlignment(vlf_texts[1], VLF_ALIGNMENT_CENTER);
		vlfGuiCentralMenu(NELEMS(items), items, 0, NULL, 0, 0);
		vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, CreateIdstorage);
		showenter_prev = 1;
	}
	else if (mode == 0x03010403)
	{		
		int i;
		char *regions[] =
		{
			STRING[STR_REGION_EUROPE],
			STRING[STR_REGION_JAPAN],
			STRING[STR_REGION_AMERICA],
		};
		
		for(i = 0; i < NELEMS(regions); i++)
		{
			char unicode[128];
			UTF82Unicode(regions[i], unicode);
			regions[i] = malloc(strlen(unicode) + 1);
			strcpy(regions[i], unicode);
		}
		
		if (macrandom)
		{
			u32 md5[4];
			u64 tick;
			SceKernelUtilsMt19937Context ctx;

			if (sceRtcGetCurrentTick(&tick) < 0)
			{
				tick = sceKernelGetSystemTimeWide();
			}

			sceKernelUtilsMd5Digest((u8 *)&tick, sizeof(u64), (u8 *)md5);
			sceKernelUtilsMt19937Init(&ctx, md5[0] ^ md5[1] ^ md5[2] ^ md5[3]);

			mac[0] = 0x00;

			if (pspKernelGetModel() == 0)
			{
				mac[1] = 0x16;
				mac[2] = 0xFE;
			}

			else
			{
				mac[1] = 0x1D;
				mac[2] = 0xD9;
			}

			mac[3] = sceKernelUtilsMt19937UInt(&ctx);
			mac[4] = sceKernelUtilsMt19937UInt(&ctx);
			mac[5] = sceKernelUtilsMt19937UInt(&ctx);
		}

		char mac_address[128];
		sprintf(mac_address, STRING[STR_PSP_INFORMATIONS_MAC_ADDRESS], mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		vlf_texts[0] = vlfGuiAddUnicodeText(75, 85, STRING[STR_PSP_INFORMATIONS_REGION], regions[selected_region]);
		vlf_texts[1] = vlfGuiAddUnicodeText(75, 110, "%s %s", mac_address, macrandom ? STRING[STR_MAC_RANDOM] : "");
		
		InitProgress(0, 0, 1, 240, 190, VLF_ALIGNMENT_CENTER, STRING[STR_PRESS_BUTTON_TO_CREATE_IDSTORAGE], vlfGuiGetButtonConfig() ? "X" : "O");
		vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, CreateIdstorage);
		showenter_prev = 1;
	}
}

int OnChangeRegionComplete(void *param)
{
	dcSetCancelMode(0);
	ResetScreen(0, 1, 0);

	SetStatus(240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_REGING_CHANGED_SUCCESFULLY]);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int ChangeRegionThread(SceSize args, void *argp)
{
	int selected = *(int *)argp;
	u32 tachyon, baryon, pommel, mb, region;
	u64 fuseid;
	int i, res;

	dcSetCancelMode(1);
	dcGetHardwareInfo(&tachyon, &baryon, &pommel, &mb, &fuseid, NULL, NULL);

	if (LoadRegeneration() < 0)
	{
		ErrorReturn(1, 0, STRING[STR_LOADING_MODULES]);
	}

	if (selected == 0)
	{
		region = 5;
	}
	else if (selected == 1)
	{
		region = 3;
	}
	else
	{
		region = 4;
	}

	if (idsRegenerationSetup(tachyon, baryon, pommel, mb, fuseid, region, NULL) < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_IN_FUNCTION], "idsRegenerationSetup");
	}

	res = idsRegenerationCreateCertificatesAndUMDKeys(big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_GENERATING_CERTIFICATES_AND_UMD_KEYS], res);
	}
	
	for (i = 0; i < 0x20; i++)
	{
		int res2;
		
		res = dcIdStorageWriteLeaf(0x100 + i, big_buffer+(0x200 * i));
		res2 = dcIdStorageWriteLeaf(0x120 + i, big_buffer+(0x200 * i));
		
		if (res == 0x80000025)
		{
			res = res2;
		}
		else if (res2 == 0x80000025)
		{
			res2 = res;
		}

		if (res < 0 || res2 < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEYS], res, 0x100 + i, 0x120 + i);
		}
	}
	
	res = dcIdStorageFlush();
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_SCEIDSTORAGEFLUSH_FAILED], res);
	}

	res = VerifyCertificates(big_buffer);
	if (res < 0)
	{
		ErrorReturn(1, 0, STRING[STR_CERTIFICATES_VERIFICATION_FAILED], res);
	}

	vlfGuiAddEventHandler(0, 700000, OnChangeRegionComplete, NULL);

	return sceKernelExitDeleteThread(0);
}

void ChangeRegion(int region)
{
	selitem = 1;
	SetTitle("update_plugin", "tex_update_icon", STRING[STR_CHANGE_REGION]);
	if (vlfGuiMessageDialog(STRING[STR_CHANGE_REGION_WARNING], VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_YESNO | VLF_MD_INITIAL_CURSOR_NO) != 1)
	{
		OnBackToMainMenu(0);
		return;
	}

	InitProgress(0, 0, 1, 240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_CHANGING_REGION]);
	RemoveBackgroundHandler(SetBackground, NULL);

	SceUID thid = sceKernelCreateThread("ChangeRegionThread", ChangeRegionThread, 0x18, 0x10000, 0, NULL);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 4, &region);
	}
}

int OnFixMacComplete(void *param)
{
	dcSetCancelMode(0);
	ResetScreen(0, 1, 0);

	SetStatus(240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_ORIGINAL_MAC_WRITTEN_SUCCESFULLY]);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int FixMacThread(SceSize args, void *argp)
{
	int res, i;
	u8 mac[6];
	dcSetCancelMode(1);

	res = dcQueryRealMacAddress(mac);
	if (res < 0)
	{
		//switch was off during all DC execution, and has been just turned on
		// Wait some seconds for wlanchipinit thread initialization
		AddWaitIcon();
		
		for (i = 0; i < 7; i++)
		{
			vlfGuiDelayAllEvents(1000000);
			sceKernelDelayThread(1000000);

			res = dcQueryRealMacAddress(mac);
			if (res >= 0 || (res < 0 && sceWlanGetSwitchState() == 0))
			{
				break;
			}
		}

		RemoveWaitIcon();
		sceKernelDelayThread(100000);

		if (res < 0)
		{
			if (sceWlanGetSwitchState() == 0)
			{
				ErrorReturn(1, 0, STRING[STR_THE_WLAN_SWITCH_IS_OFF]);
			}

			else
			{
				ErrorReturn(1, 0, STRING[STR_CANNOT_GET_REAL_MAC_ADDRESS]);
			}
		}
	}

	if (res >= 0)
	{
		memset(big_buffer, 0, 0x200);
		memcpy(big_buffer, mac, 6);

		res = dcIdStorageWriteLeaf(0x0044, big_buffer);
		if (res < 0)
		{
			ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_TO_IDSTORAGE], res);
		}

		else
		{
			res = dcIdStorageFlush();

			if (res >= 0)
			{
				mac_thid = -1;
				sceKernelDelayThread(150000);
				vlfGuiAddEventHandler(0, 700000, OnFixMacComplete, NULL);
			}

			else
			{
				mac_thid = -1;
				ErrorReturn(1, 0, STRING[STR_SCEIDSTORAGEFLUSH_FAILED], res);
			}
		}
	}

	return sceKernelExitDeleteThread(0);
}

void FixMac()
{
	if (mac_thid >= 0)
	{
		OnBackToMainMenu(0);
		return;
	}
	
	if (sceWlanGetSwitchState() == 0)
	{
		ErrorReturn(0, 0, STRING[STR_THE_WLAN_SWITCH_IS_OFF]);
		return;
	}
	else
	{
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_FIX_MAC_ADDRESS]);
		InitProgress(0, 0, 1, 240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_RESTORING_ORIGINAL_MAC]);
		RemoveBackgroundHandler(SetBackground, NULL);

		mac_thid = sceKernelCreateThread("FixMacThread", FixMacThread, 0x18, 0x10000, 0, NULL);
		if (mac_thid >= 0)
		{
			sceKernelStartThread(mac_thid, 0, NULL);
		}
	}
}

int OnDumpIdstorageComplete(void *param)
{
	dcSetCancelMode(0);
	ResetScreen(0, 1, 0);

	SetStatus(240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_DUMP_IS_COMPLETE]);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int DumpIdstorageThread(SceSize args, void *argp)
{
	int idskey[2], in, currkey;
	dcSetCancelMode(1);
	
	SceUID fd = sceIoOpen(file_path, PSP_O_RDONLY|PSP_O_WRONLY|PSP_O_CREAT, 0777);
	if (fd < 0)
	{
		ErrorReturn(1, 0, STRING[STR_ERROR_CREATING_IDSTORAGE_BIN], fd, pspNandGetScramble());
	}
	
	for (currkey = 0; currkey < 0xFFEF; currkey++)
	{
		memset(idskey, 0, sizeof(idskey));
		idskey[0] = currkey;

		in = dcIdStorageReadLeaf(currkey, big_buffer);
		if (in != 0)
		{
			continue;
		}

		SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_SAVING_IDSTORAGE_KEY], currkey);

		SetGenericProgress(currkey, 0x140, 0);
		scePowerTick(0);

		sceIoWrite(fd, idskey, 2);
		sceIoWrite(fd, big_buffer, 512);
		sceKernelDelayThread(120000);
	}
	
	sceIoLseek(fd, 0, PSP_SEEK_SET);
	int size = sceIoLseek(fd, 0, PSP_SEEK_END);
	sceIoLseek(fd, 0, PSP_SEEK_SET);
	sceIoRead(fd, big_buffer, size);

	u8 sha1[20];
	sceKernelUtilsSha1Digest(big_buffer, size, sha1);
	sceIoWrite(fd, sha1, sizeof(sha1));

	sceIoClose(fd);
	SetProgress(100, 1);

	vlfGuiAddEventHandler(0, 700000, OnDumpIdstorageComplete, NULL);

	return sceKernelExitDeleteThread(0);
}

void DumpIdstorage(char *fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	vsprintf(file_path, fmt, list);
	va_end(list);

	if (FileExists(file_path))
	{
		if (vlfGuiMessageDialog(STRING[STR_AN_EXISTING_IDSTORAGE_WAS_FOUND], VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_YESNO | VLF_MD_INITIAL_CURSOR_NO) != 1)
		{
			OnBackToMainMenu(0);
			return;
		}
		sceIoRemove(file_path);
	}

	SetTitle("update_plugin", "tex_update_icon", STRING[STR_BACKUP_IDSTORAGE]);
	InitProgress(1, 1, 1, 80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_DUMPING_IDSTORAGE]);
	RemoveBackgroundHandler(SetBackground, NULL);

	SceUID thid = sceKernelCreateThread("DumpIdstorageThread", DumpIdstorageThread, 0x18, 0x10000, 0, NULL);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 0, NULL);
	}
}

int OnRestoreIdstorageComplete(void *param)
{
	dcSetCancelMode(0);
	ResetScreen(0, 1, 0);

	SetStatus(240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_RESTORE_IS_COMPLETE]);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int RestoreIdstorageThread(SceSize args, void *argp)
{
	int size = *(int *)argp;
	int idskey[2], currkey, res;
	dcSetCancelMode(1);
	
	SceUID fd = sceIoOpen(file_path, PSP_O_RDONLY, 0777);
	while (1)
	{
		memset(idskey, 0, sizeof(idskey));
		sceIoRead(fd, idskey, 2);
		size = sceIoRead(fd, big_buffer, 512);

		currkey = (idskey[0]) + ((idskey[1])*0x100);

		SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_RESTORING_IDSTORAGE_KEY], currkey);

		SetGenericProgress(currkey, 0x140, 0);
		scePowerTick(0);

		if (currkey >= 0x140)
		{
			SetProgress(99, 1);
		}

		sceKernelDelayThread(120000);
		if(size == 512)
		{
			res = dcIdStorageWriteLeaf(currkey, big_buffer);
			if (res < 0)
			{
				ErrorReturn(1, 0, STRING[STR_ERROR_WRITING_KEY], res, currkey);
			}
		}

		else
		{
			break;
		}
	}

	sceIoClose(fd);
	dcIdStorageFlush();
	SetProgress(100, 1);

	vlfGuiAddEventHandler(0, 700000, OnRestoreIdstorageComplete, NULL);

	return sceKernelExitDeleteThread(0);
}

void RestoreIdstorage(char *fmt, ...)
{
	int size;
	u8 sha1[20], sha1_file[20];
	va_list list;
	va_start(list, fmt);
	vsprintf(file_path, fmt, list);
	va_end(list);

	size = GetFileSize(file_path);
	if (!FileExists(file_path))
	{
		ErrorReturn(0, 0, STRING[STR_IDSTORAGE_BIN_NOT_FOUND_AT_ROOT], pspNandGetScramble());
		return;
	}

	if (size != (size / 514 * 514) + 20)
	{
		ErrorReturn(0, 0, STRING[STR_IDSTORAGE_BIN_HAS_NOT_THE_CORRECT_SIZE], pspNandGetScramble());
		return;
	}
	
	if (vlfGuiMessageDialog(STRING[STR_IDSTORAGE_RESTORE_WARNING], VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_YESNO | VLF_MD_INITIAL_CURSOR_NO) != 1)
	{
		OnBackToMainMenu(0);
		return;
	}
	
	SceUID fd = sceIoOpen(file_path, PSP_O_RDONLY, 0777);
	sceIoRead(fd, sm_buffer1, size - 20);
	sceIoRead(fd, sha1_file, 20);
	sceKernelUtilsSha1Digest(sm_buffer1, size - 20, sha1);
	sceIoLseek(fd, 0, PSP_SEEK_SET);
	if (memcmp(sha1_file, sha1, 20) != 0)
	{
		if (vlfGuiMessageDialog(STRING[STR_IDSTORAGE_BIN_HAS_BEEN_MODIFIED], VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_YESNO | VLF_MD_INITIAL_CURSOR_NO) != 1)
		{
			sceIoClose(fd);
			OnBackToMainMenu(0);
			return;
		}
	}
	
	SetTitle("update_plugin", "tex_update_icon", STRING[STR_RESTORE_IDSTORAGE]);
	InitProgress(1, 1, 1, 80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_RESTORING_IDSTORAGE]);
	RemoveBackgroundHandler(SetBackground, NULL);

	SceUID thid = sceKernelCreateThread("RestoreIdstorageThread", RestoreIdstorageThread, 0x18, 0x10000, 0, NULL);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 4, &size);
	}
}

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspipl_update.h>
#include <libpsardumper.h>
#include <pspdecrypt.h>
#include <psppower.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <vlf.h>

#include "dcman.h"
#include "trans.h"
#include "common.h"
#include "vlfutils.h"
#include "modules.h"
#include "../kpspident/main.h"

extern const char **g_messages;
extern char file_path[64];
extern int MEorPRO;
extern int BIG_BUFFER_SIZE;
extern u8 *big_buffer;
extern u8 *sm_buffer1;
extern u8 *sm_buffer2;

int pbp_ver;

#define SMALL_BUFFER_SIZE 2100000
#define NELEMS(a) sizeof(a) / sizeof(a[0])

int CreateFlash0Dirs()
{
	int i, res;
	SceIoStat stat;

	stat.st_attr = 0;
	for (i = pbp_ver > 0x023A ? 0 : 1; i < NELEMS(f0_dirs); i++)
	{
		f0_dirs[i].dir_name[3] = 'c';
		if (f0_dirs[i].attribut == 0)
		{
			stat.st_attr = 0x11; // +R
		}
		else
		{
			stat.st_attr = 0x37; // +A +R +H +S
		}

		res = sceIoMkdir(f0_dirs[i].dir_name, 0777);
		if (res < 0 && res != 0x80010011)
		{
			return res;
		}

		sceIoChstat(f0_dirs[i].dir_name, &stat, 6);
	}

	return 0;
}

int CreateFlash1Dirs()
{
	int i, res;

	for (i = 0; i < NELEMS(f1_dirs); i++)
	{
		f1_dirs[i].dir_name[3] = 'c';
		res = sceIoMkdir(f1_dirs[i].dir_name, 0777);
		if (res < 0 && res != 0x80010011)
		{
			return res;
		}
	}

	return 0;
}

static int FindTablePath(char *table, int table_size, char *number, char *szOut)
{
	int i, j, k;

	for (i = 0; i < table_size - 5; i++)
	{
		if (strncmp(number, table + i, 5) == 0)
		{
			for (j = 0, k = 0; ; j++, k++)
			{
				if (table[i + j + 6] < 0x20)
				{
					szOut[k] = 0;
					break;
				}

				if (table[i + 5] == '|' && !strncmp(table + i + 6, "flash", 5) && j == 6)
				{
					szOut[6] = ':';
					szOut[7] = '/';
					k++;
				}

				else if (table[i + 5] == '|' && !strncmp(table + i + 6, "ipl", 3) && j == 3)
				{
					szOut[3] = ':';
					szOut[4] = '/';
					k++;
				}

				else
				{				
					szOut[k] = table[i + j + 6];
				}
			}

			return 1;
		}
	}

	return 0;
}

static int is5Dnum(char *str)
{
	int len = strlen(str);

	if (len != 5)
	{
		return 0;
	}

	int i;

	for (i = 0; i < len; i++)
	{
		if (str[i] < '0' || str[i] > '9')
		{
			return 0;
		}
	}

	return 1;
}

void DecryptPRX(u8 *buf, int size, u8 a, u8 b, u8 c)
{
	u8 x = a;
	u8 y = b;
	u8 z = c;
	u8 t;
	int i;
	
	if (size > 0)
	{
		for (i = 0; i < size; i++)
		{
			x += y;
			buf[i] ^= x;
			
			t = y % (z + 1);
			y = z;
			z = x;
			x = t;
		}
	}
}

int ExtractPRX(u8 *buffer, int size, int ver, int xor)
{
	int i, j;
	u8 *modptr[16];
	int modlen[16];
	char outname[128];
	
	struct modules {
		char *modname;
		char *filename;
	} modules[] = {
		{ "sceNAND_Updater_Driver", "emc_sm_updater.prx" },
		{ "sceLFatFs_Updater_Driver", "lfatfs_updater.prx" },
		{ "sceLflashFatfmtUpdater", "lflash_fatfmt_updater.prx" },
	};

	for (i = 0; i < size; i += 4)
	{
		if (*(u32 *)(buffer + i) == (0x5053507E ^ xor))
		{
			modptr[0] = &buffer[i];
			modlen[0] = (*(u32 *)(buffer + i + 0x2C)) ^ xor;

			if (ver < 0x023A)
			{
				int l;

				for (l = 0; l < modlen[0]; l += 4)
				{
					*(u32 *)(modptr[0] + l) ^= xor;
				}
			}

			for (j = 0; j < NELEMS(modules); j++)
			{
				sprintf(outname, "flash0:/kd/modules/%i/%s", pbp_ver, modules[j].filename);
				if(strcmp((char *)(modptr[0] + 0x0A), modules[j].modname) == 0)
				{
					int d = pspDecryptPRX(modptr[0], sm_buffer1, modlen[0]);
					if(d > 0)
					{
						u8 *buff_mod = sm_buffer1;
						int s = d;

						if((*(u16 *)&sm_buffer1[0] == 0x8B1F) || memcmp(sm_buffer1, "2RLZ", 4) == 0 || memcmp(sm_buffer1, "KL4E", 4) == 0)
						{
							int c = pspDecompress(sm_buffer1, sm_buffer2, SMALL_BUFFER_SIZE);
							if(c > 0)
							{
								buff_mod = sm_buffer2;
								s = c;
							}
						}

						if(WriteFile(outname, buff_mod, s) != s)
						{
							ErrorReturn(1, 1, STRING[STR_ERROR_EXTRACTING_MODULE], modules[j].filename);
						}
					}
				}
			}
		}
	}

	return 0;
}

int LoadUpdaterModules(int ofw)
{
	int i;
	char outname[128];
	int pos[] = {
		0x0000E200, 0x0000E2C0, 0x0000E580, 0x0000E5C0, 0x0000E900, 0x0001CC00, 0x0001CCC0,
		0x0001CF00, 0x0001CF40, 0x0001D280, 0x00021880, 0x00030200
	};
	int xor[] = {
		0x00000000, 0x25252525, 0x34343434, 0x43434343, 0x55555555, 0x75757575, 0x77777777, 0x93939393,
		0xF1F1F1F1, 0xDEDEDEDE
	};
	
	sprintf(outname, "flash0:/kd/modules/%i", pbp_ver);
	if (!FileExists("%s/emc_sm_updater.prx", outname) || !FileExists("%s/lfatfs_updater.prx", outname) || !FileExists("%s/lflash_fatfmt_updater.prx", outname))
	{
		SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_EXTRACTING_UPDATER_MODULES]);

		if (ReadFile(file_path, 0, big_buffer, 40) < 0)
		{
			return -1;
		}

		int dpsp_o = *(u32 *)&big_buffer[0x20];
		int psar_o = *(u32 *)&big_buffer[0x24];
		int dpsp_l = psar_o - dpsp_o;
		
		if (ReadFile(file_path, dpsp_o, big_buffer, dpsp_l) < 0)
		{
			return -1;
		}

		int r = pspDecryptPRX(big_buffer, big_buffer, dpsp_l);
		if (r < 0)
		{
			ErrorReturn(1, 1, STRING[STR_CANNOT_DECRYPT_DATA_PSP]);
		}

		if (pbp_ver > 0x023A)
		{
			for (i = 0; i < NELEMS(pos); i++)
			{
				memcpy(sm_buffer1, big_buffer + pos[i], SMALL_BUFFER_SIZE);

				if (pbp_ver > 0x023A && pbp_ver <= 0x026C)
				{
					DecryptPRX(sm_buffer1, SMALL_BUFFER_SIZE, 0x0F, 0x01, 0x0D);
				}
				else if (pbp_ver > 0x026C && pbp_ver <= 0x027F)
				{
					DecryptPRX(sm_buffer1, SMALL_BUFFER_SIZE, 0x19, 0x01, 0x0D);
				}
				else if (pbp_ver > 0x027F && pbp_ver <= 0x0294)
				{
					DecryptPRX(sm_buffer1, SMALL_BUFFER_SIZE, 0x0B, 0x07, 0x06);
				}

				if (ExtractPRX(sm_buffer1, SMALL_BUFFER_SIZE, pbp_ver, 0) < 0)
				{
					return -1;
				}

			}

			if (ExtractPRX(big_buffer, r, pbp_ver, 0) < 0)
			{
				return -1;
			}
		}

		else if (pbp_ver < 0x023A)
		{
			for (i = 0; i < NELEMS(xor); i++)
			{
				if (ExtractPRX(big_buffer, r, pbp_ver, xor[i]) < 0)
				{
					return -1;
				}
			}
		}
	}

	/*****************************************************************************************/

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_LOADING_UPDATER_MODULE]);

	dcPatchModuleString("DesCemManager", "sceNAND_Updater_Driver", "sceNand_Updater_Driver"); // patch dcman.prx

	sprintf(outname, "flash0:/kd/modules/%i/emc_sm_updater.prx", pbp_ver);
	SceUID mod = sceKernelLoadModule(outname, 0, NULL);
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

	// Patchs OK ^_^
	if (pbp_ver < 600)
	{
		u32 offset = 0x0000;

		if(pbp_ver >= 0x0173 && pbp_ver <= 0x017C)
		{
			offset = 0x0E4E;
		}
		else if(pbp_ver >= 0x0186 && pbp_ver <= 0x0189)
		{
			offset = 0x0E7E;
		}
		else if(pbp_ver >= 0x018B && pbp_ver <= 0x022B)
		{
			offset = 0x0D7E;
		}

		if (!ofw && pspKernelGetModel() == 0)
		{
			dcPatchModule("sceNAND_Updater_Driver", 1, offset, 0xAC60);
		}
		else
		{
			dcPatchModule("sceNAND_Updater_Driver", 1, offset, 0xAC64);
		}
	}

	/*****************************************************************************************/

	sprintf(outname, "flash0:/kd/modules/%i/lfatfs_updater.prx", pbp_ver);
	mod = sceKernelLoadModule(outname, 0, NULL);
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

	/*****************************************************************************************/

	sprintf(outname, "flash0:/kd/modules/%i/lflash_fatfmt_updater.prx", pbp_ver);
	mod = sceKernelLoadModule(outname, 0, NULL);
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

	/*****************************************************************************************/

	mod = sceKernelLoadModule("flash0:/kd/ipl_update.prx", 0, NULL);
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

	/*****************************************************************************************/

	return 0;
}

int VerifyChecksum()
{
	SceUID fd;
	u8 digest[16];
	SceKernelUtilsMd5Context ctx;
	int size, i;

	if((fd = sceIoOpen(file_path , PSP_O_RDONLY, 0777)) < 0)
	{
		return -1;
	}

	sceKernelUtilsMd5BlockInit(&ctx);

	while((size = sceIoRead(fd , big_buffer , 0x2000)) > 0)
	{
		sceKernelUtilsMd5BlockUpdate(&ctx, big_buffer , size);
	}

	sceIoClose(fd);
	sceKernelUtilsMd5BlockResult(&ctx, digest);	
	
	for (i = 0; i < NELEMS(eboots); i++)
	{
		if (eboots[i].version == pbp_ver)
		{
			if (memcmp(eboots[i].md5, digest, 16))
			{
				return -1;
			}
		}
	}

	return 0;
}

int OnInstallComplete(void *param)
{
	int ofw = *(int *)param;

	dcSetCancelMode(0);
	ResetScreen(0, 0, 0);

	if (ofw && pspKernelGetModel() == 1)
	{
		SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_INSTALL_COMPLETE_SHUTDOWN_REQUIRED]);
		AddShutdownRebootBD(1);
	}
	else
	{
		SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_INSTALL_COMPLETE_SHUTDOWN_OR_REBOOT_REQUIRED]);
		AddShutdownRebootBD(0);
	}

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int InstallThread(SceSize args, void *argp)
{
	int ofw = *(int *)argp;
	int i, ipl_size;
	char *argv[2];
	int res;
	SceUID fd;
	int size;
	u8 pbp_header[40];
	int error = 0, psar_pos = 0, cur_mod = 0;
	int model = pspKernelGetModel();
	int psarVersion = 0;
	int table_mode = 0;
	int comtable_size = 0, _1gtable_size = 0, _2gtable_size = 0;
	char com_table[0x4000], _1g_table[0x4000], _2g_table[0x4000];
	dcSetCancelMode(1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_VERIFYING_PBP_VER], pbp_ver);
	if (VerifyChecksum() < 0)
	{
		ErrorReturn(1, 1, STRING[STR_INVALID_OR_CORRUPTED_PBP], pbp_ver);
	}
	sceKernelDelayThread(1200000);
	SetProgress(1, 1);

	if (LoadUpdaterModules(ofw) < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ERROR_LOADING_UPDATER_MODULE]);
	}
	sceKernelDelayThread(1200000);
	SetProgress(2, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_UNASSIGN_FLASH]);
	UnassignFlash();
	sceKernelDelayThread(1200000);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_FORMAT_FLASH_PARTITION], 0);
	argv[0] = "fatfmt";
	argv[1] = "lflach0:0,0";

	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		ErrorReturn(1, 1, STRING[STR_FORMAT_FLASH_FAILED], 0, res);
	}

	sceKernelDelayThread(1200000);
	SetProgress(3, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_FORMAT_FLASH_PARTITION], 1);

	argv[1] = "lflach0:0,1";
	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		ErrorReturn(1, 1, STRING[STR_FORMAT_FLASH_FAILED], 1, res);
	}

	sceKernelDelayThread(1200000);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_FORMAT_FLASH_PARTITION], 2);

	argv[1] = "lflach0:0,2";
	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		ErrorReturn(1, 1, STRING[STR_FORMAT_FLASH_FAILED], 2, res);
	}

	sceKernelDelayThread(1200000);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_FORMAT_FLASH_PARTITION], 3);

	argv[1] = "lflach0:0,3";
	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		ErrorReturn(1, 1, STRING[STR_FORMAT_FLASH_FAILED], 3, res);
	}

	sceKernelDelayThread(1200000);
	SetProgress(4, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_ASSIGN_FLASH]);
	AssignFlash();
	sceKernelDelayThread(1200000);
	SetProgress(5, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_CREATING_FLASH_DIRECTORIES]);

	if (CreateFlash0Dirs() < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ERROR_CREATING_FLASH_DIRECTORIES], 0);
	}

	sceKernelDelayThread(1200000);
	SetProgress(6, 1);

	fd = sceIoOpen(file_path, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ERROR_OPENING_PBP], pbp_ver, fd);
	}

	size = sceIoLseek32(fd, 0, PSP_SEEK_END);
	sceIoLseek32(fd, 0, PSP_SEEK_SET);

	sceIoRead(fd, pbp_header, sizeof(pbp_header));
	sceIoLseek32(fd, *(u32 *)&pbp_header[0x24], PSP_SEEK_SET);

	size = size - *(u32 *)&pbp_header[0x24];

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_LOADING_PSAR]);
	if (sceIoRead(fd, big_buffer, BIG_BUFFER_SIZE) <= 0)
	{
		ErrorReturn(1, 1, STRING[STR_ERROR_READING_PBP], pbp_ver);
	}

	sceKernelDelayThread(10000);

	if (pspPSARInit(big_buffer, sm_buffer1, sm_buffer2) < 0)
	{
		ErrorReturn(1, 1, STRING[STR_PSPPSARINIT_FAILED]);
	}

	psarVersion = big_buffer[4];
	char *version = GetString((char *)sm_buffer1 + 0x10, ',');

	if (memcmp(version, "3.8", 3) == 0 || memcmp(version, "3.9", 3) == 0)
	{
		table_mode = 1;
	}
	else if (memcmp(version, "4.", 2) == 0)
	{
		table_mode = 2;
	}
	else if (memcmp(version, "5.", 2) == 0)
	{
		table_mode = 3;
	}
    else if ((memcmp(version, "6.3", 3) == 0) && (psarVersion == 5))
	{
		table_mode = 4;
	}
    else if ((memcmp(version, "6.", 2) == 0) && (psarVersion == 5))
	{
		table_mode = 5;
	}
    else if (memcmp(version, "6.", 2) == 0)
	{
		table_mode = 4;
	}
	else
	{
		table_mode = 0;
	}

	while (1)
	{
		char name[128];
		int cbExpanded;
		int pos;
		int signcheck;

		int res = pspPSARGetNextFile(big_buffer, size, sm_buffer1, sm_buffer2, name, &cbExpanded, &pos, &signcheck);
		if (res < 0)
		{
			if (error)			
			{
				ErrorReturn(1, 1, STRING[STR_PSAR_DECODE_ERROR], pos);
			}

			int dpos = pos - psar_pos;
			psar_pos = pos;

			error = 1;
			memmove(big_buffer, big_buffer + dpos, BIG_BUFFER_SIZE-dpos);

			if (sceIoRead(fd, big_buffer+(BIG_BUFFER_SIZE - dpos), dpos) <= 0)
			{
				ErrorReturn(1, 1, STRING[STR_ERROR_READING_PBP], pbp_ver);
			}

			pspPSARSetBufferPosition(psar_pos);

			continue;
		}
		else if (res == 0)
		{
			break;
		}

		if (is5Dnum(name))
		{
			if (strcmp(name, "00001") != 0 && strcmp(name, "00002") != 0)
			{
				int found = 0;

				if (model == 0 && _1gtable_size > 0)
				{
					found = FindTablePath(_1g_table, _1gtable_size, name, name);
				}

				if (!found && model == 1 && _2gtable_size > 0)
				{
					found = FindTablePath(_2g_table, _2gtable_size, name, name);
				}

				if (!found)
				{
					error = 0;
					continue;
				}
			}
		}
		else if (!strncmp(name, "com:", 4) && comtable_size > 0)
		{
			if(!FindTablePath(com_table, comtable_size, name+4, name))
			{
				error = 0;
				continue;
			}
		}
		else if (model == 0 && !strncmp(name, "01g:", 4) && _1gtable_size > 0)
		{
			if(!FindTablePath(_1g_table, _1gtable_size, name+4, name))
			{
				ErrorReturn(1, 1, STRING[STR_CANNOT_FIND_PATH_OF], name);
			}
		}
		else if (model == 1 && !strncmp(name, "02g:", 4) && _2gtable_size > 0)
		{
			if(!FindTablePath(_2g_table, _2gtable_size, name+4, name))
			{
				ErrorReturn(1, 1, STRING[STR_CANNOT_FIND_PATH_OF], name);
			}
		}

		if (cbExpanded > 0)
		{
			if (!strncmp(name, "flash0:/", 8))
			{
				SceIoStat stat;
				name[3] = 'c';

				SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_FLASHING_FILE], GetString(name, '/'));

				if (signcheck)
				{
					pspSignCheck(sm_buffer2);
				}

				res = WriteFile(name, sm_buffer2, cbExpanded);
				if (res <= 0)
				{
					name[3] = 's';
					ErrorReturn(1, 1, STRING[STR_ERROR_FLASHING_FILE], name);
				}

				for (i = 0; i < NELEMS(attribs); i++)
				{
					attribs[i].path[3] = 'c';
					if (attribs[i].attr && !strncmp(name, attribs[i].path, strlen(attribs[i].path)))
					{
						// +A +R +H +S
						stat.st_attr = 0x27;
						sceIoChstat(name, &stat, 6);
					}
					else if (!attribs[i].attr && !strncmp(name, attribs[i].path, strlen(attribs[i].path)))
					{
						// +A +R
						stat.st_attr = 0x21;
						sceIoChstat(name, &stat, 6);
					}
				}
			}
			else if (!strncmp(name, "ipl:", 4))
			{
				u8 *ipl;
				SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_FLASHING_FILE], model == 0 ? "ipl_01g.bin" : "ipl_02g.bin");

				if (model == 1)
				{
					cbExpanded = pspDecryptPRX(sm_buffer2, sm_buffer1+16384, cbExpanded);
					if (cbExpanded <= 0)
					{
						ErrorReturn(1, 1, STRING[STR_CANNOT_DECRYPT_PSP_SLIM_IPL]);
					}

					for (i = 0; i < NELEMS(CFW_IPLS); i++)
					{
						if (CFW_IPLS[i].ipl_version == pbp_ver && CFW_IPLS[i].ipl_model == 1 && CFW_IPLS[i].ipl_mepro == MEorPRO)
						{
							memcpy(sm_buffer1, CFW_IPLS[i].ipl_buffer, *CFW_IPLS[i].ipl_size);
						}
					}
				}
				else
				{
					memcpy(sm_buffer1 + 16384, sm_buffer2, cbExpanded);

					for (i = 0; i < NELEMS(CFW_IPLS); i++)
					{
						if (CFW_IPLS[i].ipl_version == pbp_ver && CFW_IPLS[i].ipl_model == 0 && CFW_IPLS[i].ipl_mepro == MEorPRO)
						{
							memcpy(sm_buffer1, CFW_IPLS[i].ipl_buffer, *CFW_IPLS[i].ipl_size);
						}
					}
				}

				ipl = sm_buffer1;
				ipl_size = cbExpanded + 16384;

				if (ofw)
				{
					ipl += 16384;
					ipl_size -= 16384;
				}

				dcPatchModuleString("IoPrivileged", "IoPrivileged", "IoPrivileged");

				if (pspIplUpdateClearIpl() < 0)
				{
					ErrorReturn(1, 1, STRING[STR_ERROR_IN_FUNCTION], "pspIplUpdateClearIpl");
				}

				if (pspIplUpdateSetIpl(ipl, ipl_size) < 0)
				{
					ErrorReturn(1, 1, STRING[STR_ERROR_IN_FUNCTION], "pspIplUpdateSetIpl");
				}

				sceKernelDelayThread(100000);
			}
			else if (!strcmp(name, "com:00000"))
			{
//				SetStatus(80, 100, VLF_ALIGNMENT_LEFT, "Getting common table...");
				comtable_size = pspDecryptTable(sm_buffer2, sm_buffer1, cbExpanded, table_mode);

				if (comtable_size <= 0)
				{
					ErrorReturn(1, 1, STRING[STR_CANNOT_DECRYPT_COMMON_TABLE]);
				}

				memcpy(com_table, sm_buffer2, comtable_size);
			}
			else if (!strcmp(name, "01g:00000") || !strcmp(name, "00001"))
			{
//				SetStatus(80, 100, VLF_ALIGNMENT_LEFT, "Getting 0%ig table...", 1);
				_1gtable_size = pspDecryptTable(sm_buffer2, sm_buffer1, cbExpanded, table_mode);

				if (_1gtable_size <= 0)
				{
					ErrorReturn(1, 1, STRING[STR_CANNOT_DECRYPT_GENERATION_TABLE], 1);
				}

				memcpy(_1g_table, sm_buffer2, _1gtable_size);
			}
			else if (!strcmp(name, "02g:00000") || !strcmp(name, "00002"))
			{
//				SetStatus(80, 100, VLF_ALIGNMENT_LEFT, "Getting 0%ig table...", 2);
				_2gtable_size = pspDecryptTable(sm_buffer2, sm_buffer1, cbExpanded, table_mode);

				if (_2gtable_size <= 0)
				{
					ErrorReturn(1, 1, STRING[STR_CANNOT_DECRYPT_GENERATION_TABLE], 2);
				}

				memcpy(_2g_table, sm_buffer2, _2gtable_size);
			}
		}

		SetInstallProgress(pos, size, 0, ofw);
		scePowerTick(0);
		error = 0;
	}

	if (!ofw)
	{
		SetProgress(96, 1);
		for (i = 0; i < NELEMS(CFW_Modules); i++)
		{
			if (CFW_Modules[i].files_version == pbp_ver && CFW_Modules[i].files_mepro == MEorPRO && (CFW_Modules[i].files_model == model || CFW_Modules[i].files_model == 2))
			{
				CFW_Modules[i].files_path[3] = 'c';
				SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_FLASHING_FILE], GetString(CFW_Modules[i].files_path, '/'));
				
				res = WriteFile(CFW_Modules[i].files_path, CFW_Modules[i].files_buffer, *CFW_Modules[i].files_size);
				if (res < 0)
				{
					CFW_Modules[i].files_path[3] = 's';
					ErrorReturn(1, 1, STRING[STR_ERROR_FLASHING_FILE], res, CFW_Modules[i].files_path);
				}
				
				if (cur_mod == 4)
				{
					SetProgress(97, 1);
				}
				else if (cur_mod == 8)
				{
					SetProgress(98, 1);
				}

				cur_mod++;
				sceKernelDelayThread(100000);
			}
		}

		cur_mod = 0;
	}

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_RESTORING_REGISTRY]);
	SetProgress(99, 1);
	sceKernelDelayThread(500000);

	if (CreateFlash1Dirs() < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ERROR_CREATING_FLASH_DIRECTORIES], 1);
	}

	if (!ofw)
	{
		for (i = 0; i < NELEMS(CFW_Modules); i++)
		{
			if (CFW_Modules[i].files_version == pbp_ver && CFW_Modules[i].files_mepro == MEorPRO && CFW_Modules[i].files_model == 3)
			{
				CFW_Modules[i].files_path[3] = 'c';
				res = WriteFile(CFW_Modules[i].files_path, CFW_Modules[i].files_buffer, *CFW_Modules[i].files_size);
				if (res < 0)
				{
					CFW_Modules[i].files_path[3] = 's';
					ErrorReturn(1, 1, STRING[STR_ERROR_FLASHING_FILE], res, CFW_Modules[i].files_path);
				}
			}
		}
	}

	res = ReadFile("flash2:/registry/act.dat", 0, sm_buffer1, SMALL_BUFFER_SIZE);
	if (res > 0)
	{
		WriteFile("flach2:/act.dat", sm_buffer1, res);		
	}

	SetProgress(100, 1);
	vlfGuiAddEventHandler(0, 700000, OnInstallComplete, &ofw);

	return sceKernelExitDeleteThread(0);
}

void Install(int ofw, int sel_ver)
{
	int i;
	pbp_ver = sel_ver;

	vlfGuiSetPageControlEnable(0);
	
	if (!pspKernelGetModel() && !ofw && pbp_ver == 0x027D)
	{
		ErrorReturn(0, 1, STRING[STR_CFW_NOT_SUPPORTED_BY_PSP]);
		return;
	}

	sprintf(file_path, "ms0:/%i.PBP", pbp_ver);
	if (!FileExists(file_path))
	{
		ErrorReturn(0, 1, STRING[STR_PBP_NOT_FOUND_AT_ROOT], pbp_ver);
		return;
	}

	for (i = 0; i < NELEMS(eboots); i++)
	{
		if (eboots[i].version == pbp_ver)
		{
			if (GetFileSize(file_path) != eboots[i].size)
			{
				ErrorReturn(0, 1, STRING[STR_INVALID_PBP_FILE_SIZE], pbp_ver);
				return;
			}
		}
	}

	SetTitle("update_plugin", "tex_update_icon", STRING[STR_FLASHING_FW_VERSION], ofw ? STRING[STR_OFFICIAL_FIRMWARE] : STRING[STR_CUSTOM_FIRMWARE], pbp_ver / 100.0f);
	InitProgress(1, 1, 1, 80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_VERIFYING_PBP_VER], pbp_ver);
	RemoveBackgroundHandler(SetBackground, NULL);

	SceUID thid = sceKernelCreateThread("InstallThread", InstallThread, 0x18, 0x10000, 0, NULL);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 4, &ofw);
	}
}

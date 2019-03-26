#include <pspsdk.h>
#include <pspkernel.h>
#include <libpsardumper.h>
#include <pspdecrypt.h>
#include <psppower.h>
#include <pspctrl.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <vlf.h>

#include "trans.h"
#include "common.h"
#include "vlfutils.h"
#include "modules.h"
#include "../kpspident/main.h"

extern const char **g_messages;
extern char file_path[64];
extern int BIG_BUFFER_SIZE;
extern u8 *big_buffer;
extern u8 *sm_buffer1;
extern u8 *sm_buffer2;

int pbp_ver;

#define SMALL_BUFFER_SIZE 2100000
#define NELEMS(a) sizeof(a) / sizeof(a[0])

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

int ExtractPRX(u8 *buffer, int size, int ver, int xor, char *modpath)
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
				sprintf(outname, "%s/%s", modpath, modules[j].filename);
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

int LoadUpdaterModules()
{
	int i;

	int pos[] = {
		0x0000E200, 0x0000E2C0, 0x0000E580, 0x0000E5C0, 0x0000E900, 0x0001CC00, 0x0001CCC0,
		0x0001CF00, 0x0001CF40, 0x0001D280, 0x00021880, 0x00030200
	};
	int xor[] = {
		0x00000000, 0x25252525, 0x34343434, 0x43434343, 0x55555555, 0x75757575, 0x77777777,
		0x93939393, 0xF1F1F1F1, 0xDEDEDEDE
	};

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

			if (ExtractPRX(sm_buffer1, SMALL_BUFFER_SIZE, pbp_ver, 0, "ms0:/TM/660/kd") < 0)
			{
				return -1;
			}
		}

		if (ExtractPRX(big_buffer, r, pbp_ver, 0, "ms0:/TM/660/kd") < 0)
		{
			return -1;
		}
	}

	else if (pbp_ver < 0x023A)
	{
		for (i = 0; i < NELEMS(xor); i++)
		{
			if (ExtractPRX(big_buffer, r, pbp_ver, xor[i], "ms0:/TM/500/kd") < 0)
			{
				return -1;
			}
		}
	}

	return 0;
}

int CreateDirectories(int ddc)
{
	int i, res;

	for (i = 0; i < (pbp_ver < 0x023A ? (NELEMS(DC8_dirs) - 1) : NELEMS(DC8_dirs)); i++)
	{
		if (ddc == 0 && i != 0)
		{
			DC8_dirs[i].dir_name[0x08] = '5';
			DC8_dirs[i].dir_name[0x09] = '0';
			DC8_dirs[i].dir_name[0x0A] = '0';
		}
		else if (ddc == 1 && i != 0)
		{
			DC8_dirs[i].dir_name[0x08] = '6';
			DC8_dirs[i].dir_name[0x09] = '6';
			DC8_dirs[i].dir_name[0x0A] = '0';
		}

		res = sceIoMkdir(DC8_dirs[i].dir_name, 0777);
		if (res < 0 && res != 0x80010011)
		{
			return res;
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

static int FindTablePath(char *table, int table_size, char *number, char *szOut)
{
	int i, j, k;

	for (i = 0; i < table_size-5; i++)
	{
		if (strncmp(number, table+i, 5) == 0)
		{
			for (j = 0, k = 0; ; j++, k++)
			{
				if (table[i+j+6] < 0x20)
				{
					szOut[k] = 0;
					break;
				}

				if (table[i+5] == '|' && !strncmp(table+i+6, "flash", 5) && j == 6)
				{
					szOut[6] = ':';
					szOut[7] = '/';
					k++;
				}

				else if (table[i+5] == '|' && !strncmp(table+i+6, "ipl", 3) && j == 3)
				{
					szOut[3] = ':';
					szOut[4] = '/';
					k++;
				}

				else
				{				
					szOut[k] = table[i+j+6];
				}
			}

			return 1;
		}
	}

	return 0;
}

int OnInstallComplete(void *param)
{
	scePowerUnlock(0);
	ResetScreen(0, 1, 0);
	SetStatus(240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_INSTALL_COMPLETE]);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int InstallThread(SceSize args, void *argp)
{
	SceUID fd;
	int ddc = *(int *)argp;
	int i, res, size, table_mode = 0, psarVersion = 0, error = 0, psar_pos = 0, cur_mod = 0, btn;
	u8 pbp_header[40];
	int comtable_size = 0, _1gtable_size = 0, _2gtable_size = 0;
	char com_table[0x4000], _1g_table[0x4000], _2g_table[0x4000];
	char key[128], data[128], buffer[1024], message[256];
	scePowerLock(0);

	
	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_VERIFYING_PBP_VER], pbp_ver);
	if (VerifyChecksum() < 0)
	{
		ErrorReturn(1, 1, STRING[STR_INVALID_OR_CORRUPTED_PBP], pbp_ver);
	}
	sceKernelDelayThread(1200000);
	SetProgress(1, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_CREATING_DIRECTORIES]);
	if (CreateDirectories(ddc) < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ERROR_CREATING_DIRECTORIES]);
	}
	sceKernelDelayThread(1200000);
	SetProgress(2, 1);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_EXTRACTING_UPDATER_MODULES]);
	if (LoadUpdaterModules() < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ERROR_EXTRACTING_MODULE]);
	}
	sceKernelDelayThread(1200000);
	SetProgress(3, 1);

	fd = sceIoOpen(file_path, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ERROR_OPENING_PBP], fd, pbp_ver);
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

			int dpos = pos-psar_pos;
			psar_pos = pos;
			error = 1;
			
			memmove(big_buffer, big_buffer+dpos, BIG_BUFFER_SIZE-dpos);

			if (sceIoRead(fd, big_buffer+(BIG_BUFFER_SIZE-dpos), dpos) <= 0)
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

				if (_1gtable_size > 0)
				{
					found = FindTablePath(_1g_table, _1gtable_size, name, name);
				}

				if (!found && _2gtable_size > 0)
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
		else if (!strncmp(name, "01g:", 4) && _1gtable_size > 0)
		{
			if(!FindTablePath(_1g_table, _1gtable_size, name+4, name))
			{
				ErrorReturn(1, 1, STRING[STR_CANNOT_FIND_PATH_OF], name);
			}
		}
		else if (!strncmp(name, "02g:", 4) && _2gtable_size > 0)
		{
			if(!FindTablePath(_2g_table, _2gtable_size, name+4, name))
			{
				ErrorReturn(1, 1, STRING[STR_CANNOT_FIND_PATH_OF], name);
			}
		}
		
		if (cbExpanded > 0)
		{
			char OutFile[128];
			
			if(!strncmp(name, "flash0:/", 8))
			{
				sprintf(OutFile, "ms0:/TM/%i/%s", pbp_ver, name+8);
				SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_INSTALLING_FILE], GetString(OutFile, '/'));
				
				res = WriteFile(OutFile, sm_buffer2, cbExpanded);
				if (res <= 0)
				{
					ErrorReturn(1, 1, STRING[STR_ERROR_INSTALLING_FILE], GetString(OutFile, '/'));
				}
			}
			else if(!strncmp(name, "flash1:/", 8))
			{
				sprintf(OutFile, "ms0:/TM/%i/%s", pbp_ver, name+8);
				SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_INSTALLING_FILE], GetString(OutFile, '/'));

				res = WriteFile(OutFile, sm_buffer2, cbExpanded);
				if (res <= 0)
				{
					ErrorReturn(1, 1, STRING[STR_ERROR_INSTALLING_FILE], GetString(OutFile, '/'));
				}
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

		SetInstallProgress(pos, size, 0, 0);
		scePowerTick(0);
		error = 0;
	}
	
	for (i = 0; i < NELEMS(DC8_Files); i++)
	{
		if (DC8_Files[i].version == ddc)
		{
			SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_INSTALLING_FILE], GetString(DC8_Files[i].filepath, '/'));

			res = WriteFile(DC8_Files[i].filepath, DC8_Files[i].filebuffer, *DC8_Files[i].filesize);
			if (res < 0)
			{
				ErrorReturn(1, 1, STRING[STR_ERROR_INSTALLING_FILE], GetString(DC8_Files[i].filepath, '/'));
			}
				
			if (cur_mod == 8)
			{
				SetProgress(96, 1);
			}
			else if (cur_mod == 16)
			{
				SetProgress(97, 1);
			}
			else if (cur_mod == 24)
			{
				SetProgress(98, 1);
			}

			cur_mod++;
			sceKernelDelayThread(100000);
		}
	}
	cur_mod = 0;

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_INJECT_IPL]);

	if(size_tm_ipl_DC500 != (size_tm_ipl_DC500 / 4096) * 4096)
	{
		size_tm_ipl_DC500 = (size_tm_ipl_DC500 + 4096) &0xFFFFFE00;
	}

	fd = sceIoOpen("msstor:", PSP_O_WRONLY, 0777);
	if(fd < 1)
	{
		ErrorReturn(1, 1, STRING[STR_ERROR_OPENING_MSSTOR], fd);
	}

	sceIoLseek(fd, 0x2000, 0);
	sceIoWrite(fd, tm_ipl_DC500, size_tm_ipl_DC500);
	sceIoClose(fd);
	SetProgress(99, 1);
	sceKernelDelayThread(500000);

	SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_SAVING_REGISTERY]);
	res = ReadFile("flash1:/registry/system.dreg", 0, sm_buffer1, SMALL_BUFFER_SIZE);
	if (res > 0)
	{
		res = WriteFile(ddc == 1 ? "ms0:/TM/660/registry/system.dreg" : "ms0:/TM/500/registry/system.dreg", sm_buffer1, res);
		if(res < 0)
		{
			SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_ERROR_INSTALLING_FILE], "system.dreg");
			sceKernelDelayThread(1000000);
		}
	}

	res = ReadFile("flash1:/registry/system.ireg", 0, sm_buffer1, SMALL_BUFFER_SIZE);
	if (res > 0)
	{
		res = WriteFile(ddc == 1 ? "ms0:/TM/660/registry/system.ireg" : "ms0:/TM/500/registry/system.ireg", sm_buffer1, res);
		if(res < 0)
		{
			SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_ERROR_INSTALLING_FILE], "system.ireg");
			sceKernelDelayThread(1000000);
		}
	}

	res = ReadFile("flash2:/act.dat", 0, sm_buffer1, SMALL_BUFFER_SIZE);
	if (res > 0)
	{
		res = WriteFile(ddc == 1 ? "ms0:/TM/660/registry/act.dat" : "ms0:/TM/500/registry/act.dat", sm_buffer1, res);
		if(res < 0)
		{
			SetStatus(80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_ERROR_INSTALLING_FILE], "act.dat");
			sceKernelDelayThread(1000000);
		}
	}

	if(pbp_ver == 0x01F4)
	{
		WriteFile("ms0:/TM/500/lang/vunbricker_en.txt", vunbricker_en, size_vunbricker_en);
	}

	SetProgress(100, 1);
	sceKernelDelayThread(1000000);

	ResetScreen(0, 0, 0);
	sprintf(message, STRING[STR_PLEASE_HOLD_THE_KEY]);
	SetStatus(240, 110, VLF_ALIGNMENT_CENTER, "%s", message);
	AddWaitIcon();

	while(1)
	{
		btn = pspGetKeyPress(1, 1000);
		sceKernelDelayThread(1200000);
		if(btn == pspGetKeyPress(1, 1000))
		{
			break;
		}
	}

	memset(key, 0, sizeof(key));
	if(btn &PSP_CTRL_SELECT){strcat(key, "SELECT+");}
	if(btn &PSP_CTRL_START){strcat(key, "START+");}
	if(btn &PSP_CTRL_UP){strcat(key, "UP+");}
	if(btn &PSP_CTRL_RIGHT){strcat(key, "RIGHT+");}
	if(btn &PSP_CTRL_DOWN){strcat(key, "DOWN+");}
	if(btn &PSP_CTRL_LEFT){strcat(key, "LEFT+");}
	if(btn &PSP_CTRL_LTRIGGER){strcat(key, "L+");}
	if(btn &PSP_CTRL_RTRIGGER){strcat(key, "R+");}
	if(btn &PSP_CTRL_TRIANGLE){strcat(key, "TRIANGLE+");}
	if(btn &PSP_CTRL_CIRCLE){strcat(key, "CIRCLE+");}
	if(btn &PSP_CTRL_CROSS){strcat(key, "CROSS+");}
	if(btn &PSP_CTRL_SQUARE){strcat(key, "SQUARE+");}
	if(btn &PSP_CTRL_HOME){strcat(key, "HOME+");}
	if(btn &PSP_CTRL_NOTE){strcat(key, "NOTE+");}
	if(btn &PSP_CTRL_SCREEN){strcat(key, "SCREEN+");}
	if(btn &PSP_CTRL_VOLUP){strcat(key, "VOLUP+");}
	if(btn &PSP_CTRL_VOLDOWN){strcat(key, "VOLDOWN+");}
	memset(key+strlen(key)-1, 0, sizeof(key)-(strlen(key)+1));

	if(key != NULL)
	{			
		SetStatus(240, 110, VLF_ALIGNMENT_CENTER, "%s\n\n%s", message, key);

		memset(buffer, 0, sizeof(buffer));
		memset(data, 0, sizeof(data));

		ReadFile("ms0:/TM/config.txt", 0, (u8 *)buffer, sizeof(buffer));
		sprintf(data, "%s = \"/TM/%i/ipl.bin\";\r\n", key, pbp_ver);

		strcat(data, buffer);
		WriteFile("ms0:/TM/config.txt", (u8 *)data, strlen(data));
		sceKernelDelayThread(1200000);
	}

	RemoveWaitIcon();
	vlfGuiAddEventHandler(0, 700000, OnInstallComplete, NULL);
	return sceKernelExitDeleteThread(0);
}

void Install(int ddc, int sel_ver)
{
	int i, size = size_tm_ipl_DC500;
	pbp_ver = sel_ver;

	if(size != (size / 4096) * 4096)
	{
		size = (size + 4096) &0xFFFFFE00;
	}

	GetMSInfo();
	if(MSInfo.IPLSpace < size)
	{
		ErrorReturn(0, 1, STRING[STR_INSUFFICIENT_RESERVED_SECTOR_SPACE]);
		return;
	}

	sprintf(file_path, "ms0:/%i.PBP", pbp_ver);
	if (!FileExists(file_path))
	{
		ErrorReturn(0, 1, STRING[STR_NOT_FOUND_AT_ROOT], pbp_ver);
		return;
	}

	if ((DirExists("ms0:/TM/500") && ddc == 0) || (DirExists("ms0:/TM/660") && ddc == 1))
	{
		if (vlfGuiMessageDialog(STRING[STR_AN_EXISTING_INSTALLATION_WAS_FOUND], VLF_MD_TYPE_NORMAL | VLF_MD_BUTTONS_YESNO | VLF_MD_INITIAL_CURSOR_NO) != 1)
		{
			OnBackToMainMenu(0);
			return;
		}
	}

	for (i = 0; i < NELEMS(eboots); i++)
	{
		if (eboots[i].version == pbp_ver)
		{
			if (GetFileSize(file_path) != eboots[i].size)
			{
				ErrorReturn(0, 1, STRING[STR_INVALID_FILE_SIZE], pbp_ver);
				return;
			}
		}
	}

	SetTitle("update_plugin", "tex_update_icon", STRING[STR_CREATING_MAGIC_MEMORY_STICK]);
	InitProgress(1, 1, 1, 80, 100, VLF_ALIGNMENT_LEFT, STRING[STR_VERIFYING_PBP_VER], pbp_ver);
	RemoveBackgroundHandler(SetBackground, NULL);

	SceUID thid = sceKernelCreateThread("InstallThread", InstallThread, 0x18, 0x10000, 0, NULL);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 4, &ddc);
	}
}
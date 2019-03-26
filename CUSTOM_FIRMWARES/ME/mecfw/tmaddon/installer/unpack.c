#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <stdio.h>
#include <string.h>
#include <psputility.h>
#include <pspctrl.h>
#include <psppower.h>

#include "main.h"
#include "kubridge.h"

#include "libpsardumper.h"
#include "pspdecrypt.h"
#include "pspipl_update.h"



#include "../usbdevice/usbdevice_bin.h"
#include "../vshmenu_new/satellite_bin.h"
#include "../recovery/recovery_bin.h"
#include "../dax9660/dax9660.h"
#include "../march33/isotope_bin.h"
#include "../inferno/inferno_bin.h"
#include "../idcanager/idmanager_bin.h"
#include "../galaxy/pulsar_bin.h"
#include "../horoscope/horoscope_bin.h"
#include "../vshctrl/vshctrl_bin.h"
#include "../pspbtjnf/pspbtjnf_01g.h"
#include "../pspbtjnf/pspbtjnf_02g.h"
#include "../systemctrl/systemctrl_01g_bin.h"
#include "../systemctrl/systemctrl_02g_bin.h"

#include "data/ipl_01g_bin.h"
#include "data/ipl_02g_bin.h"
#include "data/resurrection_bin.h"
#include "data/libpsardumper_bin.h"
#include "data/pspdecrypt_bin.h"
#include "data/dcman_bin.h"
#include "data/iop_bin.h"
#include "data/ipl_update_bin.h"
#include "data/pspbtdnf_bin.h"
#include "data/pspbtdnf_02g_bin.h"
#include "data/tmctrl660_bin.h"
#include "data/ipl_bin.h"
#include "data/config_bin.h"

typedef struct Module
{
	char *path;
	void* buf;
	u32 size;
} Module;

#define N_FILES 25

static const Module modules[N_FILES] = 
{
	{ "/kd/usbdev.prx", usbdevice, sizeof(usbdevice) },
	{ "/vsh/module/satellite.prx", satellite, sizeof(satellite) },
	{ "/vsh/module/recovery.prx", recovery, sizeof(recovery) },
	{ "/vsh/module/resurrection.prx", resurrection , sizeof(resurrection) },
	{ "/kd/dax9660.prx", dax9660, sizeof(dax9660) },
	{ "/kd/isotope.prx", isotope, sizeof(isotope) },
	{ "/kd/inferno.prx", inferno, sizeof(inferno) },
	{ "/kd/idmanager.prx", idmanager, sizeof(idmanager) },
	{ "/kd/pulsar.prx", pulsar, sizeof(pulsar) },
	{ "/kd/horoscope.prx", horoscope, sizeof(horoscope) },
	{ "/kd/vshctrl_02g.prx", vshctrl, sizeof(vshctrl) },
	{ "/kd/pspbtjnf.bin", pspbtjnf_01g , sizeof(pspbtjnf_01g) },
	{ "/kd/pspbtjnf_02g.bin", pspbtjnf_02g , sizeof(pspbtjnf_02g) },
	{ "/kd/systemctrl_01g.prx", systemctrl_01g, sizeof(systemctrl_01g) },
	{ "/kd/systemctrl_02g.prx", systemctrl_02g, sizeof(systemctrl_02g) },
	{ "/kd/libpsardumper.prx", libpsardumper, sizeof(libpsardumper) },
	{ "/kd/pspdecrypt.prx", pspdecrypt , sizeof(pspdecrypt) },
	{ "/kd/dcman.prx", dcman, sizeof(dcman) },
	{ "/kd/iop.prx", iop, sizeof(iop) },
	{ "/kd/ipl_update.prx", ipl_update, sizeof(ipl_update) },
	{ "/kd/pspbtdnf.bin", pspbtdnf , sizeof(pspbtdnf) },
	{ "/kd/pspbtdnf_02g.bin", pspbtdnf_02g , sizeof(pspbtdnf_02g) },
	{ "/tmctrl660.prx", tmctrl660, sizeof(tmctrl660) },
	{ "/ipl.bin", ipl, sizeof(ipl) },
	{ "/config.me", config, sizeof(config) }
};

typedef struct UpdaterModule
{
	char *path;
	u32 offs;
	u32 size;
	u32 type;
} UpdaterModule;

#define N_UPD_FILES 3

static const UpdaterModule updatermodule[N_UPD_FILES] = 
{
	{ "/kd/emc_sm_updater.prx",			0x00030200, 0x000039D0, 1 },
	{ "/kd/lfatfs_updater.prx",			0x00021880, 0x0000E970, 1 },
	{ "/kd/lflash_fatfmt_updater.prx",	0x004E6380, 0x000028A0, 0 }
};

#define SMALL_BUFFER_SIZE	2100000
int BIG_BUFFER_SIZE;

u8 *big_buffer;
u8 *sm_buffer1, *sm_buffer2;

static void *p_malloc(u32 size)
{
	u32 *p;
	SceUID h_block;

	if(size != 0)
	{
		h_block = sceKernelAllocPartitionMemory(2, "block", 0, size + sizeof(h_block), NULL);
		if(h_block > 0)
		{
			p = (u32 *)sceKernelGetBlockHeadAddr(h_block);
			*p = h_block;
			return (void *)(p + 1);
		}
	}

	return NULL;
}

s32 p_mfree(void *ptr)
{
  return sceKernelFreePartitionMemory((SceUID)*((u32 *)ptr - 1));
}

void *malloc64(u32 size)
{
	void *buff = p_malloc(size + 64);
	if(buff != NULL)
	{
		u32 uid = *((u32 *)buff - 1);
		if( (u32)buff % 64)
		{
			u32 addr = (u32)buff;
			addr += 64;
			addr &= ~63;

			*(((u32 *)addr) - 1) = uid;
			return (void *)addr;
		}
	}
	return NULL;
}

void free_buffer(void)
{
	p_mfree(sm_buffer1);
	p_mfree(sm_buffer2);
	p_mfree(big_buffer);
}

int mallocate_buffer()
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
		return -1;
	}

	return 0;
}

int LoadStartModule(char *module, int partition)
{
	SceUID mod = kuKernelLoadModule(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, 0, NULL, NULL, NULL);
}

int LoadModules()
{
	SceUID mod = LoadStartModule("libpsardumper.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0)
	{
		pspDebugScreenPrintf("Error 0x%08X loading/starting libpsardumper.prx.\n", mod);
		return mod;
	}

	mod = LoadStartModule("pspdecrypt.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0)
	{
		pspDebugScreenPrintf("Error 0x%08X loading/starting pspdecrypt.prx.\n", mod);
		return mod;
	}

	return 0;
}

static const char *dir_list[] = {
	// flash0:/
	"/codepage",
	"/data",
	"/data/cert",
	"/dic",
	"/font",
	"/kd",
	"/kd/resource",
	"/vsh",
	"/vsh/resource",
	"/vsh/etc",
	"/vsh/module",
	// flash1:/
	"/dic",
	"/gps",
	"/net",
	"/net/http",
	"/registry",
	"/vsh",
	"/vsh/theme"
};

static int CreateDirs(const char *install_dir)
{
	int i;
	int res;
	char str[128];

	for(i=0;i<(sizeof(dir_list)/sizeof(char*));i++)
	{
		sprintf(str, "%s%s", install_dir, dir_list[i]);
		pspDebugScreenPrintf("Creating %s... ", str);

		res = sceIoMkdir( str, 0777);
		if (res < 0 && res != 0x80010011)
		{
			return res;
		}
		pspDebugScreenPrintf("OK\n");
	}

	return 0;
}

static int is5Dnum(char *str)
{
	int len = strlen(str);

	if (len != 5)
		return 0;

	int i;

	for (i = 0; i < len; i++)
	{
		if (str[i] < '0' || str[i] > '9')
			return 0;
	}

	return 1;
}

static char com_table[0x4000];
static int comtable_size;

static char _1g_table[0x4000];
static int _1gtable_size;

static char _2g_table[0x4000];
static int _2gtable_size;

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

//				if (!strncmp(table+i+6, "flash", 5) && j == 6)
				if (!strncmp(table+i+6, "flash", 5) && j == 7) //fix flash0 path
				{
					szOut[6] = ':';
					szOut[7] = '/';
//					k++;
				}
//				else if (!strncmp(table+i+6, "ipl", 3) && j == 3)
				else if (!strncmp(table+i+6, "ipl", 3) && j == 4) //fix ipl path
				{
					szOut[3] = ':';
					szOut[4] = '/';
//					k++;
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

void start_unpack(const char *update_path, const char *install_dir, int update_size)
{
	int i;
	int res;
	int size;
	SceUID fd;
	char str[0x400];
	u8 pbp_header[0x28];
	int error = 0, psar_pos = 0;

	comtable_size = 0;
	_1gtable_size = 0;
	_2gtable_size = 0;

	if (LoadModules() < 0)
	{
		return;
	}

	pspDebugScreenPrintf("Creating directories... \n");

	res = sceIoMkdir ("ms0:/TM", 0777);
	if (res >= 0 || res == 0x80010011)
	{
		res = sceIoMkdir (install_dir, 0777);
		if (res >= 0 || res == 0x80010011)
		{
			if (CreateDirs(install_dir) < 0)
			{
				pspDebugScreenPrintf("\nError: Directories creation failed.\n");
				return;
			}
		}
	}
	
	pspDebugScreenPrintf("Loading PBP ... \n");

	fd = sceIoOpen( update_path, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		pspDebugScreenPrintf("Error opening %s: 0x%08X\n", update_path, fd);
		return;
	}

	size = sceIoLseek32(fd, 0, PSP_SEEK_END);
	sceIoLseek32(fd, 0, PSP_SEEK_SET);
	
	if (size != update_size)
	{
		pspDebugScreenPrintf("Error: PBP filesize missmatch.\n");
		sceIoClose(fd);

		return;
	}

	sceIoRead(fd, pbp_header, sizeof(pbp_header));

	size = *(u32 *)&pbp_header[0x24] - *(u32 *)&pbp_header[0x20];

	u8 *updata_buffer = malloc64(size);
	if(!updata_buffer)
	{
		pspDebugScreenPrintf("Error: Allocate memory.\n");
		sceIoClose(fd);
		
		return;
	}

	sceIoLseek32(fd, *(u32 *)&pbp_header[0x20], PSP_SEEK_SET);
	size = sceIoRead(fd, updata_buffer, size);

	size = pspDecryptPRX(updata_buffer, big_buffer, size);
	if (size < 0)
	{
		pspDebugScreenPrintf("Error: Decrypt updata module.\n");
		p_mfree(updata_buffer);
		sceIoClose(fd);

		return;
	}

	pspDebugScreenPrintf("Saving Updater Modules...\n");

	for (i = 0; i < N_UPD_FILES; i++)
	{
		if (updatermodule[i].type == 1)
		{
			u8 x = 0xb;
			u8 y = 0x7;
			u8 z = 0x6;
			u8 t;
			int j;

			for (j = 0; j < updatermodule[i].size; j++)
			{
				x += y;
				big_buffer[j+updatermodule[i].offs] ^= x;
				
				t = y % (z + 1);
				y = z;
				z = x;
				x = t;
			}
		}

		size = pspDecryptPRX(big_buffer + updatermodule[i].offs, sm_buffer1, updatermodule[i].size);
		if (size < 0)
		{
			pspDebugScreenPrintf("Error: Decrypt updata module.\n");
			p_mfree(updata_buffer);
			sceIoClose(fd);

			return;
		}

		sprintf(str, "%s%s", install_dir, updatermodule[i].path);
		pspDebugScreenPrintf("Writing %s... ", str);

		if (WriteFile(str, sm_buffer1, size) != size)
		{
			pspDebugScreenPrintf("\nError: Writing %s\n", str);
			p_mfree(updata_buffer);
			sceIoClose(fd);

			return;
		}

		pspDebugScreenPrintf("OK\n");
	}

	size = sceIoLseek32(fd, 0, PSP_SEEK_END);
	sceIoLseek32(fd, 0, PSP_SEEK_SET);

	sceIoLseek32(fd, *(u32 *)&pbp_header[0x24], PSP_SEEK_SET);
	size = size - *(u32 *)&pbp_header[0x24];

	if (sceIoRead(fd, big_buffer, BIG_BUFFER_SIZE) <= 0)
	{
		pspDebugScreenPrintf("Error reading %s\n", update_path);
		sceIoClose(fd);

		return;
	}

	sceKernelDelayThread(10000);

	if (pspPSARInit(big_buffer, sm_buffer1, sm_buffer2) < 0)
	{
		pspDebugScreenPrintf("pspPSARInit failed!\n");
		sceIoClose(fd);

		return;
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
				pspDebugScreenPrintf("PSAR decode error, pos=0x%08X\n", pos);
				sceIoClose(fd);
				return;
			}
			
			int dpos = pos-psar_pos;
			psar_pos = pos;
			
			error = 1;
			memmove(big_buffer, big_buffer + dpos, BIG_BUFFER_SIZE - dpos);

			if (sceIoRead(fd, big_buffer+(BIG_BUFFER_SIZE-dpos), dpos) <= 0)
			{
				pspDebugScreenPrintf("Error reading PBP.\n");
				sceIoClose(fd);
				return;
			}

			pspPSARSetBufferPosition(psar_pos);

			continue;
		}
		else if (res == 0) // no more files
		{
			break;
		}

		if (is5Dnum(name))
		{
			if ( strcmp(name, "00001") != 0 && strcmp(name, "00002") != 0 )
			{
				int found = 0;
				
				if (_1gtable_size > 0 )
				{
					found = FindTablePath(_1g_table, _1gtable_size, name, name);
				}

				if (!found && _2gtable_size > 0 )
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

		if (cbExpanded > 0)
		{
			if (!strncmp(name, "flash0:/", 8))
			{
				sprintf(str, "%s/%s", install_dir, name+8);
				pspDebugScreenPrintf("Writing %s... ", str);

				res = WriteFile(str, sm_buffer2, cbExpanded);
				if (res <= 0)
				{
					pspDebugScreenPrintf("\nError 0x%08X writing %s\n", res, str);
					return;
				}
				pspDebugScreenPrintf("OK\n");
			}

			else if (!strncmp(name, "ipl:", 4))
			{
				u8 *ipl;
				char *ipl_str;

				if (strstr(name, "ipl") && strstr(name, "02g"))
				{
					cbExpanded = pspDecryptPRX(sm_buffer2, sm_buffer1 + 0x3000, cbExpanded);
					if (cbExpanded <= 0)
					{
						pspDebugScreenPrintf("Cannot decrypt PSP Slim IPL.\n");
						return;
					}

					ipl = ipl_02g;
					ipl_str = "/ipl_02g.bin";
				}
				else
				{
					memcpy(sm_buffer1 + 0x3000, sm_buffer2, cbExpanded);

					ipl = ipl_01g;
					ipl_str = "/ipl_01g.bin";
				}

				cbExpanded = pspDecryptIPL1 (sm_buffer1 + 0x3000, sm_buffer2, cbExpanded);
				if (cbExpanded <= 0)
				{
					pspDebugScreenPrintf("Error: Cannot decrypt PSP IPL.\n");
					return;
				}
				
				cbExpanded = pspLinearizeIPL2 (sm_buffer2, sm_buffer1 + 0x3000, cbExpanded);
				if (cbExpanded <= 0)
				{
					pspDebugScreenPrintf("Error: Cannot linearize PSP IPL.\n");
					return;
				}

				memcpy(sm_buffer1, ipl, 0x3000);

				sprintf (str, "%s%s", install_dir, ipl_str);
				
				pspDebugScreenPrintf("Writing %s... ", str);

				res = WriteFile(str, sm_buffer1, cbExpanded + 0x3000);
				if (res <= 0)
				{
					pspDebugScreenPrintf("\nError 0x%08X writing %s\n", res, str);
					return;
				}
				pspDebugScreenPrintf("OK\n");

				sceKernelDelayThread(5*1000);
			}

			else if (!strcmp(name, "com:00000"))
			{
				comtable_size = pspDecryptTable(sm_buffer2, sm_buffer1, cbExpanded, 4);

				if (comtable_size <= 0)
				{
					pspDebugScreenPrintf("Cannot decrypt common table.\n");
					return;
				}

				memcpy(com_table, sm_buffer2, comtable_size);
			}
			else if (!strcmp(name, "01g:00000") || !strcmp(name, "00001"))
			{
				_1gtable_size = pspDecryptTable(sm_buffer2, sm_buffer1, cbExpanded, 4);

				if (_1gtable_size <= 0)
				{
					pspDebugScreenPrintf("Cannot decrypt 01g table.\n");
					return;
				}

				memcpy(_1g_table, sm_buffer2, _1gtable_size);
			}
			
			else if (!strcmp(name, "02g:00000") || !strcmp(name, "00002"))
			{
				_2gtable_size = pspDecryptTable(sm_buffer2, sm_buffer1, cbExpanded, 4);
				if (_2gtable_size <= 0)
				{
					pspDebugScreenPrintf("Cannot decrypt 02g table %08X.\n", _2gtable_size);
					return;
				}

				memcpy(_2g_table, sm_buffer2, _2gtable_size);
			}
		}

		scePowerTick(0);
		error = 0;
	}

	p_mfree(updata_buffer);
	sceIoClose(fd);

	pspDebugScreenPrintf("Writing CFW files...\n");

	for(i = 0;i< N_FILES; i++)
	{
		sprintf( str, "%s%s", install_dir, modules[i].path);
		pspDebugScreenPrintf("Writing %s... ", str);

		res = WriteFile( str, modules[i].buf, modules[i].size);
		if (res < 0)
		{
			pspDebugScreenPrintf("\nError 0x%08X writing %s\n", res, modules[i].path);
			return;
		}

		pspDebugScreenPrintf("OK\n");
		sceKernelDelayThread(5*1000);
	}

#define CONFIG_TXT "ms0:/TM/config.txt"

	char *conf = "L = \"/TM/660/ipl.bin\";";

	memset(str, 0, sizeof(str));

	size = ReadFile(CONFIG_TXT, str, sizeof(str));
	if(size <= 0)
	{
		res = WriteFile(CONFIG_TXT, conf, strlen(conf));
		if (res < 0)
		{
			pspDebugScreenPrintf("Error 0x%08X writing %s\n", res, CONFIG_TXT);
			return;
		}
	}
	else
	{
		if (strstr(str, conf) == 0)
		{
			char buf[0x400];
			memset(buf, 0, sizeof(buf));

			sprintf(buf, "%s\r\n%s", conf, str);
			res = WriteFile(CONFIG_TXT, buf, strlen(buf));
			if (res < 0)
			{
				pspDebugScreenPrintf("Error 0x%08X writing %s\n", res, CONFIG_TXT);
				return;
			}
		}
	}

	sceKernelDelayThread(10000);
	pspDebugScreenPrintf("complete!\n");
	return;
}

void unpack()
{
	pspDebugScreenClear();
	pspDebugScreenSetTextColor(0x0000FF00);

	start_unpack ("ms0:/660.PBP", "ms0:/TM/660", 0x01F19005);

	sub_01544();
	pspDebugScreenClear();
}

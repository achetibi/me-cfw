#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <stdio.h>
#include <string.h>
#include <psputility.h>
#include <pspctrl.h>
#include <psppower.h>

#include "main.h"
#include "dcman.h"
#include "kubridge.h"

#include "libpsardumper.h"
#include "pspdecrypt.h"
#include "pspipl_update.h"

#include "../../recovery/recovery_bin.h"
#include "../../galaxy/pulsar_bin.h"
#include "../../march33/isotope_bin.h"
#include "../../horoscope/horoscope_bin.h"
#include "../../vshctrl/vshctrl_bin.h"
#include "../../vshmenu_new/satellite_bin.h"
#include "../../idcanager/idmanager_bin.h"
#include "../../inferno/inferno_bin.h"
#include "../../usbdevice/usbdevice_bin.h"
#include "../../dax9660/dax9660.h"
#include "../../pspbtjnf/pspbtjnf_01g.h"
#include "../../pspbtjnf/pspbtjnf_02g.h"
#include "../../systemctrl/systemctrl_01g_bin.h"
#include "../../systemctrl/systemctrl_02g_bin.h"
#include "../../custom_ipl/ipl_block_01g.h"
#include "../../custom_ipl/ipl_block_02g.h"
#include "../installer/data/config_bin.h"

typedef struct Module
{
	char *path;
	void* buf;
	u32 size;
} Module;

#define N_FILES 13

static const Module modules_01g[N_FILES] = 
{
	{ "flash0:/kd/dax9660.prx", dax9660, sizeof(dax9660) },
	{ "flash0:/kd/horoscope.prx", horoscope, sizeof(horoscope) },
	{ "flash0:/kd/idmanager.prx", idmanager, sizeof(idmanager) },
	{ "flash0:/kd/inferno.prx", inferno, sizeof(inferno) },
	{ "flash0:/kd/isotope.prx", isotope, sizeof(isotope) },
	{ "flash0:/kd/pulsar.prx", pulsar, sizeof(pulsar) },
	{ "flash0:/kd/pspbtjnf.bin", pspbtjnf_01g , sizeof(pspbtjnf_01g) },
	{ "flash0:/kd/systemctrl_01g.prx", systemctrl_01g, sizeof(systemctrl_01g) },
	{ "flash0:/kd/usbdev.prx", usbdevice, sizeof(usbdevice) },
	{ "flash0:/kd/vshctrl_02g.prx", vshctrl, sizeof(vshctrl) },
	{ "flash0:/vsh/module/satellite.prx", satellite, sizeof(satellite) },
	{ "flash0:/vsh/module/recovery.prx", recovery, sizeof(recovery) },
	{ "flash1:/config.me", config, sizeof(config) }
};

static const Module modules_02g[N_FILES] = 
{
	{ "flash0:/kd/dax9660.prx", dax9660, sizeof(dax9660) },
	{ "flash0:/kd/horoscope.prx", horoscope, sizeof(horoscope) },
	{ "flash0:/kd/idmanager.prx", idmanager, sizeof(idmanager) },
	{ "flash0:/kd/inferno.prx", inferno, sizeof(inferno) },
	{ "flash0:/kd/isotope.prx", isotope, sizeof(isotope) },
	{ "flash0:/kd/pulsar.prx", pulsar, sizeof(pulsar) },
	{ "flash0:/kd/pspbtjnf_02g.bin", pspbtjnf_02g , sizeof(pspbtjnf_02g) },
	{ "flash0:/kd/systemctrl_02g.prx", systemctrl_02g, sizeof(systemctrl_02g) },
	{ "flash0:/kd/usbdev.prx", usbdevice, sizeof(usbdevice) },
	{ "flash0:/kd/vshctrl_02g.prx", vshctrl, sizeof(vshctrl) },
	{ "flash0:/vsh/module/satellite.prx", satellite, sizeof(satellite) },
	{ "flash0:/vsh/module/recovery.prx", recovery, sizeof(recovery) },
	{ "flash1:/config.me", config, sizeof(config) }
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

	for (i = 0; i < table_size - 5; i++)
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

static int LoadUpdaterModules(/*int ofw*/)
{
	SceUID mod = sceKernelLoadModule("flash0:/kd/emc_sm_updater.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
		return mod;

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
			return mod;
	}

/*
	if (!ofw && kuKernelGetModel() == 0)
		dcPatchModule("sceNAND_Updater_Driver", 1, 0x0D7E, 0xAC60);
	else
		dcPatchModule("sceNAND_Updater_Driver", 1, 0x0D7E, 0xAC64);
*/

	mod = sceKernelLoadModule("flash0:/kd/lfatfs_updater.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
		return mod;

	dcPatchModuleString("sceLFatFs_Updater_Driver", "flash", "flach");
	//printf("hit = %d\n" ,hit);

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
			return mod;
	}

	mod = sceKernelLoadModule("flash0:/kd/lflash_fatfmt_updater.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
		return mod;

	sceKernelDelayThread(10000);

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
			return mod;
	}

	mod = sceKernelLoadModule("flash0:/kd/ipl_update.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
		return mod;

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
			return mod;
	}

	mod = sceKernelLoadModule("flash0:/kd/libpsardumper.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
		return mod;

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
			return mod;
	}

	mod = sceKernelLoadModule("flash0:/kd/pspdecrypt.prx", 0, NULL);
	if (mod < 0 && mod != SCE_KERNEL_ERROR_EXCLUSIVE_LOAD)
		return mod;

	if (mod >= 0)
	{
		mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
		if (mod < 0)
			return mod;
	}

	return 0;
}

static const char *dir_list[] = {
	"flash0:/codepage",
	"flash0:/data",
	"flash0:/data/cert",
	"flash0:/dic",
	"flash0:/font",
	"flash0:/kd",
	"flash0:/kd/resource",
	"flash0:/vsh",
	"flash0:/vsh/resource",
	"flash0:/vsh/etc",
	"flash0:/vsh/module",
	"flash1:/dic",
	"flash1:/gps",
	"flash1:/net",
	"flash1:/net/http",
	"flash1:/registry",
	"flash1:/vsh",
	"flash1:/vsh/theme"
};

static int CreateDirs()
{
	int i;
	int res;
	char str[128];

	for(i = 0; i < (sizeof(dir_list)/sizeof(char*)); i++)
	{
		strcpy( str, dir_list[i]);
		printf("Creating %s... ", str);
		str[3] = 'c';
		res = sceIoMkdir( str, 0777);

		if (res < 0 && res != 0x80010011)
		{
			return res;
		}
		printf("OK\n");
	}

	return 0;
}

void install_fw(const char *update_path, int ofw)
{
	int res;
	SceUID fd;
	int size;
	u8 pbp_header[0x28];
	int error = 0, psar_pos = 0;
	char str[256];
	char *argv[2];
	int model = kuKernelGetModel();

	comtable_size = 0;
	_1gtable_size = 0;
	_2gtable_size = 0;
	
	printf("Loading updater modules... ");
	if (LoadUpdaterModules(/*ofw*/) < 0)
	{
		printf("\nError loading updater modules.");
		return;
	}
	printf("OK\n");

	sceKernelDelayThread(1200000);

	printf("Formatting flash0... ");

	argv[0] = "fatfmt";
	argv[1] = "lflach0:0,0";

	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		printf("\nFlash0 format failed: 0x%08X\n", res);
		return;
	}
	printf("OK\n");

	sceKernelDelayThread(1000*1000);

	printf("Formatting flash1... ");
	argv[1] = "lflach0:0,1";
	res = dcLflashStartFatfmt(2, argv);
	if (res < 0)
	{
		printf( "\nFlash1 format failed: 0x%08X", res);
		return;
	}
	printf("OK\n");

	sceKernelDelayThread(1000000);

	printf("Formatting flash2... ");
	argv[1] = "lflach0:0,2";
	dcLflashStartFatfmt(2, argv);
	printf("OK\n");

	sceKernelDelayThread(1000000);

	printf("Assigning flashes... ");

	res = sceIoAssign("flach0:", "lflach0:0,0", "flachfat0:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
	{
		printf("Flash0 assign failed: 0x%08X\n", res);
		return;
	}

	res = sceIoAssign("flach1:", "lflach0:0,1", "flachfat1:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
	{
		printf("Flash1 assign failed: 0x%08X\n", res);
		return;
	}

	sceIoAssign("flach2:", "lflach0:0,2", "flachfat2:", IOASSIGN_RDWR, NULL, 0);
	printf("OK\n");

	sceKernelDelayThread(1000000);

	printf("Creating directories... \n");
	if (CreateDirs() < 0)
	{
		printf("\nError: Directories creation failed.\n");
		return;
	}

	printf("Loading PBP ... \n");

	fd = sceIoOpen( update_path, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		printf("Error opening %s: 0x%08X\n", update_path, fd);
		return;
	}

	size = sceIoLseek32(fd, 0, PSP_SEEK_END);
	sceIoLseek32(fd, 0, PSP_SEEK_SET);

	sceIoRead(fd, pbp_header, sizeof(pbp_header));

	sceIoLseek32(fd, *(u32 *)&pbp_header[0x24], PSP_SEEK_SET);

	size = size - *(u32 *)&pbp_header[0x24];

	if (sceIoRead(fd, big_buffer, BIG_BUFFER_SIZE) <= 0)
	{
		printf("Error reading %s\n", update_path);
		sceIoClose(fd);
		return;
	}

	sceKernelDelayThread(10000);

	if (pspPSARInit(big_buffer, sm_buffer1, sm_buffer2) < 0)
	{
		printf("pspPSARInit failed!\n");
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
				printf("PSAR decode error, pos=0x%08X\n", pos);
				sceIoClose(fd);
				return;
			}
			
			int dpos = pos-psar_pos;
			psar_pos = pos;
			
			error = 1;
			memmove(big_buffer, big_buffer+dpos, BIG_BUFFER_SIZE-dpos);

			if (sceIoRead(fd, big_buffer+(BIG_BUFFER_SIZE-dpos), dpos) <= 0)
			{
				printf("Error reading PBP.\n");
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
				
				if (_1gtable_size > 0 && model == 0 )
				{
					found = FindTablePath(_1g_table, _1gtable_size, name, name);
				}

				if (!found && _2gtable_size > 0 && model == 1)
				{
					found = FindTablePath(_2g_table, _2gtable_size, name, name);
				}

				if (!found)
				{
//					printf("Warning: first cannot find path of %s\n", name);
//					sceKernelDelayThread(1000*1000);
					error = 0;
					continue;
				}
			}
		}

		if (cbExpanded > 0)
		{
			if (!strncmp(name, "flash0:/", 8))
			{
				if (signcheck)
				{
					pspSignCheck(sm_buffer2);
				}

				printf("Writing %s... ", name);
				strcpy( str, name);
				str[3] = 'c';

				res = WriteFile(str, sm_buffer2, cbExpanded);
				if (res <= 0)
				{
					str[3] = 's';
					printf("Error 0x%08X flashing %s\n", res, str);
					return;
				}
				
				printf("OK\n");
			}

			else if (!strncmp(name, "ipl:", 4))
			{
				u8 *ipl;
				int ipl_size;
				
				printf("Flashing IPL... ");

				if (model == 1)
				{
					cbExpanded = pspDecryptPRX(sm_buffer2, sm_buffer1 + 0x4000, cbExpanded);
					if (cbExpanded <= 0)
					{
						printf("Cannot decrypt PSP Slim IPL.\n");
						return;
					}
					
					memcpy(sm_buffer1, ipl_block_02g, 0x4000);
				}
				else
				{
					memcpy(sm_buffer1 + 0x4000, sm_buffer2, cbExpanded);
					memcpy(sm_buffer1, ipl_block_01g, 0x4000);
				}

				ipl = sm_buffer1;
				ipl_size = cbExpanded + 0x4000;

				if (ofw)
				{
					ipl += 0x4000;
					ipl_size -= 0x4000;
				}

				dcPatchModuleString("IoPrivileged", "IoPrivileged", "IoPrivileged");//clear cache?

				if (pspIplUpdateClearIpl() < 0)
				{
					printf("Error in pspIplUpdateClearIpl\n");
					return;
				}
				
				if (pspIplUpdateSetIpl(ipl, ipl_size) < 0)
				{
					printf("Error in pspIplUpdateSetIpl\n");
					return;
				}
				
				printf("OK\n");

				sceKernelDelayThread(5*1000);
			}

			else if (!strcmp(name, "com:00000"))
			{
				comtable_size = pspDecryptTable(sm_buffer2, sm_buffer1, cbExpanded, 4);

				if (comtable_size <= 0)
				{
					printf("Cannot decrypt common table.\n");
					return;
				}

				memcpy(com_table, sm_buffer2, comtable_size);
			}

			else if ( model == 0 && (!strcmp(name, "01g:00000") || !strcmp(name, "00001") ) )
			{
				_1gtable_size = pspDecryptTable(sm_buffer2, sm_buffer1, cbExpanded, 4);

				if (_1gtable_size <= 0)
				{
					printf("Cannot decrypt 1g table.\n");
					return;
				}

				memcpy(_1g_table, sm_buffer2, _1gtable_size);
			}

			else if ( model == 1 && (!strcmp(name, "02g:00000") || !strcmp(name, "00002") ))
			{
				_2gtable_size = pspDecryptTable(sm_buffer2, sm_buffer1, cbExpanded, 4);

				if (_2gtable_size <= 0)
				{
					printf("Cannot decrypt 2g table %08X.\n", _2gtable_size);
					return;
				}

				memcpy(_2g_table, sm_buffer2, _2gtable_size);
			}			
		}

		scePowerTick(0);
		error = 0;
	}

	sceIoClose(fd);

	if (!ofw)
	{
		Module *common_modules;
		int i;

		printf("Flashing CFW files...\n");

		if (model == 1)
		{
			common_modules = (Module *)modules_02g;
		}
		else
		{
			common_modules = (Module *)modules_01g;
		}

		for(i = 0; i < N_FILES; i++)
		{
			strcpy( str, common_modules[i].path);
			printf("Writing %s... ", str);
			str[3] = 'c';

			res = WriteFile( str, common_modules[i].buf, common_modules[i].size);
			if (res < 0)
			{
				printf("\nError 0x%08X flashing %s\n", res, common_modules[i].path);
				return;
			}

			printf("OK\n");
			sceKernelDelayThread(5*1000);
		}
	}

	printf("complete!\n");
	return;
}

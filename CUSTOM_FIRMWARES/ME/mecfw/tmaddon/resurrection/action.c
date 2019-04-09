#include <pspkernel.h>
#include <psppower.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "dcman.h"
#include "kubridge.h"

extern int ofw;

char *mobos[] =
{
	"TA-79 v1",
	"TA-79 v2",
	"TA-79 v3",
	"TA-81",
	"TA-82",
	"TA-85",
	"TA-85 v2",
	"TA-86",
	"TA-88",
	"TA-88+",
	"O_O'"
};

void sub_01544 ( void )
{
	printf("\n\nPress X to return\n");
	
	while (1)
	{
		sceKernelDelayThread(10000);
		u32 pad_data = ctrlRead();

		if ((pad_data & PSP_CTRL_CROSS) != 0)
			break;
	}
}

#if _PSP_FW_VERSION == 660
#define PBP_FILE "ms0:/660.PBP"
#elif _PSP_FW_VERSION == 661
#define PBP_FILE "ms0:/661.PBP"
#else
#error action.c
#endif

int check_pbp(const char *pbp, int size)
{
	SceIoStat stat;
	int ret = 0;
	
	if(sceIoGetstat(pbp, &stat) >= 0)
	{
		if (stat.st_size != size)
		{
			printf("Error: PBP filesize missmatch.\n");
			ret = -1;
		}
	}
	else
	{
		printf("Error cannot find %s\n", pbp);
		ret = -1;
	}

	return ret;
}

int fw_install()
{
	int ret = 0;
	myDebugScreenClear();

#if _PSP_FW_VERSION == 660
    int res = check_pbp(PBP_FILE, 0x01F19005);
#elif _PSP_FW_VERSION == 661
    int res = check_pbp(PBP_FILE, 0x01F123C5);
#else
#error action.c
#endif

	if (res < 0)
	{
		ret = -1;
		sub_01544();
		myDebugScreenClear();
	}
	else
	{
		dcSetCancelMode(1);
		install_fw(PBP_FILE, ofw);
		dcSetCancelMode(0);

		sub_01544();
		myDebugScreenClear();
		scePowerRequestStandby();
	}

	return ret;
}

void HardwareInfo()
{
	u32 tachyon, baryon, pommel, mb, fuseconfig, nandsize;
	U64 fuseid;
	int model;
	char *model_str;

	dcGetHardwareInfo(&tachyon, &baryon, &pommel, &mb, (void *)&fuseid, &fuseconfig, &nandsize);
	model = kuKernelGetModel();

	if (model == 0)
		model_str = "(Phat)";
	else if (model == 1)
		model_str = "(Slim)";
	else
		model_str = "";

	myDebugScreenClear();
	myDebugScreenSetTextColor(0x00FF0000, 0x00000000);

	printf("Hardware Information\n\n\n");
	myDebugScreenSetTextColor(0x00FFFFFF, 0x00000000);

	printf("Model: %02dg %s\n", model+1, model_str);
	printf("Motherboard: %s\n", mobos[mb]);
	printf("Tachyon: 0x%08X\n", tachyon);
	printf("Baryon: 0x%08X\n", baryon);
	printf("Pommel: 0x%08X\n", pommel);
	printf("Nand size: %dMB\n", nandsize / 1048576);
	printf("Fuse ID: 0x%04X%08X\n", fuseid.high, fuseid.low);

	sub_01544();
	myDebugScreenClear();
}

void TestME()
{
	sctrlKernelExitVSH(NULL);
}

void Shutdown()
{
	scePowerRequestStandby();
}

void Reboot()
{
	scePowerRequestColdReset(0);
}

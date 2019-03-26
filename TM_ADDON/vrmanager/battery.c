#include <pspsdk.h>
#include <pspkernel.h>
#include <string.h>
#include <stdio.h>
#include <vlf.h>

#include "trans.h"
#include "common.h"
#include "vlfutils.h"
#include "../kpspident/main.h"

u16 Serial1;
u16 Serial2;
extern const char **g_messages;

u32 GetBatterySerial()
{
	u16 Serial[2];
	pspReadSerial(Serial);

	return (Serial[1] & 0xFFFF) + ((Serial[0] & 0xFFFF)*0x10000);
}

int OnSetBatterySerialComplete(void *param)
{
	char BatMode[128];
	u32 Serial = GetBatterySerial();

	if(Serial == 0xFFFFFFFF)
	{
		sprintf(BatMode, STRING[STR_PANDORA_BATTERY]);
	}
	else if(Serial == 0x00000000)
	{
		sprintf(BatMode, STRING[STR_AUTOBOOT_BATTERY]);
	}
	else
	{
		sprintf(BatMode, STRING[STR_NORMAL_BATTERY]);
	}

	ResetScreen(0, 1, 0);
	SetStatus(240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_BATTERY_MODE_SERIAL], BatMode, Serial);

	return VLF_EV_RET_REMOVE_HANDLERS;
}

int SetBatterySerialThread(SceSize args, void *argp)
{
	if(GetBatterySerial() == (Serial1 &0xFFFF) + ((Serial2 &0xFFFF) * 0x10000))
	{
		ErrorReturn(1, 0, STRING[STR_NO_NEED_TO_CHANGE_SERIAL], Serial1, Serial2);
	}

	u16 Serial[2];
	Serial[0] = Serial1;
	Serial[1] = Serial2;

	if((Serial[0] == 0x5241) && (Serial[1] == 0x4E44))
	{
		Serial[0] = Rand(0x0001, 0xFFFE);
		Serial[1] = Rand(0x0001, 0xFFFE);
	}
	
	pspWriteSerial(Serial);

	if(GetBatterySerial() != (Serial[1] &0xFFFF) + ((Serial[0] &0xFFFF) * 0x10000))
	{
		ErrorReturn(1, 0, STRING[STR_UNABLE_TO_WRITE_THE_SERIAL]);
	}

	vlfGuiAddEventHandler(0, 700000, OnSetBatterySerialComplete, NULL);

	return sceKernelExitDeleteThread(0);
}

void SetBatterySerial(u16 ser1, u16 ser2)
{
	Serial1 = ser1;
	Serial2 = ser2;
	char BatMode[128];

	if(pspGetBaryonVersion() >= 0x00234000)
	{
		ErrorReturn(0, 0, STRING[STR_PSP_HARDWARE_NOT_SUPPORTED]);
		return;
	}

	if ((Serial1 == 0xFFFF) && (Serial2 == 0xFFFF))
	{
		sprintf(BatMode, STRING[STR_PANDORA_BATTERY]);
	}
	else if ((Serial1 == 0x0000) && (Serial2 == 0x0000))
	{
		sprintf(BatMode, STRING[STR_AUTOBOOT_BATTERY]);
	}
	else
	{
		sprintf(BatMode, STRING[STR_NORMAL_BATTERY]);
	}

	SetTitle("update_plugin", "tex_update_icon", STRING[STR_CONVERT_BATTERY]);
	InitProgress(0, 0, 1, 240, 120, VLF_ALIGNMENT_CENTER, STRING[STR_CONVERTING_TO], BatMode);
	RemoveBackgroundHandler(SetBackground, NULL);

	SceUID thid = sceKernelCreateThread("SetBatterySerialThread", SetBatterySerialThread, 0x18, 0x10000, 0, NULL);
	if (thid >= 0)
	{
		sceKernelStartThread(thid, 0, NULL);
	}
}


#include <pspsdk.h>
#include <pspkernel.h>
#include <psppower.h>
#include <pspwlan.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <vlf.h>

#include "dcman.h"
#include "trans.h"
#include "common.h"
#include "pspinfo.h"
#include "vlfutils.h"
#include "../kpspident/main.h"

#define VLF_ITEMS 20

extern int mode;
extern const char **g_messages;
extern VlfText vlf_texts[VLF_ITEMS];

struct regions {
	int model;
	u8 code;
} regions[] = {
	{ 0x00, 0x03 }, // Japan
	{ 0x01, 0x04 }, // America
	{ 0x02, 0x09 }, // Australia
	{ 0x03, 0x07 }, // England
	{ 0x04, 0x05 }, // Europe
	{ 0x05, 0x06 }, // Korea
	{ 0x06, 0x0A }, // Hong Kong
	{ 0x07, 0x0B }, // Taiwan
	{ 0x08, 0x0C }, // Russia
	{ 0x09, 0x0D }, // China
	{ 0x0A, 0x08 }, // Mexico
};

struct hardwares {
	u32 tachyon;
	u32 baryon;
	u32 pommel;
	int revision;
	int generation;
	char *mobo;
	char *version;
} hardwares[] = {
	/*** PSP-100X ****************************************************************/
	{ 0x00140000, 0x00030600, 0x00000103, 0x010100, 1, "TA-079 v1", "FAT v1.0"    },
	{ 0x00200000, 0x00030600, 0x00000103, 0x010101, 1, "TA-079 v2", "FAT v1.1"    },
	{ 0x00200000, 0x00040600, 0x00000103, 0x010102, 1, "TA-079 v3", "FAT v1.2"    },
	{ 0x00300000, 0x00040600, 0x00000103, 0x010200, 1, "TA-081 v1", "FAT v2.0"    },
	{ 0x00300000, 0x00040600, 0x00000104, 0x010201, 1, "TA-081 v2", "FAT v2.1"    },
	{ 0x00400000, 0x00114000, 0x00000112, 0x010300, 1, "TA-082",    "FAT v3.0"    },
	{ 0x00400000, 0x00121000, 0x00000112, 0x010400, 1, "TA-086",    "FAT v4.0"    },
	/*** PSP-200X ****************************************************************/
	{ 0x00500000, 0x0022B200, 0x00000123, 0x020100, 2, "TA-085 v1", "Slim v1.0"   },
	{ 0x00500000, 0x00234000, 0x00000123, 0x020101, 2, "TA-085 v2", "Slim v1.1"   },
	{ 0x00500000, 0x00243000, 0x00000123, 0x020200, 2, "TA-088 v1", "Slim v2.0"   },
	{ 0x00500000, 0x00243000, 0x00000123, 0x020201, 2, "TA-088 v2", "Slim v2.1"   },
	{ 0x00600000, 0x00243000, 0x00000123, 0x020202, 2, "TA-088 v3", "Slim v2.2"   },
	{ 0x00500000, 0x00243000, 0x00000132, 0x020300, 2, "TA-090 v1", "Slim v3.0"   },
	/*** PSP-300X ****************************************************************/
	{ 0x00600000, 0x00263100, 0x00000132, 0x030100, 3, "TA-090 v2", "Brite v1.0"  },
	{ 0x00600000, 0x00263100, 0x00000133, 0x030101, 3, "TA-090 v3", "Brite v1.1"  },
	{ 0x00600000, 0x00285000, 0x00000133, 0x030200, 3, "TA-092",    "Brite v2.0"  },	
	{ 0x00810000, 0x002C4000, 0x00000141, 0x040300, 4, "TA-093 v1", "Brite v3.0"  },
	{ 0x00810000, 0x002C4000, 0x00000143, 0x040301, 4, "TA-093 v2", "Brite v3.1"  },
	{ 0x00810000, 0x002E4000, 0x00000154, 0x040400, 4, "TA-095 v1", "Brite v4.0"  },
	{ 0x00820000, 0x002E4000, 0x00000154, 0x040401, 4, "TA-095 v2", "Brite v4.1"  },
	/*** PSP-N100X ***************************************************************/
	{ 0x00720000, 0x00304000, 0x00000133, 0x050100, 5, "TA-091",    "PSPgo v1.0"  },
	{ 0x00800000, 0x002A0000, 0x00000000, 0x050200, 5, "TA-094",    "PSPgo v2.0"  },
	/*** PSP-E100X ***************************************************************/
	{ 0x00900000, 0x00403000, 0x00000154, 0x060100, 6, "TA-096",    "Street v1.0" },
	/*** DevKit ******************************************************************/
	{ 0x00100000, 0x00000000, 0x00000000, 0x000000, 0, "Devkit",    "Devkit"      },
};

int GetMSInfo()
{
	char *buf = malloc(0x20000);

	SceUID fd = sceIoOpen("msstor:", PSP_O_RDONLY, 0777);
	if(fd < 1)
	{
		return -1;
	}

	sceIoRead(fd, buf, 512);
	MSInfo.BootStatus = buf[446];
	MSInfo.StartHead = buf[447];
	MSInfo.StartSector = *(u16 *)&buf[448];
	MSInfo.PartitionType = buf[450];
	MSInfo.LastHead = buf[451];
	MSInfo.LastSector = *(u16 *)&buf[452];
	MSInfo.AbsSector = (buf[454] &0xFF) + ((buf[455] &0xFF) * 0x100) + ((buf[456] &0xFF) * 0x10000) + ((buf[457] &0xFF) * 0x1000000);
	MSInfo.TotalSectors = (buf[458] &0xFF) + ((buf[459] &0xFF) * 0x100) + ((buf[460] &0xFF) * 0x10000) + ((buf[461] &0xFF) * 0x1000000);
	MSInfo.Signature = *(u16 *)&buf[510];
	MSInfo.IPLSpace = (MSInfo.AbsSector - 16) * 512;

	sceIoLseek(fd, MSInfo.AbsSector * 512, 0);
	sceIoRead(fd, buf, 512);

	MSInfo.SerialNum = (buf[67] &0xFF) + ((buf[68] &0xFF) * 0x100) + ((buf[69] &0xFF) * 0x10000) + ((buf[70] &0xFF) * 0x1000000);
	memcpy(MSInfo.Label, buf + 0x47, 11);
	memcpy(MSInfo.FileSystem, buf + 0x52, 8);
	sceIoLseek(fd, 0x2000, 0);

	memset(buf, 0, 4096);
	sceIoRead(fd, buf, 4096);
	sceIoClose(fd);
	free(buf);

	return 0;
}

int pspGetFreeRam()
{
	void* buf[128];
	int i = 0;

	for(i = 0; i < 128; i++)
	{
		buf[i] = malloc(512 * 1024);

		if(!buf[i])
		{
			break;
		}
	}

	int result = i;

	for(; i >= 0; i--)
	{
		free(buf[i]);
	}

	return (result * 512 * 1024);
}

void SystemInfoPage()
{
	int i, TimeMachine = 0;
	char buf[128], BatteryMode[64], BatterySerial[32], owner_name[32], unicode_username[26];
	u8 kirk[4];
	u8 spock[4];
	u8 macaddr[32];
	u32 nandsize, Serial;
	int hi = pspGetHardwareInfo();
	int ri = pspGetRegionInfo();
	
	char *psp_regions[] =
	{
		STRING[STR_REGION_JAPAN],
		STRING[STR_REGION_AMERICA],
		STRING[STR_REGION_AUSTRALIA],
		STRING[STR_REGION_UNITED_KINGDOM],
		STRING[STR_REGION_EUROPE],
		STRING[STR_REGION_KOREA],
		STRING[STR_REGION_HONGKONG],
		STRING[STR_REGION_TAIWAN],
		STRING[STR_REGION_RUSSIA],
		STRING[STR_REGION_CHINA],
		STRING[STR_REGION_MEXICO],
	};

	*(u32 *)kirk = pspGetKirkVersion();
	*(u32 *)spock = pspGetSpockVersion();
	dcGetHardwareInfo(NULL, NULL, NULL, NULL, NULL, NULL, &nandsize);
	pspGetMACAddress(macaddr);
	
	if (FileExists("flash0:/kd/resurrection.prx") || DirExists("flach0:/"))
	{
		TimeMachine = 1;
	}

	vlfGuiSetRectangleFade(0, VLF_TITLEBAR_HEIGHT, 480, 272 - VLF_TITLEBAR_HEIGHT, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0, NULL, NULL, 0);

	if(mode == 0x00000104)
	{
		SetTitle("video_plugin_videotoolbar", "tex_help_bar_icon", STRING[STR_PSP_INFORMATIONS]);
		memset(owner_name, 0, sizeof(owner_name));
		UTF82Unicode((char *)pspGetRegistryValue("/CONFIG/SYSTEM", "owner_name", &owner_name, sizeof(owner_name)), (char *)unicode_username);

		vlf_texts[0] = vlfGuiAddUnicodeText(30, 70, STRING[STR_PSP_INFORMATIONS_MODEL], hi < 0 ? STRING[STR_UNKNOWN] : hardwares[hi].version);
		vlf_texts[1] = vlfGuiAddUnicodeText(30, 95, STRING[STR_PSP_INFORMATIONS_GENERATION], hi < 0 ? 0 : hardwares[hi].generation);
		vlf_texts[2] = vlfGuiAddUnicodeText(30, 120, STRING[STR_PSP_INFORMATIONS_CPU_SPEED], scePowerGetCpuClockFrequency());
		vlf_texts[3] = vlfGuiAddUnicodeText(30, 145, STRING[STR_PSP_INFORMATIONS_BUS_SPEED], scePowerGetBusClockFrequency());
		vlf_texts[4] = vlfGuiAddUnicodeText(30, 170, STRING[STR_PSP_INFORMATIONS_FREE_RAM], pspGetFreeRam() / 524288);
		vlf_texts[5] = vlfGuiAddUnicodeText(30, 195, STRING[STR_PSP_INFORMATIONS_NAME], (u16 *)unicode_username);

		vlf_texts[6] = vlfGuiAddUnicodeText(230, 70, STRING[STR_PSP_INFORMATIONS_PSP_VERSION], ((pspKernelGetModel() + 1) * 1000 ) + regions[ri].model);
		vlf_texts[7] = vlfGuiAddUnicodeText(230, 95, STRING[STR_PSP_INFORMATIONS_REGION], ri < 0 ? STRING[STR_UNKNOWN] : psp_regions[ri]);
		vlf_texts[8] = vlfGuiAddUnicodeText(230, 120, STRING[STR_PSP_INFORMATIONS_ORIGINAL_FW], pspGetShippedFirmware(buf));
		vlf_texts[9] = vlfGuiAddUnicodeText(230, 145, STRING[STR_PSP_INFORMATIONS_MOTHERBOARD], hi < 0 ? STRING[STR_UNKNOWN] : hardwares[hi].mobo);
		vlf_texts[10] = vlfGuiAddUnicodeText(230, 170, STRING[STR_PSP_INFORMATIONS_WLAN_STATUS], sceWlanGetSwitchState() ? STRING[STR_ON] : STRING[STR_OFF]);
		vlf_texts[11] = vlfGuiAddUnicodeText(230, 195, STRING[STR_PSP_INFORMATIONS_MAC_ADDRESS], macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);

		for (i = 0; i < VLF_ITEMS; i++)
		{
			if(vlf_texts[i] != NULL)
			{
				vlfGuiSetTextFontSize(vlf_texts[i], 0.75f);
			}
		}
	}
	else if(mode == 0x00000204)
	{
		SetTitle("video_plugin_videotoolbar", "tex_help_bar_icon", STRING[STR_SYSTEM_INFORMATIONS]);
		
		vlf_texts[0] = vlfGuiAddUnicodeText(30, 70, STRING[STR_SYSTEM_INFORMATIONS_TACHYON], pspGetTachyonVersion());
		vlf_texts[1] = vlfGuiAddUnicodeText(30, 95, STRING[STR_SYSTEM_INFORMATIONS_BARYON], pspGetBaryonVersion());
		vlf_texts[2] = vlfGuiAddUnicodeText(30, 120, STRING[STR_SYSTEM_INFORMATIONS_POMMEL], pspGetPommelVersion());
		vlf_texts[3] = vlfGuiAddUnicodeText(30, 145, STRING[STR_SYSTEM_INFORMATIONS_KIRK_VER], kirk[3], kirk[2], kirk[1], kirk[0]);
		vlf_texts[4] = vlfGuiAddUnicodeText(30, 170, STRING[STR_SYSTEM_INFORMATIONS_SPOCK_VER], spock[3], spock[2], spock[1], spock[0]);
		vlf_texts[5] = vlfGuiAddUnicodeText(30, 195, STRING[STR_SYSTEM_INFORMATIONS_EEPROM_ACCESS], hardwares[hi].revision > 0x020100 ? STRING[STR_NO] : STRING[STR_YES]);

		vlf_texts[6] = vlfGuiAddUnicodeText(230, 70, STRING[STR_SYSTEM_INFORMATIONS_FUSE_ID], pspGetFuseId());
		vlf_texts[7] = vlfGuiAddUnicodeText(230, 95, STRING[STR_SYSTEM_INFORMATIONS_FUSE_CONFIG], pspGetFuseConfig());
		vlf_texts[8] = vlfGuiAddUnicodeText(230, 120, STRING[STR_SYSTEM_INFORMATIONS_NAND_SEED], pspNandGetScramble());
		vlf_texts[9] = vlfGuiAddUnicodeText(230, 145, STRING[STR_SYSTEM_INFORMATIONS_NAND_SIZE], nandsize / 1048576);
		vlf_texts[10] = vlfGuiAddUnicodeText(230, 170, STRING[STR_SYSTEM_INFORMATIONS_UMD_FIRMWARE], pspGetUMDFirmware(buf));
		vlf_texts[11] = vlfGuiAddUnicodeText(230, 195, STRING[STR_SYSTEM_INFORMATIONS_TIME_MACHINE], TimeMachine ? STRING[STR_YES] : STRING[STR_NO]);

		for (i = 0; i < VLF_ITEMS; i++)
		{
			if(vlf_texts[i] != NULL)
			{
				vlfGuiSetTextFontSize(vlf_texts[i], 0.75f);
			}
		}
	}
	else if(mode == 0x00000304)
	{
		SetTitle("video_plugin_videotoolbar", "tex_help_bar_icon", STRING[STR_MS_INFORMATIONS]);
		
		vlf_texts[0] = vlfGuiAddUnicodeText(30, 70, STRING[STR_MS_INFORMATIONS_BOOT_STATUS], GetMSInfo() < 0 ? 0 : MSInfo.BootStatus);
		vlf_texts[1] = vlfGuiAddUnicodeText(30, 95, STRING[STR_MS_INFORMATIONS_SIGNATURE], GetMSInfo() < 0 ? 0 : MSInfo.Signature);
		vlf_texts[2] = vlfGuiAddUnicodeText(30, 120, STRING[STR_MS_INFORMATIONS_PARTITION_TYPE], GetMSInfo() < 0 ? 0 : MSInfo.PartitionType);
		vlf_texts[3] = vlfGuiAddUnicodeText(30, 145, STRING[STR_MS_INFORMATIONS_STARTING_HEAD], GetMSInfo() < 0 ? 0 : MSInfo.StartHead);
		vlf_texts[4] = vlfGuiAddUnicodeText(30, 170, STRING[STR_MS_INFORMATIONS_LAST_HEAD], GetMSInfo() < 0 ? 0 : MSInfo.LastHead);
		vlf_texts[5] = vlfGuiAddUnicodeText(30, 195, STRING[STR_MS_INFORMATIONS_MS_SIZE], pspGetMsTotalSize() / 1048576);

		vlf_texts[6] = vlfGuiAddUnicodeText(230, 70, STRING[STR_MS_INFORMATIONS_IPL_SPACE], GetMSInfo() < 0 ? 0 : MSInfo.IPLSpace / 1024);
		vlf_texts[7] = vlfGuiAddUnicodeText(230, 95, STRING[STR_MS_INFORMATIONS_ABS_SECTOR], GetMSInfo() < 0 ? 0 : MSInfo.AbsSector);
		vlf_texts[8] = vlfGuiAddUnicodeText(230, 120, STRING[STR_MS_INFORMATIONS_TOTAL_SECTOR], GetMSInfo() < 0 ? 0 : MSInfo.TotalSectors);
		vlf_texts[9] = vlfGuiAddUnicodeText(230, 145, STRING[STR_MS_INFORMATIONS_START_SEC_CLU], GetMSInfo() < 0 ? 0 : MSInfo.StartSector);
		vlf_texts[10] = vlfGuiAddUnicodeText(230, 170, STRING[STR_MS_INFORMATIONS_LAST_SEC_CLU], GetMSInfo() < 0 ? 0 : MSInfo.LastSector);
		vlf_texts[11] = vlfGuiAddUnicodeText(230, 195, STRING[STR_MS_INFORMATIONS_FREE_MS_SIZE], pspGetMsFreeSpace() / 1048576);

		for (i = 0; i < VLF_ITEMS; i++)
		{
			if(vlf_texts[i] != NULL)
			{
				vlfGuiSetTextFontSize(vlf_texts[i], 0.75f);
			}
		}
	}
	else if(mode == 0x00000404)
	{
		SetTitle("video_plugin_videotoolbar", "tex_help_bar_icon", STRING[STR_BATTERY_INFORMATIONS]);
	
		if (pspGetBaryonVersion() <= 0x0022B200)
		{
			Serial = pspGetBatterySerial();
			sprintf(BatterySerial, "0x%08X", Serial);

			if(Serial == 0xFFFFFFFF)
			{
				sprintf(BatteryMode, STRING[STR_BATTERY_INFORMATIONS_MODE_PANDORA]);
			}
			else if(Serial == 0x00000000)
			{
				sprintf(BatteryMode, STRING[STR_BATTERY_INFORMATIONS_MODE_AUTOBOOT]);
			}
			else
			{
				sprintf(BatteryMode, STRING[STR_BATTERY_INFORMATIONS_MODE_NORMAL]);
			}
		}
		else if (pspGetBaryonVersion() >= 0x00234000)
		{
			sprintf(BatterySerial, "%s", STRING[STR_UNKNOWN]);
			sprintf(BatteryMode, STRING[STR_BATTERY_INFORMATIONS_MODE_UNSUPPORTED]);
		}

		vlf_texts[0] = vlfGuiAddUnicodeText(30, 70, STRING[STR_BATTERY_INFORMATIONS_STATUS], scePowerIsBatteryCharging() ? STRING[STR_BATTERY_INFORMATIONS_CHARGE] : STRING[STR_BATTERY_INFORMATIONS_IN_USE]);
		vlf_texts[1] = vlfGuiAddUnicodeText(30, 95, STRING[STR_BATTERY_INFORMATIONS_SOURCE], scePowerIsBatteryExist() ? STRING[STR_BATTERY_INFORMATIONS_BATTERY] : STRING[STR_BATTERY_INFORMATIONS_EXTERNAL]);
		vlf_texts[2] = vlfGuiAddUnicodeText(30, 120, STRING[STR_BATTERY_INFORMATIONS_LEVEL], scePowerGetBatteryLifePercent() < 0 ? 0 : scePowerGetBatteryLifePercent());
		vlf_texts[3] = vlfGuiAddUnicodeText(30, 145, STRING[STR_BATTERY_INFORMATIONS_HOURE_LEFT], scePowerGetBatteryLifeTime() < 0 ? 0 : (scePowerGetBatteryLifeTime() / 60), scePowerGetBatteryLifeTime() < 0 ? 0 : (scePowerGetBatteryLifeTime() - (scePowerGetBatteryLifeTime() / 60 * 60)));
		vlf_texts[5] = vlfGuiAddUnicodeText(30, 170, STRING[STR_BATTERY_INFORMATIONS_MODE], BatteryMode);

		vlf_texts[6] = vlfGuiAddUnicodeText(230, 70, STRING[STR_BATTERY_INFORMATIONS_REMAIN_CAPACITY], scePowerGetBatteryRemainCapacity() < 0 ? 0 : scePowerGetBatteryRemainCapacity());
		vlf_texts[7] = vlfGuiAddUnicodeText(230, 95, STRING[STR_BATTERY_INFORMATIONS_TOTAL_CAPACITY], scePowerGetBatteryFullCapacity() < 0 ? 0 : scePowerGetBatteryFullCapacity());
		vlf_texts[8] = vlfGuiAddUnicodeText(230, 120, STRING[STR_BATTERY_INFORMATIONS_TEMPERATURE], scePowerGetBatteryTemp() < 0 ? 0 : scePowerGetBatteryTemp());
		vlf_texts[9] = vlfGuiAddUnicodeText(230, 145, STRING[STR_BATTERY_INFORMATIONS_VOLTAGE], scePowerGetBatteryVolt() < 0 ? 0 : (float)scePowerGetBatteryVolt() / 1000.0);
		vlf_texts[10] = vlfGuiAddUnicodeText(230, 170, STRING[STR_BATTERY_INFORMATIONS_SERIAL], BatterySerial);

		for (i = 0; i < VLF_ITEMS; i++)
		{
			if(vlf_texts[i] != NULL)
			{
				vlfGuiSetTextFontSize(vlf_texts[i], 0.75f);
			}
		}
	}
}
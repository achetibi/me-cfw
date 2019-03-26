#include <pspsdk.h>
#include <pspkernel.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pspsysmem.h>
#include <pspctrl.h>
#include <pspreg.h>
#include <pspidstorage.h>
#include <pspnet.h>
#include <pspnet_adhoc.h>
#include <pspnet_adhocctl.h>
#include <psputility_netmodules.h>
#include <pspnand_driver.h>
#include <main.h>

PSP_MODULE_INFO("kpspident", 0x1007, 1, 2);
PSP_MAIN_THREAD_ATTR(0);

struct region_info {
	int model;
	u8 code;
//	char *region;
} region_info[] = {
	{ 0x00, 0x03 /*, "Japan"		*/ },
	{ 0x01, 0x04 /*, "America"		*/ },
	{ 0x02, 0x09 /*, "Australia"	*/ },
	{ 0x03, 0x07 /*, "England"		*/ },
	{ 0x04, 0x05 /*, "Europe"		*/ },
	{ 0x05, 0x06 /*, "Korea"		*/ },
	{ 0x06, 0x0A /*, "Hong Kong" 	*/ },
	{ 0x07, 0x0B /*, "Taiwan" 		*/ },
	{ 0x08, 0x0C /*, "Russia" 		*/ },
	{ 0x09, 0x0D /*, "China" 		*/ },
	{ 0x0A, 0x08 /*, "Mexico"		*/ },
};

struct hardware_info {
	u32 tachyon;
	u32 baryon;
	u32 pommel;
	int revision;
	int generation;
	char *mobo;
	char *version;
} hardware_info[] = {
	/*** PSP-100X **************************************************************/
	{ 0x00140000, 0x00030600, 0x00000103, 0x010100, 1, "TA-079 v1", "FAT v1.0" },
	{ 0x00200000, 0x00030600, 0x00000103, 0x010101, 1, "TA-079 v2", "FAT v1.1" },
	{ 0x00200000, 0x00040600, 0x00000103, 0x010102, 1, "TA-079 v3", "FAT v1.2" },
	{ 0x00300000, 0x00040600, 0x00000103, 0x010200, 1, "TA-081 v1", "FAT v2.0" },
	{ 0x00300000, 0x00040600, 0x00000104, 0x010201, 1, "TA-081 v2", "FAT v2.1" },
	{ 0x00400000, 0x00114000, 0x00000112, 0x010300, 1, "TA-082",    "FAT v3.0" },
	{ 0x00400000, 0x00121000, 0x00000112, 0x010400, 1, "TA-086",    "FAT v4.0" },
	/*** PSP-200X **************************************************************/
	{ 0x00500000, 0x0022B200, 0x00000123, 0x020100, 2, "TA-085 v1", "Slim v1.0" },
	{ 0x00500000, 0x00234000, 0x00000123, 0x020101, 2, "TA-085 v2", "Slim v1.1" },
	{ 0x00500000, 0x00243000, 0x00000123, 0x020200, 2, "TA-088 v1", "Slim v2.0" },
	{ 0x00500000, 0x00243000, 0x00000123, 0x020201, 2, "TA-088 v2", "Slim v2.1" },
	{ 0x00600000, 0x00243000, 0x00000123, 0x020202, 2, "TA-088 v3", "Slim v2.2" },
	{ 0x00500000, 0x00243000, 0x00000132, 0x020300, 2, "TA-090 v1", "Slim v3.0" },
	/*** PSP-300X ***************************************************************/
	{ 0x00600000, 0x00263100, 0x00000132, 0x030100, 3, "TA-090 v2", "Brite v1.0" },
	{ 0x00600000, 0x00263100, 0x00000133, 0x030101, 3, "TA-090 v3", "Brite v1.1" },
	{ 0x00600000, 0x00285000, 0x00000133, 0x030200, 3, "TA-092",    "Brite v2.0" },	
	{ 0x00810000, 0x002C4000, 0x00000141, 0x040300, 4, "TA-093 v1", "Brite v3.0" },
	{ 0x00810000, 0x002C4000, 0x00000143, 0x040301, 4, "TA-093 v2", "Brite v3.1" },
	{ 0x00810000, 0x002E4000, 0x00000154, 0x040400, 4, "TA-095 v1", "Brite v4.0" },
	{ 0x00820000, 0x002E4000, 0x00000154, 0x040401, 4, "TA-095 v2", "Brite v4.1" },
	/*** PSP-N100X **************************************************************/
	{ 0x00720000, 0x00304000, 0x00000133, 0x050100, 5, "TA-091",    "PSPgo v1.0" },
	{ 0x00800000, 0x002A0000, 0x00000000, 0x050200, 5, "TA-094",    "PSPgo v2.0" },
	/*** PSP-E100X **************************************************************/
	{ 0x00900000, 0x00403000, 0x00000154, 0x060100, 6, "TA-096",    "Street v1.0" },
	/*** DevKit ******************************************************************/
	{ 0x00100000, 0x00000000, 0x00000000, 0x000000, 0, "Devkit",    "Devkit" },
};

/*** Battery *********************************************************************/

u32 pspWriteBat(u8 addr, u16 data)
{
	int k1 = pspSdkSetK1(0);
	int res;
	u8 param[0x60];

	if (addr > 0x7F)
	{
		return(0x80000102);
	}

	param[0x0C] = 0x73;
	param[0x0D] = 5;
	param[0x0E] = addr;
	param[0x0F] = data;
	param[0x10] = data >> 8;

	res = sceSysconCmdExec(param, 0);
	if (res < 0)
	{
		return(res);
	}

	pspSdkSetK1(k1);
	return 0;
}

u32 pspReadBat(u8 addr)
{
	int k1 = pspSdkSetK1(0);
	int res;

	if (addr > 0x7F)
	{
		return 0x80000102;
	}

	u8 param[0x60];
	param[0x0C] = 0x74;
	param[0x0D] = 3;
	param[0x0E] = addr;

	res = sceSysconCmdExec(param, 0);
	if (res < 0)
	{
		return res;
	}

	pspSdkSetK1(k1);
	return ((param[0x21] << 8) | param[0x20]);
}

int pspErrCheck(u32 data)
{
	if ((data &0x80250000) == 0x80250000)
	{
		return -1;
	}
	else if (data &0xffff0000)
	{
		return ((data &0xffff0000) >> 16);
	}

	return 0;
}

int pspReadSerial(u16 *serial)
{
	int err = 0;
	u32 data;

	data = pspReadBat(0x07);
	err = pspErrCheck(data);
	if (!(err < 0))
	{
		serial[0] = (data &0xffff);
		data = pspReadBat(0x09);
		err = pspErrCheck(data);
		if (!(err<0))
		{
			serial[1] =  (data &0xffff);
		}
		else
		{
			err = data;
		}
	}
	else
	{
		err = data;
	}

	return err;
}

int pspWriteSerial(u16 *serial)
{
	int err = 0;

	err = pspWriteBat(0x07, serial[0]);
	if (!err)
	{
		err = pspWriteBat(0x09, serial[1]);
	}

	return err;
}

int pspReadEEPROM(int address, u16 *pdata)
{
	int err = 0;
	u32 data;

	data = pspReadBat(address);
	err = pspErrCheck(data);
	if (!(err<0))
	{
		pdata[0] = (data &0xffff);
	}
	else
	{
		err = data;
	}

	return err;
}

int pspWriteEEPROM(int address, u16 *pdata)
{
	int err = 0;
	err = pspWriteBat(address, pdata[0]);

	return err;
}

u32 pspGetBatterySerial()
{
	u16 Serial[2];
	pspReadSerial(Serial);

	return (Serial[1] &0xFFFF) + ((Serial[0] &0xFFFF) * 0x10000);
}

/*** IdStorage *********************************************************************/

int pspIdStorageLookup(u16 key, u32 offset, void *buf, u32 len)
{
	int k1 = pspSdkSetK1(0);

	memset(buf, 0, len);
	int ret = sceIdStorageLookup(key, offset, buf, len);

	pspSdkSetK1(k1);
	return ret;
}

int pspIdStorageReadLeaf(u16 key, void *buf)
{
	int k1 = pspSdkSetK1(0);

	memset(buf, 0, 512);
	int ret = sceIdStorageReadLeaf(key, buf);

	pspSdkSetK1(k1);
	return ret;
}

int pspIdStorageWriteLeaf(u16 key, void *buf)
{
	int k1 = pspSdkSetK1(0);
	char buf2[512];

	int err = sceIdStorageReadLeaf(key, buf2);
	if (err < 0)
	{
		sceIdStorageCreateLeaf(key);
	}

	int ret = sceIdStorageWriteLeaf(key, buf);
	sceIdStorageFlush();

	pspSdkSetK1(k1);
	return ret;
}

int pspIdStorageCreateLeaf(u16 key)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceIdStorageCreateLeaf(key);
	sceIdStorageFlush();

	pspSdkSetK1(k1);
	return ret;
}

int pspIdStorageDeleteLeaf(u16 key)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceIdStorageDeleteLeaf(key);
	sceIdStorageFlush();

	pspSdkSetK1(k1);
	return ret;
}

/*** Nand *********************************************************************/

int pspNandGetPageSize(void)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandGetPageSize();

	pspSdkSetK1(k1);
	return ret;
}

int pspNandGetPagesPerBlock(void)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandGetPagesPerBlock();

	pspSdkSetK1(k1);
	return ret;
}

int pspNandGetTotalBlocks(void)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandGetTotalBlocks();

	pspSdkSetK1(k1);
	return ret;
}

int pspNandIsBadBlock(u32 ppn)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandIsBadBlock(ppn);

	pspSdkSetK1(k1);
	return ret;
}

int pspNandLock(int writeflag)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandLock(writeflag);

	pspSdkSetK1(k1);
	return ret;
}

int pspNandReadBlockWithRetry(u32 ppn, void *buf, void *buf2)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandReadBlockWithRetry(ppn, buf, buf2);

	pspSdkSetK1(k1);
	return ret;
}

int pspNandReadId(void *buf, SceSize size)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandReadId(buf, size);

	pspSdkSetK1(k1);
	return ret;
}

int pspNandReadPages(u32 ppn, void *buf, void *buf2, u32 count)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandReadPages(ppn, buf, buf2, count);

	pspSdkSetK1(k1);
	return ret;
}

int pspNandReadStatus(void)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandReadStatus();

	pspSdkSetK1(k1);
	return ret;
}

int pspNandReset(int flag)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandReset(flag);

	pspSdkSetK1(k1);
	return ret;
}

int pspNandSetWriteProtect(int protectFlag)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandSetWriteProtect(protectFlag);

	pspSdkSetK1(k1);
	return ret;
}

void pspNandUnlock(void)
{
	int k1 = pspSdkSetK1(0);
	sceNandUnlock();

	pspSdkSetK1(k1);
}

int pspNandSetScramble(u32 magic)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandSetScramble(magic);

	pspSdkSetK1(k1);
	return ret;
}

u32 pspNandGetScramble()
{
	int k1 = pspSdkSetK1(0);

	u32 magic;
	u32 buf[4];
	u32 sha[5];

	buf[0] = *(vu32 *)(0xBC100090);
	buf[1] = *(vu32 *)(0xBC100094);
	buf[2] = *(vu32 *)(0xBC100090) << 1;
	buf[3] = 0xD41D8CD9;

	sceKernelUtilsSha1Digest((u8 *)buf, sizeof(buf), (u8 *)sha);
	magic = (sha[0] ^ sha[3]) + sha[2];

	pspSdkSetK1(k1);
	return magic;
}

int pspNandReadAccess(u32 page, void* buffer)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandReadAccess(page, buffer, NULL, 1, 49);

	pspSdkSetK1(k1);
	return ret;
}

int pspNandReadExtraOnly(u32 page, void* buffer)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceNandReadExtraOnly(page, buffer, 1);

	pspSdkSetK1(k1);
	return ret;
}

int pspReadNandBlock(u32 page, u8 *buffer)
{
	u32 i, j;
	int k1 = pspSdkSetK1(0);

	if(sceNandIsBadBlock(page) == 0)
	{
		for (i = 0; i < pspNandGetPagesPerBlock(); i++)
		{
			for (j = 0; j < 4; j++)
			{
				pspNandReadAccess(page, buffer);
				pspNandReadExtraOnly(page, buffer + 512);
			}

			page++;
			buffer += 528;
		}
		
		pspSdkSetK1(k1);
	}
	else
	{
		pspSdkSetK1(k1);
		return -1;
	}
	
	return 0;
}

/*** System Information *********************************************************************/

u32 pspGetTachyonVersion()
{
	int k1 = pspSdkSetK1(0);
	u32 ver = sceSysregGetTachyonVersion();

	pspSdkSetK1(k1);
	return ver;
}

u32 pspGetBaryonVersion()
{
	int k1 = pspSdkSetK1(0);
	u32 ver;
	sceSysconGetBaryonVersion(&ver);

	pspSdkSetK1(k1);
	return ver;
}

u32 pspGetPommelVersion()
{
	int k1 = pspSdkSetK1(0);
	u32 ver;
	sceSysconGetPommelVersion(&ver);

	pspSdkSetK1(k1);
	return ver;
}

u64 pspGetFuseId()
{
	return sceSysregGetFuseId();
}

u32 pspGetFuseConfig()
{
	return sceSysregGetFuseConfig();
}

u32 pspGetKirkVersion()
{
	int k1 = pspSdkSetK1(0);

	sceSysregKirkBusClockEnable();
	sceKernelDelayThread(1000);
	int err = *(u32 *)0xBDE00004;

	pspSdkSetK1(k1);
	return err;
}

u32 pspGetSpockVersion()
{
	int k1 = pspSdkSetK1(0);

	sceSysregAtaBusClockEnable();
	sceKernelDelayThread(1000);
	int err = *(u32 *)0xBDF00004;

	pspSdkSetK1(k1);
	return err;
}

int pspKernelGetModel()
{
	return sceKernelGetModel();
}

char *pspGetShippedFirmware(char *buf)
{
	pspIdStorageLookup(0x51, 0, buf, 5);

	if (buf[0] == 0)
	{
		sprintf(buf, "-");
	}

	return buf;
}

int pspGetRegionInfo()
{
	int i;
	u8 region[1];
	pspIdStorageLookup(0x100, 0x3D, region, 1);

	for (i = 0; i < sizeof(region_info) / sizeof(region_info[0]); i++)
	{
		if (region_info[i].code == region[0])
		{
			return i;
		}
	}

	return -1;
}

int pspGetHardwareInfo()
{
	int i;
	u32 tachyon = pspGetTachyonVersion();
	u32 baryon = pspGetBaryonVersion();
	u32 pommel = pspGetPommelVersion();

	for (i = 0; i < sizeof(hardware_info) / sizeof(hardware_info[0]); i++)
	{
		if (hardware_info[i].tachyon == tachyon && hardware_info[i].baryon == baryon && hardware_info[i].pommel == pommel)
		{
			return i;
		}
	}

	return -1;
}

u8 *pspGetMACAddress(u8 *buf)
{
	sceWlanGetEtherAddr(buf);

	return buf;
}

u32 pspGetMsTotalSize()
{
	int k1 = pspSdkSetK1(0);
	SystemDevCtl devctl;
	SystemDevCommand command;
	memset(&devctl, 0, sizeof(SystemDevCtl));
	command.devinf = &devctl;
	sceIoDevctl("ms0:", 0x02425818, &command, sizeof(SystemDevCommand), NULL, 0);
	u64 mssize = (devctl.maxclusters * devctl.sectorcount) * devctl.sectorsize; 

	pspSdkSetK1(k1);
	return (u32)mssize;
}

u32 pspGetMsFreeSpace()
{
	int k1 = pspSdkSetK1(0);
	SystemDevCtl devctl;
	SystemDevCommand command;

	memset(&devctl, 0, sizeof(SystemDevCtl));
	command.devinf = &devctl;
	sceIoDevctl("ms0:", 0x02425818, &command, sizeof(SystemDevCommand), NULL, 0);
	u64 freesize = (devctl.freeclusters * devctl.sectorcount) * devctl.sectorsize; 

	pspSdkSetK1(k1);
	return (u32)freesize;
}

int pspUmdExecInquiryCmd(void *drive, u8 *param, u8 *buf)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceUmdExecInquiryCmd(drive, param, buf);

	pspSdkSetK1(k1);
	return ret;
}

void *pspUmdManGetUmdDrive(int driveNum)
{
	int k1 = pspSdkSetK1(0);
	void *ret = (void*)sceUmdManGetUmdDrive(driveNum);

	pspSdkSetK1(k1);
	return ret;
}

char *pspGetUMDFirmware(char *buf)
{
	u8 buf2[0x60];
	u8 param[4] = {0, 0, 0x60, 0};

	int ret = pspUmdExecInquiryCmd(pspUmdManGetUmdDrive(0), param, buf2);
	sprintf(buf, ret < 0 ? "-" : "%c%c%c%c%c", buf2[36], buf2[37], buf2[38], buf2[39], buf2[40]);

	return buf;
}

int pspGetKeyPress(int wait, int milisecs)
{
	int k1 = pspSdkSetK1(0);
	SceCtrlData pad;
	int btn = 0;

	while(!btn)
	{
		sceCtrlSetSamplingCycle(0);
		sceCtrlSetSamplingMode(1);
		sceCtrlReadBufferPositive(&pad, 1);
		btn = pad.Buttons &0xFFFF;
		if(!wait)
		{
			break;
		}
		sceKernelDelayThread(milisecs * 10);
	}

	pspSdkSetK1(k1);
	return btn;
}

/*** Start/Stop *********************************************************************/

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop()
{
	return 0;
}
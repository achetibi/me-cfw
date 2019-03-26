#ifndef __PSPINFO_H__
#define __PSPINFO_H__

typedef struct MSStruct
{
	u8 BootStatus;
	u8 StartHead;
	u16 StartSector;
	u8 PartitionType;
	u8 LastHead;
	u16 LastSector;
	u32 AbsSector;
	u32 TotalSectors;
	u16 Signature;
	int IPLSpace;
	u32 SerialNum;
	char Label[12];
	char FileSystem[9];
	char IPLName[50];
	int IPLSize;
} MSStruct;
MSStruct MSInfo;

int GetMSInfo();
int pspGetFreeRam();
void SystemInfoPage();

#endif


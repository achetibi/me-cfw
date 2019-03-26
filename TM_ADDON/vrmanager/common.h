#ifndef __COMMON_H__
#define __COMMON_H__

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
int Rand(int min, int max);
int FileExists(const char *filepath, ...);
int DirExists(char *dirpath, ...);
int GetFileSize(const char *filepath, ...);
int ReadFile(char *file, int seek, u8 *buf, int size);
int WriteFile(char *file, u8 *buf, int size);
int UTF82Unicode(char *src, char *dst);
char *GetString(char *buf, u16 str);

#endif


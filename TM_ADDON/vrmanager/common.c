#include <pspsdk.h>
#include <pspkernel.h>
#include <malloc.h>
#include <pspreg.h>
#include <psprtc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "trans.h"
#include "common.h"

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

int Rand(int min, int max)
{
	u64 tick;
	SceKernelUtilsMt19937Context ctx;
	sceRtcGetCurrentTick(&tick);
	sceKernelUtilsMt19937Init(&ctx, (u32)tick);

	return min + (sceKernelUtilsMt19937UInt(&ctx) % max);
}

int FileExists(const char *filepath, ...)
{
	SceIoStat stat;
	int ret;
	va_list list;
	char file[128];	

	va_start(list, filepath);
	vsprintf(file, filepath, list);
	va_end(list);

	ret = sceIoGetstat(file, &stat);

	return ret == 0 ? 1 : 0;
}

int DirExists(char *dirpath, ...)
{
	va_list list;
	char dir[128];	

	va_start(list, dirpath);
	vsprintf(dir, dirpath, list);
	va_end(list);

	SceUID d = sceIoDopen(dir);
	if(d < 0)
	{
		return 0;
	}

	sceIoClose(d);

	return 1;
}

int GetFileSize(const char *filepath, ...)
{
	int size;
	SceIoStat stat;
	va_list list;
	char file[128];	

	va_start(list, filepath);
	vsprintf(file, filepath, list);
	va_end(list);

	sceIoGetstat(file, &stat);
	size = stat.st_size;

	return size;
}

int ReadFile(char *file, int seek, u8 *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	if(fd < 0)
	{
		return fd;
	}

	if(seek > 0)
	{
		if(sceIoLseek(fd, seek, PSP_SEEK_SET) != seek)
		{
			sceIoClose(fd);
			return -1;
		}
	}

	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return read;
}

int WriteFile(char *file, u8 *buf, int size)
{
	int i, pathlen = 0;
	char dirpath[128];

	for(i = 1; i < (strlen(file)); i++)
	{
		if(strncmp(file + i - 1, "/", 1) == 0)
		{
			pathlen = i - 1;
			strncpy(dirpath, file, pathlen);
			dirpath[pathlen] = 0;
			sceIoMkdir(dirpath, 0777);
		}
	}

	SceUID fd = sceIoOpen(file, PSP_O_WRONLY|PSP_O_CREAT|PSP_O_TRUNC, 0777);
	if(fd < 0)
	{
		return fd;
	}

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return written;
}

int UTF82Unicode(char *src, char *dst)
{
	int i, x;
	unsigned char *usrc = (unsigned char *)src;

	for(i = 0, x = 0; usrc[i];)
	{
		char ch;

		if ((usrc[i] &0xE0) == 0xE0)
		{
			ch = ((usrc[i] &0x0F) << 12) | ((usrc[i + 1] &0x3F) << 6) | (usrc[i + 2] &0x3F);
			i += 3;
		}

		else if ((usrc[i] &0xC0) == 0xC0)
		{
			ch = ((usrc[i] &0x1F) << 6) | (usrc[i + 1] &0x3F);
			i += 2;
		}

		else
		{
			ch = usrc[i];
			i += 1;
		}

		dst[x++] = ch;
	}

	dst[x++] = '\0';

	return x;
}

char *GetString(char *buf, u16 str)
{
	char *p = strrchr(buf, str);

	if (!p)
	{
		return NULL;
	}

	return p + 1;
}
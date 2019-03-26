#include <pspsdk.h>
#include <pspkernel.h>
#include <pspreg.h>
#include <psprtc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "trans.h"
#include "common.h"

extern const char **g_messages;

int AssignFlash()
{
	int res = sceIoAssign("flach0:", "lflach0:0,0", "flachfat0:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ASSIGN_FLASH_FAILED], 0, res);
	}

	res = sceIoAssign("flach1:", "lflach0:0,1", "flachfat1:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ASSIGN_FLASH_FAILED], 1, res);
	}

	res = sceIoAssign("flach2:", "lflach0:0,2", "flachfat2:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ASSIGN_FLASH_FAILED], 2, res);
	}

	res = sceIoAssign("flach3:", "lflach0:0,3", "flachfat3:", IOASSIGN_RDWR, NULL, 0);
	if (res < 0)
	{
		ErrorReturn(1, 1, STRING[STR_ASSIGN_FLASH_FAILED], 3, res);
	}

	return res;
}

int UnassignFlash()
{
	int res = sceIoUnassign("flach0:");
	if(res < 0)
	{
		return res;
	}

	res = sceIoUnassign("flach1:");
	if(res < 0)
	{
		return res;
	}

	res = sceIoUnassign("flach2:");
	if(res < 0)
	{
		return res;
	}

	res = sceIoUnassign("flach3:");
	if(res < 0)
	{
		return res;
	}

	return res;
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

int pspGetRegistryValue(const char *dir, const char *name, void *buf, int bufsize)
{
	int ret = 0;
	struct RegParam reg;
	REGHANDLE h;

	memset(&reg, 0, sizeof(reg));
	reg.regtype = 1;
	reg.namelen = strlen("/system");
	reg.unk2 = 1;
	reg.unk3 = 1;
	strcpy(reg.name, "/system");
	if(sceRegOpenRegistry(&reg, 2, &h) == 0)
	{
		REGHANDLE hd;
		if(!sceRegOpenCategory(h, dir, 2, &hd))
		{
			REGHANDLE hk;
			unsigned int type, size;

			if(!sceRegGetKeyInfo(hd, name, &hk, &type, &size))
			{
				if(!sceRegGetKeyValue(hd, hk, buf, bufsize))
				{
					ret = (int)buf;
					sceRegFlushCategory(hd);
				}
			}
			sceRegCloseCategory(hd);
		}
		sceRegFlushRegistry(h);
		sceRegCloseRegistry(h);
	}

	return ret;
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
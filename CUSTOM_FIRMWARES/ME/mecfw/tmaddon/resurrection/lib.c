#include <pspkernel.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

int ReadFile(const char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	int read = sceIoRead(fd, buf, size);
	
	sceIoClose(fd);

	return read;
}

int WriteFile(const char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	
	if (fd < 0)
		return fd;

	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);

	return written;
}

int limit(int val,int min,int max)
{
	if(val<min) val = max;
	if(val>max) val = min;

	return val;
}

int limit2(int val,int min,int max)
{
	if(val<min) val = min;
	if(val>max) val = max;

	return val;
}

#ifndef __GALAXY_DRIVER_H__
#define __GALAXY_DRIVER_H__

#include "inline.h"

#define SECTOR_SIZE	0x800

enum {
	ISO,
	CSO,
	DAX
};

typedef struct
{
  int   fpointer;
  void  *buf;
  int   read_size;
} NP9660ReadParams;

int sub_00000588();
int umd9660_read(int lba , u8 *buf, int read_size);
int sceIoClosePatched(SceUID fd);

int OpenIso();
int ReadUmdFileRetry(void *buf, int size, int fpointer);
int umd9660_read2(void *a0);
int GetIsoDiscSize();

char *sctrlSEGetUmdFile();
void *sctrlKernelMalloc(size_t size);



#endif


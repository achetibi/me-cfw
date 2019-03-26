#ifndef __UMD9660_DRIVER_H__
#define __UMD9660_DRIVER_H__

#define SECTOR_SIZE	0x800

//char *GetUmdFile();
//void SetUmdFile(char *file);
int  OpenIso();
int  ReadUmdFileRetry(void *buf, int size, int fpointer);
int  Umd9660ReadSectors(int lba, int nsectors, void *buf);
PspIoDrv *getumd9660_driver();
void DoAnyUmd();
int  Umd9660ReadSectors2(int lba, int nsectors, void *buf); // MIA


typedef struct
{
  int   unknown1;
  int   cmd;
  int   lba_top;
  int   lba_size;
  int   byte_size_total;
  int   byte_size_centre;
  int   byte_size_start;
  int   byte_size_last;
} LbaParams;

typedef struct
{
  void *drvState;
  u8 *buf;
  SceInt64 read_size;
  LbaParams *lbaparams;
} UmdUnknownParam1;


void *sceUmdManGetUmdDiscInfoPatched();
void DoNoUmdPatches();


#endif


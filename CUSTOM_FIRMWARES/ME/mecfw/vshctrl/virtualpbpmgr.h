#ifndef __VIRTUALPBPMGR_H__
#define __VIRTUALPBPMGR_H__

#include "isofs_driver.h"

#define ISOFILE_MAX (112 - 16)

typedef struct
{
	u32  header[10];//0
	char isofile[ ISOFILE_MAX ];//40	
	char discid[12];//152
	char system_ver[4];
	int parental_level;
	int opnssmp_type;//164
	char sfotitle[64 + 8 ];//168
	int psfo_lba;//232
	int psfo_size;//236
	int i0png_lba; //240
	int i0png_size;//244
	int i1pmf_lba ;//248
	int i1pmf_size;//252
	int p0png_lba;//256
	int p0png_size;//260
	int p1png_lba;//264
	int p1png_size;//268
	int s0at3_lba;//272
	int s0at3_size;//276
	int filesize;//280
	int filepointer;//284
	ScePspDateTime mtime;//288
} VirtualPbp;

typedef struct
{
	int dread;
	int deleted;//4
	int psdirdeleted;
} InternalState;

typedef struct {
	char FileName[13];
	char LongName[256];
}SceFatMsDirentPrivate;

typedef struct {
	SceSize size;
	char FileName[16];
	char LongName[1024];
}SceFatMsDirentPrivateKernel;

int virtualpbp_init();
int virtualpbp_exit();
int virtualpbp_reset();
int virtualpbp_add(char *isofile, ScePspDateTime *mtime, VirtualPbp *res);
int virtualpbp_fastadd(VirtualPbp *pbp);
int virtualpbp_open(int i);
int virtualpbp_close(SceUID fd);
int virtualpbp_read(SceUID fd, void *data, SceSize size);
int virtualpbp_lseek(SceUID fd, SceOff offset, int whence);
int virtualpbp_getstat(int i, SceIoStat *stat);
int virtualpbp_chstat(int i, SceIoStat *stat, int bits);
int virtualpbp_remove(int i);
int virtualpbp_rmdir(int i);
int virtualpbp_dread(SceUID fd, SceIoDirent *dir);
char *virtualpbp_getfilename(int i);
int virtualpbp_get_isotype(int i);

void getlba_andsize(PspIoDrvFileArg *arg, const char *file, int *lba, int *size);
const char *init_version_str();

#endif


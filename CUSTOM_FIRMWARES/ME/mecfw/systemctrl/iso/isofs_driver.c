#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../malloc.h"

#include "psperror.h"

#include "isofs_driver.h"


#define MAX_HANDLERS	33
#define SIZE_OF_SECTORS	(SECTOR_SIZE * 8) + 64

static Iso9660DirectoryRecord main_record;
static FileHandle handlers[MAX_HANDLERS];
static u8 *sectors;

static SceUID isofs_sema;
static int g_lastLBA;
static int g_lastReadSize;
//int increased = 0;

int IsofsReadSectors(int lba, int nsectors, void *buf)
{
	if (buf == sectors/*dataAC60*/)
	{
		if (nsectors > 8)
		{
			return SCE_ERROR_ERRNO_EFBIG;
		}

		memset(sectors + (nsectors * SECTOR_SIZE), 0, 64);
	}

	return Umd9660ReadSectors2(lba, nsectors, buf);//loc_0000525C
}

static void UmdNormalizeName(char *filename)
{
	char *p = strstr(filename, ";1");

	if (p)
	{
		*p = 0;
	}
}

static inline int SizeToSectors(int size)
{
	int len = size / SECTOR_SIZE;

	if ((size % SECTOR_SIZE) != 0)
	{
		len++;
	}

	return len;
}

static int GetFreeHandle()
{
	int i;

	// Lets ignore handler 0 to avoid problems with bad code...
	for (i = 1; i < MAX_HANDLERS; i++)
	{
		if (handlers[i].opened == 0)
			return i;
	}

	return SCE_ERROR_ERRNO_EMFILE;//0x80010018
}

//sub_000035BC:
int GetPathAndName(char *fullpath, char *path, char *filename)
{
	char fullpath2[256];

	strcpy(fullpath2, fullpath);
	
	if (fullpath2[strlen(fullpath2)-1] == '/')
	{
		fullpath2[strlen(fullpath2)-1] = 0;
	}

	char *p = strrchr(fullpath2, '/');
	
	if (!p)
	{
		if (strlen(fullpath2)+1 > 32)
		{
			/* filename too big for ISO9660 */
			return SCE_ERROR_ERRNO_ENAMETOOLONG;
		}

		memset(path, 0, 256);
		UmdNormalizeName(fullpath2);//sub_00003460
		strcpy(filename, fullpath2);
		
		return 0;
	}

	if (strlen(p+1)+1 > 32)
	{
		/* filename too big for ISO9660 */
		return SCE_ERROR_ERRNO_ENAMETOOLONG;
	}

	strcpy(filename, p+1);
	p[1] = 0;

	if (fullpath2[0] == '/')
		strcpy(path, fullpath2+1);
	else
		strcpy(path, fullpath2);

	UmdNormalizeName(filename);
	
	return 0;
}

//sub_00003E14:
int FindFileLBA(char *filename, int lba, int dirSize, int isDir, Iso9660DirectoryRecord *retRecord)
{
	Iso9660DirectoryRecord *record;
	char name[32];
	u8 *p; 
	int oldDirLen = 0;
	int pos, res;
	int remaining = 0;

	pos = lba * SECTOR_SIZE;

	int sec_size = SizeToSectors(dirSize);

	if (sec_size <= 8)//sub_00002C80
	{
		res = IsofsReadSectors(lba, sec_size, sectors);	//sub_000036E4
	}
	else
	{
		remaining = sec_size - 8;
		res = IsofsReadSectors(lba, 8, sectors );
	}
	
	if (res < 0)
	{
		return res;
	}

	p = sectors;
	record = (Iso9660DirectoryRecord *)p;
	
	while (1)
	{
		if (record->len_dr == 0)
		{
			if (SECTOR_SIZE - (pos % SECTOR_SIZE) <= oldDirLen)
			{
				p += (SECTOR_SIZE - (pos % SECTOR_SIZE));
				pos += (SECTOR_SIZE - (pos % SECTOR_SIZE));				
				record = (Iso9660DirectoryRecord *)p;

				if (record->len_dr == 0)
				{
					return SCE_ERROR_ERRNO_ENOENT;
				}
			}

			else
			{
				return SCE_ERROR_ERRNO_ENOENT;
			}		
		}

		if (record->len_fi > 32)
		{
			return SCE_ERROR_ERRNO_EINVAL;
		}

		if (record->fi == 0)
		{
			if (strcmp(filename, ".") == 0)
			{
				memcpy(retRecord, record, sizeof(Iso9660DirectoryRecord));
				return record->lsbStart;
			}
		}

		else if(record->fi == 1)
		{
			if (strcmp(filename, "..") == 0)
			{
				memcpy(retRecord, record, sizeof(Iso9660DirectoryRecord));
				return record->lsbStart;
			}
		}

		else
		{
			memset(name, 0, 32);
			memcpy(name, &record->fi, record->len_fi);
			UmdNormalizeName(name);

			if (strcmp(name, filename) == 0)
			{
				if (isDir)
				{
					if (!(record->fileFlags & ISO9660_FILEFLAGS_DIR))
					{
						return SCE_ERROR_ERRNO_ENOTDIR;
					}
				}				

				memcpy(retRecord, record, sizeof(Iso9660DirectoryRecord));
				return record->lsbStart;
			}
		}

		pos += record->len_dr;
		p += record->len_dr;
		oldDirLen = record->len_dr;
		record = (Iso9660DirectoryRecord *)p;
		
		if (remaining > 0)
		{
			int offset = (p - sectors);

			if ((offset + sizeof(Iso9660DirectoryRecord) + 0x60) >= (8 * SECTOR_SIZE))
			{
				lba += (offset / SECTOR_SIZE);
				res = IsofsReadSectors(lba, 8, sectors);

				if (res < 0)
				{
					return res;
				}
				
				if (offset >= (8 * SECTOR_SIZE))
				{
					remaining -= 8;
				}
				else
				{
					remaining -= 7;
				}

				p = sectors + (offset % SECTOR_SIZE);
				record = (Iso9660DirectoryRecord *)p;
			}
		}
	}

	return -1;
}

//sub_0000417C:
int FindPathLBA(char *path, Iso9660DirectoryRecord *retRecord)
{
	char *p, *curpath;
	char curdir[32];
	int lba;
	int level = 0;
	
	lba = main_record.lsbStart;	
	memcpy(retRecord, &main_record, sizeof(Iso9660DirectoryRecord));
	
	p = strchr(path, '/');
	curpath = path;

	while (p)
	{
		if (p-curpath+1 > 32)
		{
			return SCE_ERROR_ERRNO_ENAMETOOLONG;
		}

		memset(curdir, 0, sizeof(curdir));
		strncpy(curdir, curpath, p-curpath);

		if (strcmp(curdir, ".") == 0)
		{

		}
		else if (strcmp(curdir, "..") == 0)
		{
			level--;
		}
		else
		{
			level++;
		}

		if (level > 8)
		{
			return SCE_ERROR_ERRNO_EINVAL;
		}
		
		lba = FindFileLBA(curdir, lba, retRecord->lsbDataLength, 1, retRecord);

		if (lba < 0)
			return lba;
		
		curpath = p+1;
		p = strchr(curpath, '/');		
	}
	
	return lba;
}

//0x00003CCC
int isofs_init(PspIoDrvArg* arg)
{
	int res;

	//increased = 0;

	//sectors = (u8 *)malloc(SIZE_OF_SECTORS);
	sectors = (u8 *)sctrlKernelMalloc(SIZE_OF_SECTORS);
	if (!sectors)
	{
		return -1;
	}

	isofs_sema = sceKernelCreateSema("", 0, 1, 1, NULL);//EcsIsofsMgr
	if (isofs_sema < 0)//dataAC64
	{
		return isofs_sema;
	}

	memset(sectors, 0, SIZE_OF_SECTORS);//16448

	res = IsofsReadSectors(0x10, 1, sectors);//sub_000036E4
	if (res < 0)
	{
		return res;
	}
	
	if (memcmp(sectors+1, "CD001", 5) != 0)
	{
		return -1;
	}	

//	memcpy(&dataB090, sectors+0x9C, sizeof(Iso9660DirectoryRecord));
	memcpy(&main_record, sectors+0x9C, sizeof(Iso9660DirectoryRecord));

	memset(handlers, 0, sizeof(handlers));//sizeof(handlers) = 1024

	g_lastLBA = -1;//dataAC68
	g_lastReadSize = 0;	//dataB0B4
	
	return 0;
}

/* fast init, just for direct lba access purposes */
int isofs_fastinit()
{
	//increased = 0;
	
	sectors = (u8 *)sctrlKernelMalloc(SIZE_OF_SECTORS);
	if (!sectors)
	{
		return -1;
	}

	memset(&main_record, 0, sizeof(Iso9660DirectoryRecord));//dataAC60
	memset(handlers, 0, sizeof(handlers));//dataAC6C

	g_lastLBA = -1;//dataAC68
	g_lastReadSize = 0;//dataB0B4

	isofs_sema = sceKernelCreateSema("", 0, 1, 1, NULL);//EcsIsofsMgr
	if (isofs_sema < 0)//dataAC64
	{
		return isofs_sema;
	}
	
	return 0;
}
//0x00003488: 
int isofs_exit(PspIoDrvArg* arg)
{
	g_lastLBA = -1;//dataAC68
	g_lastReadSize = 0;	//dataB0B4
	
	if (isofs_sema >= 0)//dataAC64
	{
		sceKernelWaitSema(isofs_sema, 1, NULL);	
	}
	
	if (sectors)//dataAC60
	{
		sctrlKernelFree(sectors);
		//free(sectors);
		sectors = NULL;
	}

	if (isofs_sema >= 0)//dataAC64
	{
		sceKernelDeleteSema(isofs_sema);
		isofs_sema = -1;
	}

	//increased = 0;
	
	return 0;
}

//0x00002CD8:
int isofs_exit2(PspIoDrvArg* arg)
{
	return 0;
}
//0x000033E4:
int isofs_reset()
{
	sceKernelWaitSema(isofs_sema, 1, NULL);	

	memset(&main_record, 0, sizeof(Iso9660DirectoryRecord));
	memset(handlers, 0, sizeof(handlers));

	g_lastLBA = -1;//AC68
	g_lastReadSize = 0;//B0B4

	sceKernelSignalSema(isofs_sema, 1);	

	return 0;
}



typedef struct
{
	PspIoDrvFileArg *arg; 
	char *file;
	int flags;
	SceMode mode;
} OpenParams;

OpenParams open_params;//dataB06C

//sub_00004650:
int isofs_open2()
{
	Iso9660DirectoryRecord record;
	char fullpath[256], path[256], filename[32];
	
	int res, lba, i;
	int notallowedflags = PSP_O_WRONLY | PSP_O_APPEND | PSP_O_CREAT | PSP_O_TRUNC | PSP_O_EXCL;
	
	//Kprintf("Open %s\n", open_params.file);

	if (!open_params.file || !open_params.arg)
	{
		return SCE_ERROR_ERRNO_EINVAL;
	}	

	if (strcmp(open_params.file, "/") == 0)
	{
		i = GetFreeHandle();//sub_00002CA0
		if (i < 0)
		{
			return i;
		}

		handlers[i].opened = 1;
		handlers[i].lba = main_record.lsbStart;
		handlers[i].filesize = main_record.lsbDataLength;
		handlers[i].filepointer = 0;
		handlers[i].attributes = main_record.fileFlags;

		open_params.arg->arg = (void *)i;
		return 0;
	}

	memset(fullpath, 0, 256);
	strncpy(fullpath, open_params.file, 256);

	if (fullpath[strlen(fullpath)-1] == '/')
	{
		fullpath[strlen(fullpath)-1] = 0;
	}

	if (strlen(fullpath)+1 > 256)
	{
		// path too big for ISO9660 
		//Kprintf("Error open 1 %s\n", open_params.file);
		return SCE_ERROR_ERRNO_ENAMETOOLONG;
	}
	
	if (open_params.arg->fs_num >= 1)
	{
		//Kprintf("Error open 2 %s\n", open_params.file);
		return SCE_ERROR_ERRNO_EINVAL;
	}
	
	if (open_params.flags & notallowedflags)
	{
		//Kprintf("Error open 3 %s\n", open_params.file);
		return SCE_ERROR_ERRNO_EFLAG;
	}	

	if (strncmp(fullpath, "/sce_lbn", 8) != 0)
	{
		if ((res = GetPathAndName(fullpath, path, filename)) < 0)//sub_000035BC
		{
			return res;
		}

		if (path[0])
		{
			lba = FindPathLBA(path, &record);//sub_0000417C		 
		}
		else
		{
			memcpy(&record, &main_record, sizeof(Iso9660DirectoryRecord));
			lba = record.lsbStart;		
		}

		if (lba < 0)
		{
			return lba;
		}

		lba =  FindFileLBA(filename, lba, record.lsbDataLength, 0, &record);
		if (lba < 0)
		{
			//Kprintf("Error open lba %s\n", open_params.file);
			return lba;
		}
		
		i = GetFreeHandle();
		if (i < 0)
		{
			//Kprintf("Error open free %s\n", open_params.file);
			return i;
		}

		handlers[i].opened = 1;
		handlers[i].lba = lba;
		handlers[i].filesize = record.lsbDataLength;
		handlers[i].filepointer = 0;
		handlers[i].attributes = record.fileFlags;	
	}
	else
	{
		// lba  access
		char str[11];
		char *p;
		int  size;
		
		p = strstr(fullpath, "_size");

		if (!p)
		{
			//Kprintf("Error open a %s\n", open_params.file);
			return SCE_ERROR_ERRNO_EINVAL;
		}

		if ((p-(fullpath+8)) > 10)
		{
			//Kprintf("Error open b %s\n", open_params.file);
			return SCE_ERROR_ERRNO_ENAMETOOLONG;
		}

		memset(str, 0, 11);
		strncpy(str, fullpath+8, p-(fullpath+8));
		
		lba = strtol(str, NULL, 0);
		if (lba < 0)
		{
			//Kprintf("Error open c %s\n", open_params.file);
			return SCE_ERROR_ERRNO_EINVAL;
		}

		if ((p+strlen(p)-(p+5)) > 10)
		{
			//Kprintf("Error open d %s\n", open_params.file);
			return SCE_ERROR_ERRNO_ENAMETOOLONG;
		}

		memset(str, 0, 11);
		strncpy(str, p+5, p+strlen(p)-(p+5));

		size = strtol(str, NULL, 0);
		if (size < 0)
		{
			//Kprintf("Error open e %s\n", open_params.file);
			return SCE_ERROR_ERRNO_EINVAL;
		}

		i = GetFreeHandle();
		if (i < 0)
		{
			//Kprintf("Error open 7 %s\n", open_params.file);
			return i;
		}

		handlers[i].opened = 1;
		handlers[i].lba = lba;
		handlers[i].filesize = size;
		handlers[i].filepointer = 0;
		handlers[i].attributes = 0x01;		

		//Kprintf("lba opened %08X\n", lba);
	}

	open_params.arg->arg = (void *)i;	
	return 0;
}

//; 0x000032A0:
int isofs_open(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	sceKernelWaitSema(isofs_sema, 1, NULL);	

	open_params.arg = arg;
	open_params.file = file;
	open_params.flags = flags;
	open_params.mode = mode;
	
	int res = sceKernelExtendKernelStack(0x1800,(void *)isofs_open2 /*sub_00004650*/, NULL);

	sceKernelSignalSema(isofs_sema, 1);
	return res;
}
//; 0x00003070: 
int isofs_close(PspIoDrvFileArg *arg)
{
	SceUID fd = (SceUID)arg->arg;
	sceKernelWaitSema(isofs_sema, 1, NULL);

	int ret = 0;

	if (fd < 0 || fd >= MAX_HANDLERS) {
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_EBADF;//0x80010009
		ret = SCE_ERROR_ERRNO_EBADF;
		goto RETURN_LABEL;
	}

	if (!handlers[fd].opened) {
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_EBADF;//0x80010009
		ret = SCE_ERROR_ERRNO_EBADF;
		goto RETURN_LABEL;
	}

	handlers[fd].opened = 0;
	
RETURN_LABEL:
	sceKernelSignalSema(isofs_sema, 1);
	return ret;
}


typedef struct
{
	PspIoDrvFileArg *arg;
	char *data;
	int len;
} ReadParams;

ReadParams read_params;

//sub_00003A98
int isofs_read2()
{
	int sectorstoread;
	int sectorscanbewritten = 0; // Sectors that can be written directly to the output buffer 
	int nextsector, remaining;
	int res, read = 0;//, eod = 0;
	u8 *p;

	//Kprintf("Free stack 2%d\n", sceKernelGetThreadKernelStackFreeSize(sceKernelGetThreadId()));

	SceUID fd = (SceUID)read_params.arg->arg;

	//Kprintf("Read %d\n", read_params.len);

	if (fd < 0 || fd >= MAX_HANDLERS)
	{
		return SCE_ERROR_ERRNO_EBADF;
	}

	if (!handlers[fd].opened)
	{
		return SCE_ERROR_ERRNO_EBADF;
	}

	if (read_params.len < 0)
	{
		return SCE_ERROR_ERRNO_EINVAL;
	}

	p = (u8 *)read_params.data;
	remaining = read_params.len;

	if (remaining+handlers[fd].filepointer > handlers[fd].filesize)
	{
		remaining -= (remaining+handlers[fd].filepointer)-handlers[fd].filesize;
	}

	if (remaining <= 0)
	{
		return 0;
	}

	nextsector = handlers[fd].lba + (handlers[fd].filepointer / SECTOR_SIZE);
	
	if ((handlers[fd].filepointer % SECTOR_SIZE) != 0)
	{
		res = IsofsReadSectors(nextsector, 1, sectors);//, &eod
		if (res != 1)
		{
			return res;
		}

		read = SECTOR_SIZE-(handlers[fd].filepointer % SECTOR_SIZE);
		
		if (read > remaining)
		{
			read = remaining;
		}

		memcpy(p, sectors+(handlers[fd].filepointer % SECTOR_SIZE), read);

		remaining -= read;
		p += read;
		handlers[fd].filepointer += read;
		nextsector++;
	}

	if (remaining <= 0)// || eod
	{
		return read;
	}
	
	sectorstoread = SizeToSectors(remaining);

	if ((remaining % SECTOR_SIZE) != 0)
	{
		sectorscanbewritten = sectorstoread-1;
	}
	else
	{
		sectorscanbewritten = sectorstoread;		
	}	

	if (sectorscanbewritten > 0)
	{
		res = IsofsReadSectors(nextsector, sectorscanbewritten, p);//, &eod
		if (res < 0)
		{
			return res;
		}

		remaining -= (res*SECTOR_SIZE);
		p += (res*SECTOR_SIZE);
		handlers[fd].filepointer += (res*SECTOR_SIZE);
		read += (res*SECTOR_SIZE);
		nextsector += res;
	}

	if (remaining <= 0)// || eod
	{
		return read;
	}
	
	res = IsofsReadSectors(nextsector, 1, sectors);
	if (res < 0)
	{
		return res;
	}

	memcpy(p, sectors, (remaining % SECTOR_SIZE));
	read += (remaining % SECTOR_SIZE);
	handlers[fd].filepointer += (remaining % SECTOR_SIZE);
	remaining -= (remaining % SECTOR_SIZE);
	
	if (remaining > 0)
	{
		return SCE_ERROR_ERRNO_EIO;
	}
	
	return read;
}


//0x00003210:
int isofs_read(PspIoDrvFileArg *arg, char *data, int len)
{
	sceKernelWaitSema(isofs_sema, 1, NULL);

	read_params.arg = arg;
	read_params.data = data;
	read_params.len = len;
	int res = sceKernelExtendKernelStack(0x1800,(void *)isofs_read2 /*sub_00003A98*/, NULL);
	
	sceKernelSignalSema(isofs_sema, 1);
	return res;
}

//0x00002F50:
SceOff isofs_lseek(PspIoDrvFileArg *arg, SceOff ofs, int whence)
{
	SceUID fd = (SceUID)arg->arg;

	sceKernelWaitSema(isofs_sema, 1, NULL);

	SceOff ret;

	if (fd < 0 || fd >= MAX_HANDLERS) {
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_EBADF;
		ret = SCE_ERROR_ERRNO_EBADF;
		goto RETURN_LEBEL;
	}

	if (!handlers[fd].opened) {
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_EBADF;
		ret = SCE_ERROR_ERRNO_EBADF;
		goto RETURN_LEBEL;
	}

	if (whence == PSP_SEEK_SET)
	{
		handlers[fd].filepointer = (int)ofs;
	}
	else if (whence == PSP_SEEK_CUR)
	{
		handlers[fd].filepointer += (int)ofs;
	}
	else if (whence == PSP_SEEK_END)
	{
		handlers[fd].filepointer = handlers[fd].filesize - (int)ofs;
	}
	else
	{
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_EINVAL;//0x80010016
		ret = SCE_ERROR_ERRNO_EINVAL;
		goto RETURN_LEBEL;
	}

	ret = handlers[fd].filepointer;
RETURN_LEBEL:
	sceKernelSignalSema(isofs_sema, 1);
//	return handlers[fd].filepointer;
	return ret;
}


typedef struct
{
	PspIoDrvFileArg *arg;
	const char *file;
	SceIoStat *stat;
} GetStatParams;

GetStatParams getstat_params;

//sub_00004A04:
int isofs_getstat2()
{
	Iso9660DirectoryRecord record;
	char fullpath[256], path[256], filename[32];
	int res, lba;
		
	if (!getstat_params.arg || !getstat_params.file || !getstat_params.stat)
	{
		return SCE_ERROR_ERRNO_EINVAL;
	}

	if (strlen(getstat_params.file)+1 > 256)
	{
		// path too big for ISO9660 
		return SCE_ERROR_ERRNO_ENAMETOOLONG;
	}
	
	memset(fullpath, 0, 256);
	strncpy(fullpath, getstat_params.file, 256);

	if (fullpath[strlen(fullpath)-1] == '/')
	{
		fullpath[strlen(fullpath)-1] = 0;
	}	
	
	if (getstat_params.arg->fs_num >= 1)
	{
		return SCE_ERROR_ERRNO_EINVAL;
	}
	
	if (strncmp(fullpath, "/sce_lbn", 8) != 0)
	{
		if ((res = GetPathAndName(fullpath, path, filename)) < 0)
		{
			return res;
		}

		if (path[0])
		{
			lba = FindPathLBA(path, &record);		 
		}
		else
		{
			memcpy(&record, &main_record, sizeof(Iso9660DirectoryRecord));
			lba = record.lsbStart;		
		}
		
		if (lba < 0)
		{
			return lba;
		}

		lba =  FindFileLBA(filename, lba, record.lsbDataLength, 0, &record);
		if (lba < 0)
		{
			return lba;
		}

		memset(getstat_params.stat, 0, sizeof(SceIoStat));
		
		if (record.fileFlags & ISO9660_FILEFLAGS_DIR)
		{
			getstat_params.stat->st_mode = 0x116D;
		}
		else
		{
			getstat_params.stat->st_mode = 0x216D;
		}

		getstat_params.stat->st_size = record.lsbDataLength;
		getstat_params.stat->st_ctime.year = getstat_params.stat->st_mtime.year = record.year;
		getstat_params.stat->st_ctime.month = getstat_params.stat->st_mtime.month = record.month;
		getstat_params.stat->st_ctime.day = getstat_params.stat->st_mtime.day = record.day;
		getstat_params.stat->st_ctime.hour = getstat_params.stat->st_mtime.hour = record.hour;
		getstat_params.stat->st_ctime.minute = getstat_params.stat->st_mtime.minute = record.minute;
		getstat_params.stat->st_ctime.second = getstat_params.stat->st_mtime.second = record.second;
		getstat_params.stat->st_private[0] = lba;
	}
	else
	{
		// lba  access
		char str[11];
		char *p;
		int  size;
		
		p = strstr(fullpath, "_size");

		if (!p)
		{
			return SCE_ERROR_ERRNO_EINVAL;
		}

		if ((p-(fullpath+8)) > 10)
		{
			return SCE_ERROR_ERRNO_ENAMETOOLONG;
		}

		memset(str, 0, 11);
		strncpy(str, fullpath+8, p-(fullpath+8));
		
		lba = strtol(str, NULL, 0);
		if (lba < 0)
		{
			return SCE_ERROR_ERRNO_EINVAL;
		}

		if ((p+strlen(p)-(p+5)) > 10)
		{
			return SCE_ERROR_ERRNO_ENAMETOOLONG;
		}

		memset(str, 0, 11);
		strncpy(str, p+5, p+strlen(p)-(p+5));

		size = strtol(str, NULL, 0);
		if (size < 0)
		{
			return SCE_ERROR_ERRNO_EINVAL;
		}

		// DO nothing, but return success, like official $CE driver does 
	}

	return 0;
}
//0x00003180
int isofs_getstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat)
{
	sceKernelWaitSema(isofs_sema, 1, NULL);
	
	getstat_params.arg = arg;
	getstat_params.file = file;
	getstat_params.stat = stat;

	int res = sceKernelExtendKernelStack(0x1800, (void *)isofs_getstat2/*sub_00004324*/, NULL);

	sceKernelSignalSema(isofs_sema, 1);
	return res;
}
//sub_00003340:
int isofs_dopen(PspIoDrvFileArg *arg, const char *dirname)
{
	SceUID fd;

	//Kprintf("Free stack %d\n", sceKernelGetThreadKernelStackFreeSize(sceKernelGetThreadId()));

	int res = isofs_open(arg, (char *)dirname, PSP_O_RDONLY, 0);

	if (res < 0)
	{
		return res;
	}

	sceKernelWaitSema(isofs_sema, 1, NULL);

	fd = (SceUID)arg->arg;	

	if (!(handlers[fd].attributes & ISO9660_FILEFLAGS_DIR))
	{
		handlers[fd].opened = 0;
		sceKernelSignalSema(isofs_sema, 1);
		return SCE_ERROR_ERRNO_ENOENT;
	}

	handlers[fd].olddirlen = 0;
	handlers[fd].curlba = handlers[fd].lba;
	handlers[fd].eof = 0;

	sceKernelSignalSema(isofs_sema, 1);
	return 0;
}

// 0x00002EA4
int isofs_dclose(PspIoDrvFileArg *arg)
{
	SceUID fd = (SceUID)arg->arg;
	sceKernelWaitSema(isofs_sema, 1, NULL);
	int ret = 0;

	if (fd < 0 || fd >= MAX_HANDLERS)
	{
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_EBADF;
		ret = SCE_ERROR_ERRNO_EBADF;
		goto RETURN_LABEL;
	}

	if (!handlers[fd].opened)
	{
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_EBADF;
		ret = SCE_ERROR_ERRNO_EBADF;
		goto RETURN_LABEL;
	}

	if (!(handlers[fd].attributes & ISO9660_FILEFLAGS_DIR))
	{
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_ENOTDIR;
		ret = SCE_ERROR_ERRNO_ENOTDIR;
		goto RETURN_LABEL;
	}

	handlers[fd].opened = 0;

RETURN_LABEL:
	sceKernelSignalSema(isofs_sema, 1);
	return ret;
}


typedef struct
{
	PspIoDrvFileArg *arg;
	SceIoDirent *dirent;
} DreadParams;

DreadParams dread_params;
//sub_00003774
int isofs_dread2()
{
	Iso9660DirectoryRecord *record;
	u8 *p;
	SceUID fd = (SceUID)dread_params.arg->arg;	

	//Kprintf("dread.\n");

	if (!dread_params.dirent)
	{
		return SCE_ERROR_ERRNO_EINVAL;
	}

	if (fd < 0 || fd >= MAX_HANDLERS)
	{
		return SCE_ERROR_ERRNO_EBADF;
	}

	if (!handlers[fd].opened)
	{
		return SCE_ERROR_ERRNO_EBADF;
	}

	if (!(handlers[fd].attributes & ISO9660_FILEFLAGS_DIR))
	{
		return SCE_ERROR_ERRNO_ENOTDIR;
	}

	if (handlers[fd].eof)
	{
		return 0;
	}

	while (1)
	{
		if (handlers[fd].filesize > (8*SECTOR_SIZE))
		{
			int base = handlers[fd].curlba - handlers[fd].lba;
			int offset = handlers[fd].filepointer - (base*SECTOR_SIZE);

			if ((offset + sizeof(Iso9660DirectoryRecord) + 0x60) >= (8*SECTOR_SIZE))
			{
				handlers[fd].curlba += (offset / SECTOR_SIZE);
				IsofsReadSectors(handlers[fd].curlba, 8, sectors);
				p = sectors + (offset % SECTOR_SIZE);
			}
			else
			{
				IsofsReadSectors(handlers[fd].curlba, 8, sectors);
				p = sectors+offset;
			}
		}
		else
		{
			IsofsReadSectors(handlers[fd].lba, handlers[fd].filesize / SECTOR_SIZE, sectors );
			p = sectors+handlers[fd].filepointer;
		}

		record = (Iso9660DirectoryRecord *)p;

		if (record->len_dr == 0)
		{
			if (SECTOR_SIZE - (handlers[fd].filepointer % SECTOR_SIZE) <= handlers[fd].olddirlen)
			{
				p += (SECTOR_SIZE - (handlers[fd].filepointer % SECTOR_SIZE));
				handlers[fd].filepointer += (SECTOR_SIZE - (handlers[fd].filepointer % SECTOR_SIZE));				
				record = (Iso9660DirectoryRecord *)p;

				if (record->len_dr == 0)
				{
					handlers[fd].eof = 1;
					return 0;
				}
			}

			else
			{
				handlers[fd].eof = 1;
				return 0;
			}		
		}

		memset(dread_params.dirent, 0, sizeof(SceIoDirent));

		if (record->fileFlags & ISO9660_FILEFLAGS_DIR)
		{
			dread_params.dirent->d_stat.st_mode = 0x116D;
		}
		else
		{
			dread_params.dirent->d_stat.st_mode = 0x216D;
		}

		dread_params.dirent->d_stat.st_size = record->lsbDataLength;
		dread_params.dirent->d_stat.st_ctime.year = dread_params.dirent->d_stat.st_mtime.year = record->year;
		dread_params.dirent->d_stat.st_ctime.month = dread_params.dirent->d_stat.st_mtime.month = record->month;
		dread_params.dirent->d_stat.st_ctime.day = dread_params.dirent->d_stat.st_mtime.day = record->day;
		dread_params.dirent->d_stat.st_ctime.hour = dread_params.dirent->d_stat.st_mtime.hour = record->hour;
		dread_params.dirent->d_stat.st_ctime.minute = dread_params.dirent->d_stat.st_mtime.minute = record->minute;
		dread_params.dirent->d_stat.st_ctime.second = dread_params.dirent->d_stat.st_mtime.second = record->second;
		dread_params.dirent->d_stat.st_private[0] = record->lsbStart;

		if (record->fi == 0)
		{
			strcpy(dread_params.dirent->d_name, ".");
		}
		else if (record->fi == 1)
		{
			strcpy(dread_params.dirent->d_name, "..");
		}
		else
		{
			strncpy(dread_params.dirent->d_name, &record->fi, record->len_fi);
		}

		handlers[fd].filepointer += record->len_dr;
		handlers[fd].olddirlen = record->len_dr;

		if (strcmp(dread_params.dirent->d_name, ".") != 0 && strcmp(dread_params.dirent->d_name, "..") != 0)
		{
			break;
		}
	}

	return 1;	
}

//sub_00003100:
int isofs_dread(PspIoDrvFileArg *arg, SceIoDirent *dirent)
{
	sceKernelWaitSema(isofs_sema, 1, NULL);
	
	dread_params.arg = arg;
	dread_params.dirent = dirent;
	int res = sceKernelExtendKernelStack(0x1800,(void *)isofs_dread2/* sub_00003774*/, NULL);

	sceKernelSignalSema(isofs_sema, 1);
	return res;
}
//; 0x00002D0C
int isofs_ioctl(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	SceUID fd = (SceUID)arg->arg;
	int lowcmd;
	u32 *outdata32 = (u32 *)outdata;

	sceKernelWaitSema(isofs_sema, 1, NULL);

	int ret = -1;

	if (fd < 0 || fd >= MAX_HANDLERS)
	{
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_EBADF;
		ret = SCE_ERROR_ERRNO_EBADF;
		goto RETURN_LABEL;
	}

	if (!handlers[fd].opened)
	{
//		sceKernelSignalSema(isofs_sema, 1);
//		return SCE_ERROR_ERRNO_EBADF;
		ret = SCE_ERROR_ERRNO_EBADF;
		goto RETURN_LABEL;
	}

	lowcmd = cmd & 0xFFFF;

	if ( (lowcmd >= 0x8010 && lowcmd <= 0x8013 ) || 
	lowcmd == 0x8001 || lowcmd == 0x8081)
	{
//		sceKernelSignalSema(isofs_sema, 1);
//		return 0;
		ret = 0;
		goto RETURN_LABEL;
	}


	if (cmd <= 0x01020003)
	{
		if (cmd >= 0x01020001)
		{
//			sceKernelSignalSema(isofs_sema, 1);
//			return 0;	
			ret = 0;
			goto RETURN_LABEL;
		}  

		else if (cmd == 0x01000011) // SCE_ISOFS_CLEAR_DIRBUF 
		{
//			sceKernelSignalSema(isofs_sema, 1);
//			return 0;
			ret = 0;
			goto RETURN_LABEL;
		}

//		sceKernelSignalSema(isofs_sema, 1);
//		return -1;
		ret = -1;
		goto RETURN_LABEL;
	}

	switch (cmd)
	{
		case 0x01020004: // SCE_ISOFS_GET_FILEPOINTER 	
			if (!outdata || outlen < 4)
			{
//				sceKernelSignalSema(isofs_sema, 1);
//				return SCE_ERROR_ERRNO_EINVAL;
				ret = SCE_ERROR_ERRNO_EINVAL;
			}
			else
			{
				outdata32[0] = handlers[fd].filepointer;
//				sceKernelSignalSema(isofs_sema, 1);
//				return 0;
				ret = 0;
			}
		break;
	
		case 0x01020006: // get lba 
			
			if (!outdata || outlen < 4)
			{
//				sceKernelSignalSema(isofs_sema, 1);
//				return SCE_ERROR_ERRNO_EINVAL;
				ret = SCE_ERROR_ERRNO_EINVAL;
			}
			else
			{
				outdata32[0] = handlers[fd].lba;
//				sceKernelSignalSema(isofs_sema, 1);
//				return 0;
				ret = 0;
			}
		break;

		case 0x01020007: // get file size (64 bits) 
			
			if (!outdata || outlen < 8)
			{
//				return SCE_ERROR_ERRNO_EINVAL;
				ret = SCE_ERROR_ERRNO_EINVAL;
			}
			else
			{
				outdata32[0] = handlers[fd].filesize;
				outdata32[1] = 0;
//				sceKernelSignalSema(isofs_sema, 1);
//				return 0;
				ret = 0;
			}
		break; 		
	}

RETURN_LABEL:
	sceKernelSignalSema(isofs_sema, 1);
//	return -1;	
	return ret;
}

//loc_00002CE0:
int isofs_devctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	return 0;
}
//loc_00002CE8:	
int isofs_chdir(PspIoDrvFileArg *arg, const char *dir)
{
//	WriteFile("ms0:/chdir.bin", dir, strlen( dir ));
	return 0;
}

//loc_00002CF0:
int isofs_mount(PspIoDrvFileArg *arg)
{
	return 0;
}
//loc_00002CF8:
int isofs_umount(PspIoDrvFileArg *arg)
{
	return 0;
}

PspIoDrvFuncs isofs_funcs =
{
	isofs_init,//3CCC
	isofs_exit,//3488
	isofs_open,//32A0
	isofs_close,//3070
	isofs_read,//3210
	NULL, /* no write */
	isofs_lseek,//2F50
	isofs_ioctl,//2D0C
	NULL, /* no remove */
	NULL, /* no mkdir */
	NULL, /* no rmdir */
	isofs_dopen,//3340
	isofs_dclose,//2EA4
	isofs_dread,//3100
	isofs_getstat,//3180
	NULL, /* no chstat */
	NULL, /* no rename */
	isofs_chdir,//2CE8
	isofs_mount,//2CF0
	isofs_umount,//2CF8
	isofs_devctl,//2CE0
	NULL
};

PspIoDrv isofs_driver = { "isofs", 0x10, 0x800, "ISOFS", &isofs_funcs };

//sub_00002D00:
PspIoDrv *getisofs_driver()
{
	return &isofs_driver;
}


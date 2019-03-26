// dummy functions to allow the rest to compile

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../malloc.h"

#include "../main.h"
#include "psperror.h"

#include "systemctrl_me.h"

#include "csoread.h"
#include "daxread.h"
#include "isoread.h"

#include "umd9660_driver.h"
#include "isofs_driver.h"

enum {
	ISO,
	CSO,
	DAX
};

SceUID umd_sema = -1;
SceUID umdfd = -1;
int umd_file_len = 0x7FFFFFFF;
int discType = 0x10;
//int umd_cur_sector = -1;
int umd_open = 0;
SceOff umd_cur_offset;

static u8 *sectorbuf = NULL;
static int umd_format;
static char umd_buff[128];//
static char *umdfilename = umd_buff;

typedef struct
{
  int   lba;
  int   nsectors;
  void  *buf;
} UmdReadParams;

static UmdReadParams umd_read_params;

static int UmdRead_secbuf(int lba);


char *sctrlSEGetUmdFile()//GetUmdFile
{
	return umdfilename;;
}

char *sctrlSEGetUmdFileEx(char *input)
{
	umdfilename = ( input != NULL )? input :umd_buff;
	return umdfilename;
}

/*
void sctrlSECopyUmdFile(char *out)
{
	int k1 = pspSdkSetK1(0);
	strcpy( out , umdfilename );
	pspSdkSetK1(k1);
}
*/

void sctrlSESetUmdFile(const char *umd)
{
	strncpy(umdfilename,umd, 128 );

	sceIoClose( umdfd );//dataA6D0

	umdfd = -1;//dataA6D0
	umd_open = 0;//dataA7D0
}

void sctrlSESetUmdFileEx(const char *umd, char *input)
{
	umdfilename = ( input != NULL )? input:umd_buff;
	sctrlSESetUmdFile( umd );
}

void sctrlSESetDiscType(int type)
{
	int k1 = pspSdkSetK1(0);
	discType = type;
	pspSdkSetK1(k1);
}

static int GetIsoDiscSize()
{
	if(umd_format)
	{
		if (umd_format == CSO)
		{
			return CisofileGetDiscSize(umdfd);
		}
		else
		{
			return DaxfileGetDiscSize(umdfd);
		}
	}
	else
	{
		return IsofileGetDiscSize(umdfd);
	}
}

int OpenIso()
{
	sceIoClose( umdfd );
	umd_open = 0;

	int fd = sceIoOpen(umdfilename , PSP_O_RDONLY , 0 );
//	printf("%s open %s ret = 0x%08X \n", __func__ , umdfilename, fd );

	if(fd < 0)
	{
		return -1;
	}

	umdfd = fd;
	umd_format = ISO;

	if(CisoOpen(umdfd) >= 0)
	{
		umd_format = CSO;
	}
	else if (DaxOpen(umdfd) >= 0)
	{
		umd_format = DAX;
	}

	umd_file_len/*dataA6E0*/ = GetIsoDiscSize();

//	umd_cur_sector = -1;
	umd_open = 1;

	return 0;
}

//sub_000051A0:
int ReadUmdFileRetry(void *buf, int size, int fpointer)
{
	int i, read;
	for(i = 0; i < 16; i++) {
		if(sceIoLseek32(umdfd, fpointer, PSP_SEEK_SET) >= 0)
		{
			for(i = 16; i > 0; i--)
			{
				if((read = sceIoRead(umdfd, buf, size)) >= 0) 
				{
					return read;
				}

				OpenIso();
			}

			return 0x80010013;
		}

		OpenIso();
	}

	return 0x80010013;
}

//sub_000054B8
int Umd9660ReadSectors3()
{
	if(umd_open == 0) {
		int i;
		for(i = 16; i > 0; i--)
		{
			if(sceIoLseek32(umdfd, 0, PSP_SEEK_CUR) >= 0)
			{
				break;
			}

			OpenIso();
		}

		if(umd_open == 0)
		{
			return 0x80010013;
		}
	}

	if (umd_format)
	{
		if (umd_format == CSO)
		{
			return CisofileReadSectors(umd_read_params.lba, umd_read_params.nsectors, umd_read_params.buf);
		}
		else
		{
			return DaxfileReadSectors(umd_read_params.buf, umd_read_params.nsectors, umd_read_params.lba);
		}
	}
	else
	{
		return IsofileReadSectors(umd_read_params.lba, umd_read_params.nsectors, umd_read_params.buf);
	}
}

//sub_00004D88:
int  Umd9660ReadSectors(int lba, int nsectors, void *buf)//, int *eod
{
  umd_read_params.lba = lba;
  umd_read_params.nsectors = nsectors;
  umd_read_params.buf = buf;

  return sceKernelExtendKernelStack(0x2000, (void *)Umd9660ReadSectors3/*sub_000054B8*/, 0);
}

//loc_0000525C:
int  Umd9660ReadSectors2(int lba, int nsectors, void *buf)
{ 
	if(umd_open == 0) 
	{
		int i;
		for(i = 16; i > 0; i--)
		{
			if(sceIoLseek32(umdfd, 0, PSP_SEEK_CUR) >= 0)
			{
				break;
			}

			OpenIso();
		}

		if(umd_open == 0)
		{
			return 0x80010013;
		}
	}

	if(umd_format)
	{
		if (umd_format == CSO)
		{
			return CisofileReadSectors(lba, nsectors, buf);//loc_00005A28
		}
		else
		{
			return DaxfileReadSectors(buf, nsectors, lba);
		}
	}
	else
	{
		return IsofileReadSectors(lba, nsectors, buf);//loc_00005934
	}
}

//sub_00004AE0:
int umd9660_devctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	sceKernelWaitSema( umd_sema ,1,NULL);//dataA6CC

	if(cmd == 0x01F20001)
	{
		((int *)outdata)[0]=-1;
		((int *)outdata)[1]= discType;//16;//
	}
	else if(cmd == 0x01F20002)
	{
		((int *)outdata)[0]=umd_file_len;//dataA6E0
	}
	else if(cmd != 0x01F00003)
	{
		int ret = -1;

//		if( cmd == 0x01E18030 )
//			ret = 1;

		sceKernelSignalSema(umd_sema,1);		
		return ret;
	}


	sceKernelSignalSema(umd_sema,1);//dataA6CC
	return 0;
}

//sub_00004D10:
int umd9660_close(PspIoDrvFileArg *arg)
{
	sceKernelWaitSema( umd_sema ,1,NULL);//dataA6CC
	sceKernelSignalSema( umd_sema ,1);//dataA6CC
	return 0;
}

//sub_00004DB0:
int umd9660_read(PspIoDrvFileArg *arg, char *data, int len)
{
	sceKernelWaitSema(umd_sema, 1, 0);//dataA6CC

	int i = len;
	if(umd_file_len < umd_cur_offset /*B0EC*/ + len) //len >= pos + a2
	{
		i = umd_file_len - umd_cur_offset; //len - pos
	}

	int ret = Umd9660ReadSectors(umd_cur_offset /*B0EC*/, i, data);//sub_00004D88

	if(ret >= 0)
	{
		umd_cur_offset += ret;//dataB0EC
	}

	sceKernelSignalSema(umd_sema, 1);

	return ret;
}

//sub_00004B9C:
int umd9660_ioctl(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	sceKernelWaitSema( umd_sema, 1, NULL);//dataA6CC

	if(cmd == 0x01D20001)
	{
		((SceOff *)outdata)[0]=umd_cur_offset;//dataB0EC
		sceKernelSignalSema( umd_sema ,1);//dataA6CC
	}
	else
	{
		sceKernelSignalSema( umd_sema ,1);//dataA6CC
		//printf("ioctl %08x \n", cmd);
	return -1;
	}

	return 0;
}

//sub_00004C1C
SceOff umd9660_lseek(PspIoDrvFileArg *arg ,SceOff offset,int whence)
{
	sceKernelWaitSema(umd_sema, 1, 0);//dataA6CC

	if(whence == PSP_SEEK_SET)
	{
		umd_cur_offset  = offset;//dataB0EC
	}
	else if(whence == PSP_SEEK_CUR)
	{
		umd_cur_offset += offset;
	}
	else if(whence == PSP_SEEK_END)
	{
		umd_cur_offset = umd_file_len - offset;//dataA6E0 - offset
	}
	else
	{
		sceKernelSignalSema(umd_sema, 1);
		return 0x80010016;
	}

	umd_cur_offset = (umd_cur_offset < umd_file_len) ? umd_cur_offset : umd_file_len;
	sceKernelSignalSema(umd_sema, 1);

	return umd_cur_offset;
}


//sub_00004ED4
int umd9660_exit(PspIoDrvArg* arg)
{
	sceKernelWaitSema( umd_sema /*dataA6CC*/ , 1, NULL);

	if (sectorbuf)
	{
		sctrlKernelFree(sectorbuf);
		sectorbuf = NULL;
	}

	if (umd_sema >= 0)
	{
		sceKernelDeleteSema(umd_sema);
		umd_sema = -1;
	}

	sceIoClose(umdfd);

	return 0;
}

//sub_00004F54
int umd9660_init()
{
	if(!sectorbuf)
	{
		sectorbuf = sctrlKernelMalloc(SECTOR_SIZE);//2048

		if(sectorbuf == NULL)
		{
			return -1;
		}
	}

	//dataA6CC
	if(umd_sema < 0)
	{
		umd_sema = sceKernelCreateSema("",0 ,1 ,1 ,NULL);//dataA6CC

		if(umd_sema < 0)//dataA6CC
		{
			return umd_sema;
		}
	}

	return 0;
}

//sub_000050F4:
int umd9660_open(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	sceKernelWaitSema( umd_sema , 1,NULL);

	int i=0;
	
	for(i = 0; i < 16; i++)
	{

		if(sceIoLseek32(umdfd, 0, PSP_SEEK_SET) < 0)
		{
			OpenIso();
		}
		else
		{
			arg->arg = 0;
			umd_cur_offset = 0;
			sceKernelSignalSema(umd_sema ,1);
			return 0;
		}
	}

	sceKernelSignalSema(umd_sema, 1);//dataA6CC

	return 0x80010013;
}

PspIoDrvFuncs umd9660_funcs =//dataA6F0
{
	umd9660_init,//4F54
	umd9660_exit,//4ED4
	umd9660_open,//50F4
	umd9660_close,//4D10
	umd9660_read,//4DB0
	NULL, /* no write */
	(void *)umd9660_lseek,//4C1C
	umd9660_ioctl,//4B9C
	NULL, /* no remove */
	NULL, /* no mkdir */
	NULL, /* no rmdir */
	NULL,//umd9660_dopen
	NULL,//umd9660_dclose
	NULL,//umd9660_dread
	NULL,//umd9660_getstat
	NULL, /* no chstat */
	NULL, /* no rename */
	NULL,//umd9660_chdir
	NULL,//umd9660_mount
	NULL,//umd9660_umount
	umd9660_devctl,//4Ae0
	NULL
};

PspIoDrv umd9660_driver = { "umd"/*7D88*/, 0x4, SECTOR_SIZE, "UMD9660", &umd9660_funcs/*A6F0*/ };

//sub_00004A04
PspIoDrv *getumd9660_driver()
{
	return &umd9660_driver;
}

int umd_read_block2(void *a0)
{
	int boffs, lba, bsize, ret;
	UmdUnknownParam1 *params = (UmdUnknownParam1 *)a0;
	u8 *buf = params->buf; //s4

	int size = params->lbaparams->byte_size_total; //s2 = v1[0x10 / 4];
	if(params->lbaparams->byte_size_start && (params->lbaparams->byte_size_centre || params->lbaparams->byte_size_last))
	{
		boffs = SECTOR_SIZE - params->lbaparams->byte_size_start;
	}
	else
	{
		boffs = params->lbaparams->byte_size_start;
	}

	lba = params->lbaparams->lba_top;

	if(boffs > 0)
	{
		bsize = SECTOR_SIZE - boffs;
		if(bsize > size)
		{
			bsize = size;
		}

		if(bsize > 0)
		{
			ret = UmdRead_secbuf(lba);
			if(ret < 0)
			{
				return ret;
			}
		}

		memcpy(buf, &sectorbuf[boffs], bsize);

		size -= bsize;
		buf += bsize;
		lba++;
	}

	int burst_size = size & 0xFFFFF800;
	if(burst_size)
	{
		ret = Umd9660ReadSectors2(lba, burst_size / SECTOR_SIZE, buf);//loc_0000525C
		if(ret < 0)
		{
			return ret;
		}

		lba += burst_size / SECTOR_SIZE;
		buf += burst_size;
		size -= burst_size;
		//->52e4
	}

	//52e4
	ret = 0;
	if(size > 0)
	{
		ret = UmdRead_secbuf(lba);//sub_0000534C
		if(ret >= 0)
		{
			memcpy(buf, sectorbuf/*dataA7C8*/, size);
			return 0;
		}
	}

	return ret;
}

//sub_00004D50:
int umd_read_block(void *drvState, u8 *buf, SceInt64 read_size, LbaParams *lba_param)
{
	UmdUnknownParam1 params;

	params.drvState = drvState;
	params.buf = buf;
	params.read_size = read_size;
	params.lbaparams = lba_param;

	return sceKernelExtendKernelStack(0x2000, (void *)umd_read_block2/*sub_0000538C*/, &params);
}

static int UmdRead_secbuf(int lba)
{
	int ret = Umd9660ReadSectors2(lba, 1, sectorbuf);
	if(ret < 0)
	{
		return ret;
	}

	return 0;
}

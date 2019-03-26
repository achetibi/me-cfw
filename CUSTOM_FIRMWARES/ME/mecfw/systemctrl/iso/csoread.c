#include <pspsdk.h>
#include <pspkernel.h>
#include "../malloc.h"

#include "csoread.h"
#include "umd9660_driver.h"

extern int sceKernelDeflateDecompress(void *dst,int dsize,void *src,int *pparam);

#define DEV_NAME "CSO"


// thread priority in read
// when READ_PRIORITY >= 48 then OutRun2006 are not work.
//#define READ_PRIORITY 47

// index buffer size
#define CISO_INDEX_SIZE (512/4)

// compresed data buffer cache size
#define CISO_BUF_SIZE 0x2000

/****************************************************************************
****************************************************************************/

//#ifdef CISO_BUF_SIZE
//static unsigned char ciso_data_buf[CISO_BUF_SIZE] __attribute__((aligned(64)));
//#else
//static unsigned char ciso_data_buf[0x800] __attribute__((aligned(64)));
//#endif
static u8 *ciso_data_buf = NULL;//dataA800

static u32 ciso_index_buf[CISO_INDEX_SIZE] __attribute__((aligned(64)));//dataA840

static u32 ciso_buf_pos;    // file poisiotn of the top of ciso_data_buf//dataAA40
static u32 ciso_cur_index;  // index(LBA) number of the top of ciso_index_buf//dataAA44

// header buffer
static CISO_H ciso;//dataAA48

/****************************************************************************
	Mount UMD event callback
****************************************************************************/
static int max_sectors;

//sub_00005BC4
int CisoOpen(int umdfd)
{
	int result;

	// check CISO header
	ciso.magic = 0;
	ciso_buf_pos = 0x7FFFFFFF;//dataAA40

	//result = dhReadFileRetry(&ciso_fd, 0, &ciso, sizeof(ciso));
	sceIoLseek(umdfd, 0, PSP_SEEK_SET);
	result = sceIoRead(umdfd, &ciso, sizeof(ciso));
	if(result < 0)
		return result;

	if(ciso.magic == 0x4F534943)//C I S O
	{
		max_sectors = (int)(ciso.total_bytes) / ciso.block_size;
		ciso_cur_index = 0xffffffff;

		if (!ciso_data_buf)//dataA800
		{
			ciso_data_buf = (u8 *)sctrlKernelMalloc(CISO_BUF_SIZE + 64);

			if (!ciso_data_buf)
				return -1;

			if ((((u32)ciso_data_buf) % 64) != 0)
			{
				ciso_data_buf += (64 - (((u32)ciso_data_buf) % 64));
			}
		}

		return 0;
	}

	// header check error
	return SCE_KERNEL_ERROR_NOFILE;//0x8002012F
}

/****************************************************************************
	get file pointer in sector
****************************************************************************/
//sub_00005988
static int inline ciso_get_index(u32 sector, int *pindex)
{
	int result;
	int index_off;

	// search index
	index_off = sector - ciso_cur_index;//sector - dataAA44

	if((ciso_cur_index == 0xffffffff) || (index_off < 0) || (index_off >= CISO_INDEX_SIZE))
	{
		//result = dhReadFileRetry(&ciso_fd,sizeof(ciso)+sector*4,ciso_index_buf,sizeof(ciso_index_buf));

		result = ReadUmdFileRetry(ciso_index_buf, sizeof(ciso_index_buf), sizeof(ciso) + sector * 4);//sub_000051A0
		

		if(result < 0) 
			return result;

		ciso_cur_index = sector;
		index_off = 0;//a3=0
	}

	// get file posision and sector size
	*pindex = ciso_index_buf[index_off];//dataA840

	return 0;
}

/****************************************************************************
	Read one sector
****************************************************************************/

//loc_00005A68:
static int ciso_read_one(void *buf, int sector)
{
	int result;
	int index, index2;
	int dpos, dsize;

	// get current index
	result = ciso_get_index(sector, &index);//sub_00005988
	if(result < 0) 
	{
		return result;
	}

	// get file posision and sector size
	dpos = (index & 0x7fffffff) << ciso.align;

	if(index & 0x80000000)
	{
		result = ReadUmdFileRetry(buf, SECTOR_SIZE, dpos);//sub_000051A0
		return result;
	}

	// compressed sector
	// sub_00005988
	// get sector size from next index
	result = ciso_get_index(sector + 1, &index2);
	if(result < 0)
		return result;

	dsize = ((index2 & 0x7fffffff) << ciso.align) - dpos;

	// adjust to maximum size for scramble(shared) sector index
	if((dsize <= 0) || (dsize > SECTOR_SIZE))
		dsize = SECTOR_SIZE;

	// read sector buffer
	if((dpos < ciso_buf_pos) || ((dpos + dsize) > (ciso_buf_pos + CISO_BUF_SIZE)))
	{
		// seek & read
		result = ReadUmdFileRetry(ciso_data_buf, CISO_BUF_SIZE, dpos);//sub_000051A0
		if(result < 0)
		{
			ciso_buf_pos = 0xfff00000; // set invalid position
			return result;
		}

		ciso_buf_pos = dpos;
	}

	result = sceKernelDeflateDecompress(buf, SECTOR_SIZE, ciso_data_buf + dpos - ciso_buf_pos, NULL);
	if(result < 0) 
		return result;

	return SECTOR_SIZE;
}

/****************************************************************************
	Read Request
****************************************************************************/

int CisofileReadSectors(int lba, int nsectors, void *buf)
{
	int result;
	int i;

	int num_bytes = nsectors * SECTOR_SIZE;

	for(i = 0; i < num_bytes; i += SECTOR_SIZE)
	{
		result = ciso_read_one(buf, lba);
		if(result < 0)
		{
			nsectors = result;
			break;
		}

		buf += SECTOR_SIZE;
		lba++;
	}

	return nsectors;
}

/****************************************************************************/

//loc_0000596C
int CisofileGetDiscSize(int umdfd)
{
	return (int)(ciso.total_bytes) / ciso.block_size;
}


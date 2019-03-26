#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <string.h>

#include "systemctrl_me.h"
#include "daxread.h"
#include "umd9660_driver.h"

static DAXHeader header;
static NCArea ncareas[MAX_NCAREAS];

static void *dax_read_buf = NULL;
static void *dax_dec_data_buf = NULL;
static void *dax_com_data_buf = NULL;

static u32 g_DAX_idx_cache[DAX_INDEX_SIZE/4] __attribute__((aligned(64)));
static u16 g_DAX_len_cache[DAX_INDEX_SIZE/4] __attribute__((aligned(64)));

static u32 g_DAX_dec_buf_offset = (u32)-1;
static int g_DAX_dec_buf_size = 0;
static int g_DAX_cur_idx = -1;
static int g_DAX_cur_pos = -1;
static int max_sectors;

int IsNCArea(unsigned int frame, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		if (frame >= ncareas[i].frame && ((frame - ncareas[i].frame) < ncareas[i].size))
		{
			return 1;
		}
	}

	return 0;
}

int DaxOpen(int umdfd)
{
	sceIoLseek(umdfd, 0, PSP_SEEK_SET);

	int result = sceIoRead(umdfd, &header, sizeof(header));
	if(result < 0)
	{
		return result;
	}

	if(header.signature == DAX_FILE_SIGNATURE)
	{
		if (header.version <= DAX_FILE_VERSION_1)
		{
			max_sectors = header.decompsize / DAX_FRAME_SIZE;
			if ((header.decompsize % DAX_FRAME_SIZE) != 0)
			{
				max_sectors++;
			}

			if (dax_com_data_buf == NULL)
			{
				dax_com_data_buf = sctrlKernelMalloc(DAX_FRAME_SIZE + 1024);
				if (!dax_com_data_buf)
				{
					return -1;
				}
			}

			if (dax_dec_data_buf == NULL)
			{
				dax_dec_data_buf = sctrlKernelMalloc(DAX_FRAME_SIZE);
				if (!dax_dec_data_buf)
				{
					return -1;
				}
			}

			if (dax_read_buf == NULL)
			{
				dax_read_buf = sctrlKernelMalloc(SECTOR_SIZE);
				if (!dax_read_buf)
				{
					return -1;
				}
			}

			if (header.nNCareas > 0)
			{
				sceIoLseek(umdfd, sizeof(DAXHeader) + max_sectors * 4 + max_sectors * 2, PSP_SEEK_SET);

				result = sceIoRead(umdfd, ncareas, sizeof(ncareas));
				if(result < 0)
				{
					return result;
				}
			}

			g_DAX_dec_buf_offset = (u32)-1;
			g_DAX_dec_buf_size = 0;
			g_DAX_cur_idx = -1;
			g_DAX_cur_pos = -1;

			return 0;
		}
		else
		{
			return -1;
		}
	}

	return SCE_KERNEL_ERROR_NOFILE;
}

static int DaxReadOne(void *buf, int sector)
{
	int g_DAX_pos = sector - (sector % 4);

	if (g_DAX_cur_pos == -1 || g_DAX_pos != g_DAX_cur_pos)
	{
		int result;
		int n_sector = (g_DAX_pos - g_DAX_cur_idx) / 4;

		if (g_DAX_cur_idx == -1 || n_sector < 0 || n_sector >= (sizeof(g_DAX_idx_cache) / sizeof(g_DAX_idx_cache[0])))
		{
			memset(g_DAX_idx_cache, 0, sizeof(g_DAX_idx_cache));
			result = ReadUmdFileRetry(g_DAX_idx_cache, sizeof(g_DAX_idx_cache), sizeof(DAXHeader) + g_DAX_pos);
			if (result < 0)
			{
				return result;
			}

			memset(g_DAX_len_cache, 0, sizeof(g_DAX_len_cache));
			result = ReadUmdFileRetry(g_DAX_len_cache, sizeof(g_DAX_len_cache), sizeof(DAXHeader) + (max_sectors * 4) + (g_DAX_pos / 2));
			if (result < 0)
			{
				return result;
			}

			g_DAX_cur_idx = g_DAX_pos;
			n_sector = 0;
		}

		u32 offset = g_DAX_idx_cache[n_sector];
		int size = g_DAX_len_cache[n_sector];
		g_DAX_cur_pos = g_DAX_pos;

		if (g_DAX_dec_buf_offset == (u32)-1 || offset < g_DAX_dec_buf_offset || offset + size >= g_DAX_dec_buf_offset + g_DAX_dec_buf_size)
		{
			memset(dax_com_data_buf, 0, DAX_FRAME_SIZE + 1024);
			result = ReadUmdFileRetry(dax_com_data_buf, size, offset);
			if (result < 0)
			{
				g_DAX_dec_buf_offset = (u32)-1;
				return result;
			}

			g_DAX_dec_buf_offset = offset;
			g_DAX_dec_buf_size = result;
		}

		memset(dax_dec_data_buf, 0, DAX_FRAME_SIZE);

		if (IsNCArea(g_DAX_pos / 4, header.nNCareas))
		{
			memcpy(dax_dec_data_buf, dax_com_data_buf, DAX_FRAME_SIZE);
		}
		else
		{
			result = sctrlKernelUncompress(dax_dec_data_buf, DAX_FRAME_SIZE, dax_com_data_buf, size);
		}
	}

	memcpy(buf, dax_dec_data_buf + (SECTOR_SIZE * (sector % 4)), SECTOR_SIZE);

	return SECTOR_SIZE;
}

int DaxfileReadSectors(void *buf, int read_size, int fpointer)
{
	int result;
	int i;
	int ret=0;
	int offset;
	int nsectors = fpointer >> 11;

	int size = (fpointer & 0x7FF);

	if(size)
	{
		result = DaxReadOne(dax_read_buf, nsectors);
		if(result < 0)
		{
			return result;
		}

		offset = size;
		size = SECTOR_SIZE - offset;

		if(size > read_size)
		{
			size = read_size;
		}

		memcpy(buf, dax_read_buf + offset, size);

		ret = size;
		read_size -= size;
		buf += size;
		nsectors++;
	}

	int cnt = 0;
	
	if(read_size)
	{
		cnt = read_size / SECTOR_SIZE;
	}

	if(cnt > 0)
	{
		for(i = 0; i < cnt; i++)
		{
			result = DaxReadOne(buf, nsectors);
			if(result < 0)
			{
				return result;
			}

			ret += SECTOR_SIZE;
			read_size -= SECTOR_SIZE;
			buf += SECTOR_SIZE;
			nsectors++;
		}
	}

	if(read_size)
	{
		result = DaxReadOne(dax_read_buf, nsectors);
		if(result < 0)
		{
			return result;
		}

		memcpy(buf, dax_read_buf, read_size);

		ret += read_size;
	}

	return ret;
}

int DaxfileGetDiscSize(int umdfd)
{
	return max_sectors * 4;
}

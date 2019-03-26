/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pspkernel.h>
#include <pspreg.h>
#include <stdio.h>
#include <string.h>
#include <systemctrl_me.h>
#include <pspsysmem_kernel.h>
#include <psprtc.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>

#include "inferno.h"

// 0x00002784
struct IoReadArg g_read_arg;

// 0x00002484
void *g_sector_buf = NULL;

// 0x0000248C
int g_iso_opened = 0;

// 0x000023D0
SceUID g_iso_fd = -1;

// 0x000023D4
int g_total_sectors = -1;

// 0x00002488
static int g_is_compressed = ISO;

// 0x000024C0
static void *g_block_buf = NULL;

// 0x000024C4, size DEC_BUFFER_SIZE + (1 << g_CISO_hdr.align), align 64
static void *g_dec_buf = NULL;
static void *g_com_buf = NULL;

// 0x00002704
static int g_cur_idx = 0;

static int g_cur_pos = 0;

// 0x00002700
static u32 g_dec_buf_offset = (u32)-1;

static int g_dec_buf_size = 0;

// 0x00002720
static int g_total_block = 0;

struct CISO_header {
	u8 magic[4];		// 0
	u32 header_size;	// 4
	u64 total_bytes;	// 8
	u32 block_size;		// 16
	u8 ver;				// 20
	u8 align;			// 21
	u8 rsv_06[2];		// 22
};

struct DAXheader {
	u8 magic[4];
	u32 total_bytes;
	u32 version;
	u32 nNCareas;
	u32 reserved[4];
};

struct NCArea {
	u32 frame;
	u32 size;
};

// 0x00002708
static struct CISO_header g_CISO_hdr __attribute__((aligned(64)));
static struct DAXheader g_DAX_hdr __attribute__((aligned(64)));

static struct NCArea ncareas[MAX_NCAREAS];

// 0x00002500
static u32 g_idx_cache[IDX_BUFFER_SIZE/4] __attribute__((aligned(64)));
static u16 g_len_cache[IDX_BUFFER_SIZE/4] __attribute__((aligned(64)));

// 0x00000368
static void wait_until_ms0_ready(void)
{
	int ret, status = 0, bootfrom;
	const char *drvname;

	drvname = "mscmhc0:";

	if(psp_model == 4) {
		bootfrom = sctrlKernelBootFrom();

		if(bootfrom == 0x50) {
			drvname = "mscmhcemu0:";
		} else {
			// vsh mode?
			return;
		}
	}

	while( 1 ) {
		ret = sceIoDevctl(drvname, 0x02025801, 0, 0, &status, sizeof(status));

		if(ret < 0) {
			sceKernelDelayThread(20000);
			continue;
		}

		if(status == 4) {
			break;
		}

		sceKernelDelayThread(20000);
	}
}

// 0x00000E58
static int get_nsector(void)
{
	int ret;

	if(g_is_compressed == CSO) {
		ret = g_total_block;
	} else if(g_is_compressed == DAX) {
		ret = g_total_block * 4;
	} else {
		SceOff offset = sceIoLseek(g_iso_fd, 0, PSP_SEEK_CUR);
		int size = sceIoLseek(g_iso_fd, 0, PSP_SEEK_END);
		sceIoLseek(g_iso_fd, offset, PSP_SEEK_SET);

		if(offset < 0) {
			return (int)offset;
		}

		ret = size / ISO_SECTOR_SIZE;
	}

	return ret;
}

// 0x00000F00
static int is_ciso(SceUID fd)
{
	int ret;
	u32 *magic;

	g_CISO_hdr.magic[0] = '\0';
	g_dec_buf_offset = (u32)-1;
	g_dec_buf_size = 0;

	sceIoLseek(fd, 0, PSP_SEEK_SET);
	ret = sceIoRead(fd, &g_CISO_hdr, sizeof(g_CISO_hdr));

	if(ret != sizeof(g_CISO_hdr)) {
		ret = -1;
		goto exit;
	}

	magic = (u32*)g_CISO_hdr.magic;

	if(*magic == CISO_MAGIC) {
		g_cur_idx = -1;
		g_total_block = g_CISO_hdr.total_bytes / g_CISO_hdr.block_size;

		if(g_dec_buf == NULL) {
			g_dec_buf = sctrlKernelMalloc(DEC_BUFFER_SIZE + (1 << g_CISO_hdr.align) + 64);

			if(g_dec_buf == NULL) {
				ret = -2;
				goto exit;
			}

			if((u32)g_dec_buf & 63)
				g_dec_buf = (void*)(((u32)g_dec_buf & (~63)) + 64);
		}

		if(g_block_buf == NULL) {
			g_block_buf = sctrlKernelMalloc(ISO_SECTOR_SIZE + 64);

			if(g_block_buf == NULL) {
				ret = -3;
				goto exit;
			}

			if((u32)g_block_buf & 63)
				g_block_buf = (void*)(((u32)g_block_buf & (~63)) + 64);
		}

		ret = 0;
	} else {
		ret = 0x8002012F;
	}

exit:
	return ret;
}

int Is_NCArea(unsigned int frame, int count)
{
	int i;

	for (i = 0; i < count; i++) {
		if (frame >= ncareas[i].frame && ((frame - ncareas[i].frame) < ncareas[i].size)) {
			return 1;
		}
	}

	return 0;
}

static int is_dax(SceUID fd)
{
	int ret;
	u32 *magic;

	g_DAX_hdr.magic[0] = '\0';
	g_dec_buf_offset = (u32)-1;
	g_dec_buf_size = 0;

	sceIoLseek(fd, 0, PSP_SEEK_SET);
	ret = sceIoRead(fd, &g_DAX_hdr, sizeof(g_DAX_hdr));
	if(ret < 0) {
		ret = -1;
		goto exit;
	}

	magic = (u32 *)g_DAX_hdr.magic;

	if(*magic == DAX_MAGIC) {
		if (g_DAX_hdr.version <= DAX_VERSION_1) {
			g_total_block = g_DAX_hdr.total_bytes / DEC_BUFFER_SIZE;
			if ((g_DAX_hdr.total_bytes % DEC_BUFFER_SIZE) != 0) {
				g_total_block++;
			}

			if(g_com_buf == NULL) {
				g_com_buf = sctrlKernelMalloc(DEC_BUFFER_SIZE + 1024);

				if(g_com_buf == NULL) {
					ret = -2;
					goto exit;
				}
			}

			if(g_dec_buf == NULL) {
				g_dec_buf = sctrlKernelMalloc(DEC_BUFFER_SIZE);

				if(g_dec_buf == NULL) {
					ret = -3;
					goto exit;
				}
			}

			if(g_block_buf == NULL) {
				g_block_buf = sctrlKernelMalloc(ISO_SECTOR_SIZE);

				if(g_block_buf == NULL) {
					ret = -4;
					goto exit;
				}
			}
			
			if (g_DAX_hdr.nNCareas > 0)
			{
				sceIoLseek(fd, sizeof(g_DAX_hdr) + g_total_block * 4 + g_total_block * 2, PSP_SEEK_SET);

				ret = sceIoRead(fd, ncareas, sizeof(ncareas));
				if(ret < 0)
				{
					ret = -1;
					goto exit;
				}
			}

			g_cur_idx = -1;
			g_cur_pos = -1;

			ret = 0;
		} else {
			ret = -5;
		}
	} else {
		ret = 0x8002012F;
	}

exit:
	return ret;
}

// 0x000009D4
int iso_open(void)
{
	int retries;

	wait_until_ms0_ready();
	sceIoClose(g_iso_fd);
	g_iso_opened = 0;
	retries = 0;

	do {
		g_iso_fd = sceIoOpen(g_iso_fn, 0x000F0001, 0777);

		if(g_iso_fd < 0) {
			if(++retries >= 16) {
				return -1;
			}

			sceKernelDelayThread(20000);
		}
	} while(g_iso_fd < 0);

	if(g_iso_fd < 0) {
		return -1;
	}

	g_is_compressed = ISO;

	if(is_ciso(g_iso_fd) >= 0) {
		g_is_compressed = CSO;
	} else if (is_dax(g_iso_fd) >= 0) {
		g_is_compressed = DAX;
	}

	g_iso_opened = 1;
	g_total_sectors = get_nsector();

	return 0;
}

// 0x00000BB4
static int read_raw_data(u8* addr, u32 size, u32 offset)
{
	int ret, i;
	SceOff ofs;

	i = 0;

	do {
		i++;
		ofs = sceIoLseek(g_iso_fd, offset, PSP_SEEK_SET);

		if(ofs >= 0) {
			i = 0;
			break;
		} else {
			iso_open();
		}
	} while(i < 16);

	if(i == 16) {
		ret = 0x80010013;
		goto exit;
	}

	for(i = 0; i < 16; ++i) {
		ret = sceIoRead(g_iso_fd, addr, size);

		if(ret >= 0) {
			i = 0;
			break;
		} else {
			iso_open();
		}
	}

	if(i == 16) {
		ret = 0x80010013;
		goto exit;
	}

exit:
	return ret;
}

// 0x00001018
static int read_cso_sector(u8 *addr, int sector)
{
	int ret;
	int n_sector;
	u32 offset, next_offset;
	int size;

	n_sector = sector - g_cur_idx;

	// not within sector idx cache?
	if(g_cur_idx == -1 || n_sector < 0 || n_sector >= NELEMS(g_idx_cache)) {
		ret = read_raw_data((u8*)g_idx_cache, sizeof(g_idx_cache), (sector << 2) + sizeof(struct CISO_header));

		if(ret < 0) {
			ret = -4;
			return ret;
		}

		g_cur_idx = sector;
		n_sector = 0;
	}

	offset = (g_idx_cache[n_sector] & 0x7FFFFFFF) << g_CISO_hdr.align;

	// is plain?
	if(g_idx_cache[n_sector] & 0x80000000) {
		return read_raw_data(addr, ISO_SECTOR_SIZE, offset);
	}

	sector++;
	n_sector = sector - g_cur_idx;

	if(g_cur_idx == -1 || n_sector < 0 || n_sector >= NELEMS(g_idx_cache)) {
		ret = read_raw_data((u8*)g_idx_cache, sizeof(g_idx_cache), (sector << 2) + sizeof(struct CISO_header));

		if(ret < 0) {
			ret = -5;
			return ret;
		}

		g_cur_idx = sector;
		n_sector = 0;
	}

	next_offset = (g_idx_cache[n_sector] & 0x7FFFFFFF) << g_CISO_hdr.align;
	size = next_offset - offset;
	
	if(g_CISO_hdr.align)
		size += 1 << g_CISO_hdr.align;

	if(size <= ISO_SECTOR_SIZE)
		size = ISO_SECTOR_SIZE;

	if(g_dec_buf_offset == (u32)-1 || offset < g_dec_buf_offset || offset + size >= g_dec_buf_offset + g_dec_buf_size) {
		ret = read_raw_data(g_dec_buf, size, offset);

		/* May not reach DEC_BUFFER_SIZE */	
		if(ret < 0) {
			g_dec_buf_offset = (u32)-1;
			ret = -6;
			return ret;
		}

		g_dec_buf_offset = offset;
		g_dec_buf_size = ret;
	}

	ret = sceKernelDeflateDecompress(addr, ISO_SECTOR_SIZE, g_dec_buf + offset - g_dec_buf_offset, 0);

	return ret < 0 ? ret : ISO_SECTOR_SIZE;
}

static int read_dax_sector(u8 *addr, int sector)
{
	int g_DAX_pos = sector - (sector % 4);

	if (g_cur_pos == -1 || g_DAX_pos != g_cur_pos) {
		int ret;
		int n_sector = (g_DAX_pos - g_cur_idx) / 4;

		if (g_cur_idx == -1 || n_sector < 0 || n_sector >= NELEMS(g_idx_cache)) {
			memset(g_idx_cache, 0, sizeof(g_idx_cache));

			ret = read_raw_data((u8*)g_idx_cache, sizeof(g_idx_cache), sizeof(struct DAXheader) + g_DAX_pos);
			if (ret != sizeof(g_idx_cache)) {
				return -20;
			}

			memset(g_len_cache, 0, sizeof(g_len_cache));

			ret = read_raw_data((u8*)g_len_cache, sizeof(g_len_cache), sizeof(struct DAXheader) + get_nsector() + (g_DAX_pos / 2));
			if (ret != sizeof(g_len_cache)) {
				return -21;
			}

			g_cur_idx = g_DAX_pos;
			n_sector = 0;
		}

		u32 offset = g_idx_cache[n_sector];
		int size = g_len_cache[n_sector];
		g_cur_pos = g_DAX_pos;

		if (g_dec_buf_offset == (u32)-1 || offset < g_dec_buf_offset || offset + size >= g_dec_buf_offset + g_dec_buf_size) {
			memset(g_com_buf, 0, DEC_BUFFER_SIZE + 1024);

			ret = read_raw_data(g_com_buf, size, offset);
			if (ret < 0) {
				g_dec_buf_offset = (u32)-1;
				return -22;
			}

			g_dec_buf_offset = offset;
			g_dec_buf_size = ret;
		}

		memset(g_dec_buf, 0, DEC_BUFFER_SIZE);

		if (Is_NCArea(g_DAX_pos / 4, g_DAX_hdr.nNCareas)) {
			memcpy(g_dec_buf, g_com_buf, DEC_BUFFER_SIZE);
		} else {
			ret = sctrlKernelUncompress(g_dec_buf, DEC_BUFFER_SIZE, g_com_buf, size);			
		}
	}

	memcpy(addr, g_dec_buf + (ISO_SECTOR_SIZE * (sector % 4)), ISO_SECTOR_SIZE);

	return ISO_SECTOR_SIZE;
}

static int read_com_data(u8* addr, u32 size, u32 offset)
{
	u32 cur_block;
	int pos, ret, read_bytes;
	u32 o_offset = offset;

	while(size > 0) {
		cur_block = offset / ISO_SECTOR_SIZE;
		pos = offset & (ISO_SECTOR_SIZE - 1);

		if(cur_block >= get_nsector()) {
			// EOF reached
			break;
		}

		if (g_is_compressed == CSO) {
			ret = read_cso_sector(g_block_buf, cur_block);
		} else {
			ret = read_dax_sector(g_block_buf, cur_block);
		}

		if(ret != ISO_SECTOR_SIZE) {
			ret = -7;
			return ret;
		}

		read_bytes = MIN(size, (ISO_SECTOR_SIZE - pos));
		memcpy(addr, g_block_buf + pos, read_bytes);
		size -= read_bytes;
		addr += read_bytes;
		offset += read_bytes;
	}

	return offset - o_offset;
}

// 0x00000C7C
int iso_read(struct IoReadArg *args)
{
	if(g_is_compressed != ISO) {
		return read_com_data(args->address, args->size, args->offset);
	}

	return read_raw_data(args->address, args->size, args->offset);
}

// 0x000003E0
int iso_read_with_stack(u32 offset, void *ptr, u32 data_len)
{
	int ret, retv;

	ret = sceKernelWaitSema(g_umd9660_sema_id, 1, 0);

	if(ret < 0) {
		return -1;
	}

	g_read_arg.offset = offset;
	g_read_arg.address = ptr;
	g_read_arg.size = data_len;
	retv = sceKernelExtendKernelStack(0x2000, (void*)&iso_cache_read, &g_read_arg);

	ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

	if(ret < 0) {
		return -1;
	}

	return retv;
}

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspcrypt.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <pspdecrypt.h>
#include <systemctrl.h>

#include "keys_0x10_byte.h"
#include "keys_0x90_byte.h"

extern int UtilsForKernel_6C6887EE(void *, u32, void *, void *);

////////// Decryption 1 //////////
static TAG_INFO const* GetTagInfo(u32 tagFind)
{
	int iTag;

	for (iTag = 0; iTag < sizeof(g_tagInfo)/sizeof(TAG_INFO); iTag++)
	{
		if (g_tagInfo[iTag].tag == tagFind)
		{
			return &g_tagInfo[iTag];
		}
	}

	return NULL;
}

static void ExtraV2Mangle(u8* buffer1, u8 codeExtra)
{
	static u8 g_dataTmp[20+0xA0] __attribute__((aligned(0x40)));
	u8* buffer2 = g_dataTmp;

	memcpy(buffer2+20, buffer1, 0xA0);
	u32* pl2 = (u32*)buffer2;
	pl2[0] = 5;
	pl2[1] = pl2[2] = 0;
	pl2[3] = codeExtra;
	pl2[4] = 0xA0;

	int ret = sceUtilsBufferCopyWithRange(buffer2, 20+0xA0, buffer2, 20+0xA0, 7);
	if (ret != 0)
	Kprintf("Extra de-mangle returns %d\n", ret);

	memcpy(buffer1, buffer2, 0xA0);
}

static int Scramble(u32 *buf, u32 size, u32 code)
{
	buf[0] = 5;
	buf[1] = buf[2] = 0;
	buf[3] = code;
	buf[4] = size;

	if (sceUtilsBufferCopyWithRange(buf, size+0x14, buf, size+0x14, 7) < 0)
	{
		return -1;
	}

	return 0;
}

static int DecryptPRX1(const u8* pbIn, u8* pbOut, int cbTotal, u32 tag)
{
	int i, retsize;
	u8 bD0[0x80], b80[0x50], b00[0x80], bB0[0x20];
	
	TAG_INFO const* pti = GetTagInfo(tag);
	if (pti == NULL)
	{
		return -1;
	}

	retsize = *(u32*)&pbIn[0xB0];

	for (i = 0; i < 0x14; i++)
	{
		if (pti->key[i] != 0)
			break;
	}
	
	if (i == 0x14)
	{
		Scramble((u32 *)pti->key, 0x90, pti->code);
	}

	if (pbIn != pbOut)
	{
		memcpy(pbOut, pbIn, cbTotal);
	}
		
	memcpy(bD0, pbIn+0xD0, 0x80);
	memcpy(b80, pbIn+0x80, 0x50);
	memcpy(b00, pbIn+0x00, 0x80);
	memcpy(bB0, pbIn+0xB0, 0x20);

	memset(pbOut, 0, 0x150);
	memset(pbOut, 0x55, 0x40);

	u32* pl = (u32*)(pbOut+0x2C);
	pl[0] = 5;
	pl[1] = pl[2] = 0;
	pl[3] = pti->code;
	pl[4] = 0x70;

	u8 buffer1[0x150];
    memcpy(buffer1+0x00, bD0, 0x80);
    memcpy(buffer1+0x80, b80, 0x50);
    memcpy(buffer1+0xD0, b00, 0x80);

	if (pti->codeExtra != 0)
	{
		ExtraV2Mangle(buffer1+0x10, pti->codeExtra);
	}

	memcpy(pbOut+0x40, buffer1+0x40, 0x70);

	int ret;
	int iXOR;
	for (iXOR = 0; iXOR < 0x70; iXOR++)
	{
		pbOut[0x40+iXOR] = pbOut[0x40+iXOR] ^ pti->key[0x14+iXOR];
	}

	ret = sceUtilsBufferCopyWithRange(pbOut+0x2C, 20+0x70, pbOut+0x2C, 20+0x70, 7);
	if (ret != 0)
	{
		Kprintf("Mangle#7 returned 0x%08X\n", ret);
		return -1;
	}

	for (iXOR = 0x6F; iXOR >= 0; iXOR--)
	{
		pbOut[0x40+iXOR] = pbOut[0x2C+iXOR] ^ pti->key[0x20+iXOR];
	}

	memset(pbOut+0xA0, 0, 0x10);
	pbOut[0xA0] = 1;

    memcpy(pbOut+0xB0, bB0, 0x20);
    memcpy(pbOut+0xD0, b00, 0x80);

	ret = sceUtilsBufferCopyWithRange(pbOut, cbTotal, pbOut+0x40, cbTotal-0x40, 0x1);
	if (ret != 0)
    {
		pbOut[0xA4] = 1;

		ret = sceUtilsBufferCopyWithRange(pbOut, cbTotal, pbOut+0x40, cbTotal-0x40, 0x1);

		if (ret != 0)
		{
			Kprintf("Mangle#1 returned $%x\n", ret);
			return -1;
		}
    }

	return retsize;
}

////////// Decryption 2 //////////
static TAG_INFO2 *GetTagInfo2(u32 tagFind)
{
	int iTag;

	for (iTag = 0; iTag < sizeof(g_tagInfo2) / sizeof(TAG_INFO2); iTag++)
	{
		if (g_tagInfo2[iTag].tag == tagFind)
		{
			return &g_tagInfo2[iTag];
		}
	}

	return NULL;
}

static int DecryptPRX2(const u8 *inbuf, u8 *outbuf, u32 size, u32 tag)
{
	TAG_INFO2 * pti = GetTagInfo2(tag);

	if (!pti)
	{
		return -1;
	}

	int retsize = *(int *)&inbuf[0xB0];
	u8 tmp1[0x150], tmp2[0x90+0x14], tmp3[0x60+0x14];

	memset(tmp1, 0, 0x150);
	memset(tmp2, 0, 0x90+0x14);
	memset(tmp3, 0, 0x60+0x14);

	memcpy(outbuf, inbuf, size);

	if (size < 0x160)
	{
		Kprintf("Buffer not big enough\n");
		return -2;
	}

	if (((u32)outbuf & 0x3F))
	{
		Kprintf("Buffer not aligned to 64 bytes\n");
		return -3;
	}

	if ((size - 0x150) < retsize)
	{
		Kprintf("Not enough data\n");
		return -4;
	}

	memcpy(tmp1, outbuf, 0x150);

	int i, j;
	u8 *p = tmp2+0x14;

	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			p[(i << 4) + j] = pti->key[j];
		}

		p[(i << 4)] = i;
	}	

	if (Scramble((u32 *)tmp2, 0x90, pti->code) < 0)
	{
		Kprintf("Error in Scramble#1\n");
		return -5;
	}

	memcpy(outbuf, tmp1+0xD0, 0x5C);
	memcpy(outbuf+0x5C, tmp1+0x140, 0x10);
	memcpy(outbuf+0x6C, tmp1+0x12C, 0x14);
	memcpy(outbuf+0x80, tmp1+0x080, 0x30);
	memcpy(outbuf+0xB0, tmp1+0x0C0, 0x10);
	memcpy(outbuf+0xC0, tmp1+0x0B0, 0x10);
	memcpy(outbuf+0xD0, tmp1+0x000, 0x80);

	memcpy(tmp3+0x14, outbuf+0x5C, 0x60);	

	if (Scramble((u32 *)tmp3, 0x60, pti->code) < 0)
	{
		Kprintf("Error in Scramble#2\n");
		return -6;
	}

	memcpy(outbuf+0x5C, tmp3, 0x60);
	memcpy(tmp3, outbuf+0x6C, 0x14);
	memcpy(outbuf+0x70, outbuf+0x5C, 0x10);

	memset(outbuf+0x18, 0, 0x58);
	memcpy(outbuf+0x04, outbuf, 0x04);

	*((u32 *)outbuf) = 0x014C;
	memcpy(outbuf+0x08, tmp2, 0x10);

	if (sceUtilsBufferCopyWithRange(outbuf, 3000000, outbuf, 3000000, 0x0B) != 0)
	{
		Kprintf("Error in sceUtilsBufferCopyWithRange 0xB\n");
		return -7;
	}	

	if (memcmp(outbuf, tmp3, 0x14) != 0)
	{
		//don't know why it thinks that 6.30 stuff is the wrong SHA-1... it's not.
		//if(!using_kprx) Kprintf("WARNING (SHA-1 incorrect)\n");
		//return -8;
	}
	
	int iXOR;

	for (iXOR = 0; iXOR < 0x40; iXOR++)
	{
		tmp3[iXOR+0x14] = outbuf[iXOR+0x80] ^ tmp2[iXOR+0x10];
	}

	if (Scramble((u32 *)tmp3, 0x40, pti->code) != 0)
	{
		Kprintf("Error in Scramble#3\n");
		return -9;
	}
	
	for (iXOR = 0x3F; iXOR >= 0; iXOR--)
	{
		outbuf[iXOR+0x40] = tmp3[iXOR] ^ tmp2[iXOR+0x50];
	}

	memset(outbuf+0x80, 0, 0x30);
	*(u32 *)&outbuf[0xA0] = 1;

	memcpy(outbuf+0xB0, outbuf+0xC0, 0x10);
	memset(outbuf+0xC0, 0, 0x10);

	int ret = sceUtilsBufferCopyWithRange(outbuf, size, outbuf+0x40, size-0x40, 0x1);
	if (ret != 0x00000000)
	{
		if(ret != 0x00000003)
		{
			Kprintf("Error in sceUtilsBufferCopyWithRange 0x1 (0x%08X)\n", ret);
		}

		return -1;
	}

	if (retsize < 0x150)
	{
		memset(outbuf+retsize, 0, 0x150-retsize);		
	}

	return retsize;
}

u8 g_kernel_phat_key[0x10] = { 0 };
u8 g_kernel_slim_key[0x10] = { 0 };
u8 buf_1[0x150] = { 0 };
u8 buf_2[0xb4] = { 0 };
u8 buf_3[0x150] = { 0 };
u8 buf_4[0x90] = { 0 };
u8 buf_5[0x20] = { 0 };

void prx_xor_key_into(u8 *dstbuf, u32 size, u8 *srcbuf, u8 *xor_key)
{
	u32 i;

	i = 0;

	while (i < size)
	{
		dstbuf[i] = srcbuf[i] ^ xor_key[i];
		++i;
	}
}

int kirk7(u8* prx, u32 size, u32 scramble_code, u32 use_polling)
{
	int ret;

	((u32 *) prx)[0] = 5;
	((u32 *) prx)[1] = 0;
	((u32 *) prx)[2] = 0;
	((u32 *) prx)[3] = scramble_code;
	((u32 *) prx)[4] = size;

	if (!use_polling)
	{
		ret = sceUtilsBufferCopyWithRange(prx, size + 20, prx, size + 20, 7);
	}
	else
	{
		ret = sceUtilsBufferCopyWithRange(prx, size + 20, prx, size + 20, 7);
	}

	return ret;
}

int _kprx_decrypt(u8 *prx, u32 size, u32 *newsize, u32 use_polling)
{
	int ret;
	u32 type = 0, scramble_code = 0, i=0;
	u8 *key_addr = NULL;
	u8 *blacklist = NULL;
	u32 blacklistsize = 0;

	if (prx == NULL || newsize == NULL)
	{
		ret = -201;
		goto exit;
	}

	if (size < 0x160)
	{
		ret = -202;
		goto exit;
	}

	if ((u32)prx & 0x3f)
	{
		ret = -203;
		goto exit;
	}

	if (!((0x00220202 >> ((((u32)prx) >> 27) & 0x0000001F)) & 1))
	{
		ret = -204;
		goto exit;
	}

	memcpy(buf_3, prx, 0x150);
	
	if (0x4C9494F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys660_k1;
		scramble_code = 0x43;
		type = 0;

	}
	else if (0x4C9495F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys660_k2;
		scramble_code = 0x43;
		type = 0;

	}
	else if (0x457B90F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys660_v1;
		scramble_code = 0x5B;
		type = 0;

	}
	else if (0x457B91F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys660_v7;
		scramble_code = 0x5B;
		type = 0;

	}
	else if (0x380290F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys660_v1;
		scramble_code = 0x5A;
		type = 0;

	}
	else if (0x380291F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys660_v2;
		scramble_code = 0x5A;
		type = 0;

	}
	else if (0x4C948DF0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys638_k4;
		scramble_code = 0x43;
		type = 3;

	}
	else if (0x4C948AF0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys636_k1;
		scramble_code = 0x43;
		type = 3;

	}
	else if (0x4C948BF0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys636_k2;
		scramble_code = 0x43;
		type = 3;

	}
	else if (0x4C948CF0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys636_k3;
		scramble_code = 0x43;
		type = 3;

	}
	else if (0x457B81F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys630_k4;
		scramble_code = 0x5B;
		type = 3;
	}
	else if (0x457B82F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys630_k5;
		scramble_code = 0x5B;
		type = 3;

	}
	else if (0x457B83F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys630_k7;
		scramble_code = 0x5B;
		type = 3;

	}
	else if (0x4C9485F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys630_k3;
		scramble_code = 0x43;
		type = 3;

	}
	else if (0x4C9486F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys630_k6;
		scramble_code = 0x43;
		type = 3;

	}
	else if (0x4C9487F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys630_k8;
		scramble_code = 0x43;
		type = 3;

	}
	else if (0x380283F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = key_380283F0;
		scramble_code = 0x5A;
		type = 3;

	}
	else if (0x380280f0 == *(u32*)&buf_3[0xD0])
	{
		blacklist = NULL;
		blacklistsize = 0;
		key_addr = key_380280f0;
		scramble_code = 0x5A;
		type = 3;
	}
	else if (0x4C9484F0 == *(u32*)&buf_3[0xD0])
	{
		blacklist = NULL;
		blacklistsize = 0;
		key_addr = keys630_k1;
		scramble_code = 0x43;
		type = 3;
	}
	else if (0x457b80f0 == *(u32*)&buf_3[0xD0])
	{
		blacklist = NULL;
		blacklistsize = 0;
		key_addr = keys630_k2;
		scramble_code = 0x5B;
		type = 3;
	}
	else if (0x4C941DF0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys620_1;
		scramble_code = 0x43;
		type = 2;
	}
	else if (0x4C940FF0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = key_2DA8;
		scramble_code = 0x43;
		type = 2;
	}
	else if (0x4C941CF0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys620_0;
		scramble_code = 0x43;
		type = 2;
	}
	else if (0x4C940AF0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = key_2DB8;
		scramble_code = 0x43;
		type = 2;
	}
	else if (0xCFEF08F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = key_2D90;
		scramble_code = 0x62;
		type = 2;
	}
	else if (0xCFEF06F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = key_2D80;
		scramble_code = 0x62;
		type = 2;
	}
	else if (0xCFEF05F0 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys280_0;
		scramble_code = 0x62;
		type = 2;
	}
	else if (0x16D59E03 == *(u32*)&buf_3[0xD0])
	{
		key_addr = keys260_0;
		scramble_code = 0x62;
		type = 2;
	}
	else if (0x4467415D == *(u32*)&buf_3[0xD0])
	{
		key_addr = key_22E0;
		scramble_code = 0x59;
		type = 1;
	}
	else if (0x00000000 == *(u32*)&buf_3[0xD0])
	{
		key_addr = key_21C0;
		blacklist = NULL;
		blacklistsize = 0;
		scramble_code = 0x42;
		type = 0;
	}
	else if (0x01000000 == *(u32*)&buf_3[0xD0])
	{
		key_addr = key_2250;
		scramble_code = 0x43;
		type = 0;
	}
	else
	{
		ret = -301;
		goto exit;
	}

	if (type == 2)
	{
		u8 *p = buf_3;

		i = 0;
		while (i<0x58)
		{
			if (p[0xD4])
			{
				ret = -302;
				goto exit;
			}

			i++;
			p++;
		}
	}

	if (type == 3)
	{
		u8 *p = buf_3;

		i = 0;
		while (i<0x38)
		{
			if (p[0xD4])
			{
				ret = -302;
				goto exit;
			}

			i++;
			p++;
		}
	}

	*newsize = *(u32*)&prx[0xB0];

	if (size - 0x150 < *newsize)
	{
		ret = -206;
		goto exit;
	}

	if (type == 2 || type == 3)
	{
		int i;

		for (i=0; i<9; i++)
		{
			memcpy(buf_1 + 0x14 + (i << 4), key_addr, 0x10);
			buf_1[0x14+ (i<<4)] = i;
		}

		memset(g_kernel_phat_key, 0, 0x10);
		memset(g_kernel_slim_key, 0, 0x10);
	}
	else
	{
		memcpy(buf_1+20, key_addr, 0x90);
	}

	ret = kirk7(buf_1, 0x90, scramble_code, use_polling);

	if (ret != 0)
	{
		if (ret < 0)
		{
			ret = -101;
		}
		else
		{
			if (ret == 0xC)
			{
				ret = -107;
			}
			else
			{
				ret = -104;
			}
		}

		goto exit;
	}

	memcpy(buf_4, buf_1, 0x90);

	if (type == 2 || type == 3)
	{
		memcpy(buf_1, &buf_3[0xD0], 0x5C);
		memcpy(buf_1+0x5C, &buf_3[0x140], 0x10);
		memcpy(buf_1+0x5C+0x10, &buf_3[0x12c], 0x14);
		memcpy(buf_1+0x5C+0x10+0x14, &buf_3[0x80], 0x30);
		memcpy(buf_1+0x5C+0x10+0x14+0x30, &buf_3[0xc0], 0x10);
		memcpy(buf_1+0x5C+0x10+0x14+0x30+0x10, &buf_3[0xb0], 0x10);
		memcpy(buf_1+0x5C+0x10+0x14+0x30+0x10+0x10, &buf_3[0], 0x80);
	}
	else
	{
		memcpy(buf_1, &buf_3[0xD0], 0x80);
		memcpy(buf_1+0x80, &buf_3[0x80], 0x50);
		memcpy(buf_1+0x80+0x50, &buf_3[0], 0x80);
	}

	if (type == 1)
	{
		memcpy(buf_2+0x14, buf_1+0x10, 0xA0);
		ret = kirk7(buf_2, 0xA0, scramble_code, use_polling);

		if (ret != 0)
		{
			if (ret >= 0)
			{
				if (ret == 0xC)
				{
					ret = -107;
				}
				else
				{
					ret = -106;
				}
			}
			else
			{
				ret = -103;
			}

			memcpy(prx, buf_3, 0x150);
			goto exit;
		}

		memcpy(buf_1+0x10, buf_2, 0xA0);
	}

	if (type == 2 || type == 3)
	{
		memcpy(buf_2+20, buf_1+92, 0x60);
		ret = kirk7(buf_2, 0x60, scramble_code, use_polling);

		if (ret != 0)
		{
			if (ret >= 0)
			{
				if (ret == 0xC)
				{
					ret = -107;
				}
				else
				{
					ret = -106;
				}
			}
			else
			{
				ret = -103;
			}

			memcpy(prx, buf_3, 0x150);
			goto exit;
		}

		memcpy(buf_1+92, buf_2, 0x60);
	}

	if (type == 2 || type == 3)
	{
		memcpy(buf_2, buf_1+108, 0x14);
		memcpy(buf_1+112, buf_1+92, 0x10);

		if (type == 3)
		{
			memcpy(buf_5, buf_1+60, 0x20);
			memcpy(buf_1+80, buf_5, 0x20);
			memset(buf_1+24, 0, 0x38);
		}
		else
		{
			memset(buf_1+24, 0, 0x58);
		}

		memcpy(buf_1+4, buf_1, 4);
		*(u32*)buf_1 = 0x14C;
		memcpy(buf_1+8, buf_4, 0x10);
		memset(buf_4, 0, 0x10);
	}
	else
	{
		memcpy(buf_2, buf_1+4, 0x14);
		*(u32*)buf_1 = 0x14C;
		memcpy(buf_1+4, buf_4, 0x14);
	}

	if (!use_polling)
	{
		ret = sceUtilsBufferCopyWithRange(buf_1, 0x150, buf_1, 0x150, 0xB);
	}
	else
	{
		ret = sceUtilsBufferCopyWithRange(buf_1, 0x150, buf_1, 0x150, 0xB);
	}

	if (ret != 0)
	{
		if (ret > 0)
		{
			if (ret == 0xC)
			{
				ret = -107;
			}
			else
			{
				ret = -106;
			}
		}
		else
		{
			ret = -102;
		}

		goto exit;
	}

	ret = memcmp(buf_1, buf_2, 0x14);

	if (ret != 0)
	{
		ret = -302;
		goto exit;
	}

	if (type == 2 || type == 3)
	{
		prx_xor_key_into(buf_1+128, 0x40, buf_1+128, buf_4+16);
		memset(buf_4+16, 0, 0x40);
		ret = kirk7(buf_1+0x6C, 0x40, scramble_code, use_polling);

		if (ret != 0)
		{
			if (ret > 0)
			{
				if (ret == 0xC)
				{
					ret = -107;
				}
				else
				{
					ret = -106;
				}
			}
			else
			{
				ret = -102;
			}

			goto exit;
		}

		prx_xor_key_into(prx+64, 0x40, buf_1+108, buf_4+80);
		memset(buf_4+80, 0, 0x40);

		if (type == 3)
		{
			memcpy(prx+128, buf_5, 0x20);
			memset(prx+160, 0, 0x10);
			prx[164] = 1;
			prx[160] = 1;
		}
		else
		{
			memset(prx+128, 0, 0x30);
			prx[160] = 1;
		}

		memcpy(prx+176, buf_1+192, 0x10);
		memset(prx+192, 0, 0x10);
		memcpy(prx+208, buf_1+208, 0x80);
	}
	else
	{
		prx_xor_key_into(buf_1+64, 0x70, buf_1+64, buf_4+20);
		ret = kirk7(buf_1+0x2C, 0x70, scramble_code, use_polling);

		if (ret != 0)
		{
			if (ret > 0)
			{
				if (ret == 0xC)
				{
					ret = -107;
				}
				else
				{
					ret = -106;
				}
			}
			else
			{
				ret = -102;
			}

			goto exit;
		}

		prx_xor_key_into(prx+64, 0x70, buf_1+44, buf_4+32);
		memcpy(prx+176, buf_1+176, 0xA0);
	}

	if (!use_polling)
	{
		ret = sceUtilsBufferCopyWithRange(prx, size, prx+0x40, size-0x40, 1);
	}
	else
	{
		ret = sceUtilsBufferCopyWithRange(prx, size, prx+0x40, size-0x40, 1);
	}

	if (ret != 0)
	{
		if (ret > 0)
		{
			if (ret == 0xC)
			{
				ret = -107;
			}
			else
			{
				ret = -304;
			}
		}
		else
		{
			ret = -303;
		}

		goto exit;
	}

	if (*newsize < 0x150)
	{
		memset(prx+*newsize, 0, 0x150-*newsize);		
	}

	ret = 0;
exit:
	memset(g_kernel_phat_key, 0, 0x10);
	memset(g_kernel_slim_key, 0, 0x10);
	memset(buf_1, 0, 0x150);
	memset(buf_2, 0, 0xB4);
	memset(buf_3, 0, 0x150);
	memset(buf_4, 0, 0x90);
	memset(buf_5, 0, 0x20);

	return ret;
}

static int _pspDecryptPRX(u32 *arg)
{
	u8 *inbuf = (u8 *)arg[0];
	u8 *outbuf = (u8 *)arg[1];
	u32 size = arg[2];

	int retsize = DecryptPRX1(inbuf, outbuf, size, *(u32 *)&inbuf[0xD0]);

	if (retsize <= 0)
	{
		retsize = DecryptPRX2(inbuf, outbuf, size, *(u32 *)&inbuf[0xD0]);
	}
	
	if (retsize <= 0)
	{
		u32 newsize = 0;
		u32 ret;

		memcpy(outbuf, inbuf, size);
		ret = _kprx_decrypt(outbuf, size, &newsize, 0);

		if (ret == 0)
		{
			return newsize;
		}
		else
		{
			return ret;
		}
	}

	return retsize;
}

int pspDecryptPRX(u8 *inbuf, u8 *outbuf, u32 size)
{
	int k1 = pspSdkSetK1(0);
	u32 arg[3];

	arg[0] = (u32)inbuf;
	arg[1] = (u32)outbuf;
	arg[2] = size;
	
	int res = sceKernelExtendKernelStack(0x2000, (void *)_pspDecryptPRX, arg);

	pspSdkSetK1(k1);
	return res;
}

////////// SignCheck //////////

u8 check_keys0[0x10] =
{
	0x71, 0xF6, 0xA8, 0x31, 0x1E, 0xE0, 0xFF, 0x1E,
	0x50, 0xBA, 0x6C, 0xD2, 0x98, 0x2D, 0xD6, 0x2D
};

u8 check_keys1[0x10] =
{
	0xAA, 0x85, 0x4D, 0xB0, 0xFF, 0xCA, 0x47, 0xEB,
	0x38, 0x7F, 0xD7, 0xE4, 0x3D, 0x62, 0xB0, 0x10
};

static int Encrypt(u32 *buf, int size)
{
	buf[0] = 4;
	buf[1] = buf[2] = 0;
	buf[3] = 0x100;
	buf[4] = size;

	/* Note: this encryption returns different data in each psp,
	   But it always returns the same in a specific psp (even if it has two nands) */
	if (sceUtilsBufferCopyWithRange(buf, size+0x14, buf, size+0x14, 5) != 0)
		return -1;

	return 0;
}

int _pspSignCheck(u8 *buf)
{
	u8 enc[0xD0+0x14];
	int iXOR, res;

	memcpy(enc+0x14, buf+0x110, 0x40);
	memcpy(enc+0x14+0x40, buf+0x80, 0x90);
	
	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[0x14+iXOR] ^= check_keys0[iXOR&0xF];
	}

	if ((res = Encrypt((u32 *)enc, 0xD0)) != 0)
	{
		Kprintf("Encrypt failed.\n");
		return -1;
	}

	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[0x14+iXOR] ^= check_keys1[iXOR&0xF];
	}

	memcpy(buf+0x80, enc+0x14, 0xD0);
	
	return 0;
}

int pspSignCheck(u8 *buf)
{
	int k1 = pspSdkSetK1(0);

	int res = sceKernelExtendKernelStack(0x2000, (void *)_pspSignCheck, buf);

	pspSdkSetK1(k1);
	return res;
}

int pspIsSignChecked(u8 *buf)
{
	int k1 = pspSdkSetK1(0);
	int i, res = 0;

	for (i = 0; i < 0x58; i++)
	{
		if (buf[0xD4+i] != 0)
		{
			res = 1;
			break;
		}
	}

	pspSdkSetK1(k1);
	return res;
}

////////// UnsignCheck //////////

static int Decrypt(u32 *buf, int size)
{
	buf[0] = 5;
	buf[1] = buf[2] = 0;
	buf[3] = 0x100;
	buf[4] = size;

	if (sceUtilsBufferCopyWithRange(buf, size+0x14, buf, size+0x14, 8) != 0)
		return -1;
	
	return 0;
}

int _pspUnsignCheck(u8 *buf)
{
	u8 enc[0xD0+0x14];
	int iXOR, res;
	int k1 = pspSdkSetK1(0);

	memcpy(enc+0x14, buf+0x80, 0xD0);

	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[iXOR+0x14] ^= check_keys1[iXOR&0xF]; 
	}

	if ((res = Decrypt((u32 *)enc, 0xD0)) < 0)
	{
		Kprintf("Decrypt failed.\n");
		pspSdkSetK1(k1);
		return res;
	}

	for (iXOR = 0; iXOR < 0xD0; iXOR++)
	{
		enc[iXOR] ^= check_keys0[iXOR&0xF];
	}

	memcpy(buf+0x80, enc+0x40, 0x90);
	memcpy(buf+0x110, enc, 0x40);

	pspSdkSetK1(k1);
	return 0;
}

int pspUnsignCheck(u8 *buf)
{
	int k1 = pspSdkSetK1(0);

	int res = sceKernelExtendKernelStack(0x2000, (void *)_pspUnsignCheck, buf);

	pspSdkSetK1(k1);
	return res;
}
////////// IPL Decryption /////////
int pspDecryptIPL1(const u8* pbIn, u8* pbOut, int cbIn)
{
    int k1 = pspSdkSetK1(0);
	
	// 0x1000 pages
    static u8 g_dataTmp[0x1040] __attribute__((aligned(0x40)));
    int cbOut = 0;
    while (cbIn >= 0x1000)
    {
	    memcpy(g_dataTmp+0x40, pbIn, 0x1000);
        pbIn += 0x1000;
        cbIn -= 0x1000;

        int ret = sceUtilsBufferCopyWithRange(g_dataTmp, 0x1040, g_dataTmp+0x40, 0x500, 1);
	    if (ret != 0)
        {
	        Kprintf("Decrypt IPL 1 failed 0x%08X, WTF!\n", ret);
            break; // stop, save what we can
        }
        memcpy(pbOut, g_dataTmp, 0x1000);
        pbOut += 0x1000;
        cbOut += 0x1000;
    }

	pspSdkSetK1(k1);
    return cbOut;
}

int pspLinearizeIPL2(const u8* pbIn, u8* pbOut, int cbIn)
{
    int k1 = pspSdkSetK1(0);
	
	u32 nextAddr = 0;
    int cbOut = 0;
    while (cbIn > 0)
    {
        u32* pl = (u32*)pbIn;
        u32 addr = pl[0];
        
		if (addr != nextAddr && nextAddr != 0)
		{
            pspSdkSetK1(k1);
			return 0;   // error
		}

        u32 count = pl[1];
        nextAddr = addr + count;
        memcpy(pbOut, pbIn+0x10, count);
        pbOut += count;
        cbOut += count;
        pbIn += 0x1000;
        cbIn -= 0x1000;
    }

	pspSdkSetK1(k1);
    return cbOut;
}

int pspDecryptIPL3(const u8* pbIn, u8* pbOut, int cbIn)
{
    int k1 = pspSdkSetK1(0);
	int ret;
	
	// all together now (pbIn/pbOut must be aligned)
    pbIn += 0x10000;
    cbIn -= 0x10000;
	memcpy(pbOut+0x40, pbIn, cbIn);
	
	ret = sceUtilsBufferCopyWithRange(pbOut, cbIn+0x40, pbOut+0x40, cbIn, 1);
	if (ret != 0)
    {
		Kprintf("mangle#1 returned $%x\n", ret);
		pspSdkSetK1(k1);
        return 0;
    }

	ret = *(u32*)&pbIn[0x70];  // true size
    
	pspSdkSetK1(k1);
	return ret;
}

////////// Decompression //////////

int pspIsCompressed(u8 *buf)
{
	int k1 = pspSdkSetK1(0);
	int res = 0;

	if (buf[0] == 0x1F && buf[1] == 0x8B)
		res = 1;
	else if (memcmp(buf, "2RLZ", 4) == 0)
		res = 1;

	pspSdkSetK1(k1);
	return res;
}


int decompress_kle(void *outbuf, u32 outcapacity, void *inbuf, void *unk)
{
	int (* decompress)(void *, u32, void *, void *);
	
	u32 *mod = (u32 *)sceKernelFindModuleByName("sceLoadExec");
	u32 text_addr = *(mod+27);
	decompress = (void *)(text_addr+0);

	return decompress(outbuf, outcapacity, inbuf, unk);
}

static int _pspDecompress(u32 *arg)
{
	int retsize;
	u8 *inbuf = (u8 *)arg[0];
	u8 *outbuf = (u8 *)arg[1];
	u32 outcapacity = arg[2];
	
	if (inbuf[0] == 0x1F && inbuf[1] == 0x8B) 
	{
		retsize = sceKernelGzipDecompress(outbuf, outcapacity, inbuf, 0);
	}
	else if (memcmp(inbuf, "2RLZ", 4) == 0) 
	{
		int (*lzrc)(void *outbuf, u32 outcapacity, void *inbuf, void *unk) = NULL;
		
		if (sceKernelDevkitVersion() >= 0x03080000)
		{
			
			u32 *mod = (u32 *)sceKernelFindModuleByName("sceNp9660_driver");
			if (!mod)
				return -1;

			u32 *code = (u32 *)mod[27];

			int i;
			
			for (i = 0; i < 0x8000; i++)
			{
				if (code[i] == 0x27bdf4f0 && code[i+20] == 0x34018080)
				{
					lzrc = (void *)&code[i];
					break;
				} 
			}

			if (i == 0x8000)
				return -2;
			//lzrc = lzrc_;
		}
		else
		{
			lzrc = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "UtilsForKernel", 0x7DD07271);

		}
		
		retsize = lzrc(outbuf, outcapacity, inbuf+4, NULL);
	}
	else if (memcmp(inbuf, "KL4E", 4) == 0)
	{
		retsize = UtilsForKernel_6C6887EE(outbuf, outcapacity, inbuf+4, NULL);
	}
	else if (memcmp(inbuf, "KL3E", 4) == 0) 
	{
		retsize = decompress_kle(outbuf, outcapacity, inbuf+4, NULL);
	}
	else
	{
		retsize = -1;
	}

	return retsize;
}

int pspDecompress(const u8 *inbuf, u8 *outbuf, u32 outcapacity)
{
	int k1 = pspSdkSetK1(0);
	u32 arg[3];

	arg[0] = (u32)inbuf;
	arg[1] = (u32)outbuf;
	arg[2] = outcapacity;
	
	int res = sceKernelExtendKernelStack(0x2000, (void *)_pspDecompress, arg);
	
	pspSdkSetK1(k1);
	return res;
}

////////// 3.70+ Table Insanity //////////

u8 key_C[56] = 
{
	0x07, 0x0F, 0x17, 0x1F, 0x27, 0x2F, 0x37, 0x3F, 0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, 0x36, 0x3E, 
	0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x35, 0x3D, 0x04, 0x0C, 0x14, 0x1C, 0x01, 0x09, 0x11, 0x19, 
	0x21, 0x29, 0x31, 0x39, 0x02, 0x0A, 0x12, 0x1A, 0x22, 0x2A, 0x32, 0x3A, 0x03, 0x0B, 0x13, 0x1B, 
	0x23, 0x2B, 0x33, 0x3B, 0x24, 0x2C, 0x34, 0x3C
};

u8 key_Z[48] = 
{
	0x32, 0x2F, 0x35, 0x28, 0x3F, 0x3B, 0x3D, 0x24, 0x31, 0x3A, 0x2B, 0x36, 0x29, 0x2D, 0x34, 0x3C, 
	0x26, 0x38, 0x30, 0x39, 0x25, 0x2C, 0x33, 0x3E, 0x17, 0x0C, 0x21, 0x1B, 0x11, 0x09, 0x22, 0x18, 
	0x0D, 0x13, 0x1F, 0x10, 0x14, 0x0F, 0x19, 0x08, 0x1E, 0x0B, 0x12, 0x16, 0x0E, 0x1C, 0x23, 0x20
};

u8 key_M[512] = 
{
	0x0D, 0x01, 0x02, 0x0F, 0x08, 0x0D, 0x04, 0x08, 0x06, 0x0A, 0x0F, 0x03, 0x0B, 0x07, 0x01, 0x04, 
	0x0A, 0x0C, 0x09, 0x05, 0x03, 0x06, 0x0E, 0x0B, 0x05, 0x00, 0x00, 0x0E, 0x0C, 0x09, 0x07, 0x02, 
	0x07, 0x02, 0x0B, 0x01, 0x04, 0x0E, 0x01, 0x07, 0x09, 0x04, 0x0C, 0x0A, 0x0E, 0x08, 0x02, 0x0D, 
	0x00, 0x0F, 0x06, 0x0C, 0x0A, 0x09, 0x0D, 0x00, 0x0F, 0x03, 0x03, 0x05, 0x05, 0x06, 0x08, 0x0B, 
	0x04, 0x0D, 0x0B, 0x00, 0x02, 0x0B, 0x0E, 0x07, 0x0F, 0x04, 0x00, 0x09, 0x08, 0x01, 0x0D, 0x0A, 
	0x03, 0x0E, 0x0C, 0x03, 0x09, 0x05, 0x07, 0x0C, 0x05, 0x02, 0x0A, 0x0F, 0x06, 0x08, 0x01, 0x06, 
	0x01, 0x06, 0x04, 0x0B, 0x0B, 0x0D, 0x0D, 0x08, 0x0C, 0x01, 0x03, 0x04, 0x07, 0x0A, 0x0E, 0x07, 
	0x0A, 0x09, 0x0F, 0x05, 0x06, 0x00, 0x08, 0x0F, 0x00, 0x0E, 0x05, 0x02, 0x09, 0x03, 0x02, 0x0C, 
	0x0C, 0x0A, 0x01, 0x0F, 0x0A, 0x04, 0x0F, 0x02, 0x09, 0x07, 0x02, 0x0C, 0x06, 0x09, 0x08, 0x05, 
	0x00, 0x06, 0x0D, 0x01, 0x03, 0x0D, 0x04, 0x0E, 0x0E, 0x00, 0x07, 0x0B, 0x05, 0x03, 0x0B, 0x08, 
	0x09, 0x04, 0x0E, 0x03, 0x0F, 0x02, 0x05, 0x0C, 0x02, 0x09, 0x08, 0x05, 0x0C, 0x0F, 0x03, 0x0A, 
	0x07, 0x0B, 0x00, 0x0E, 0x04, 0x01, 0x0A, 0x07, 0x01, 0x06, 0x0D, 0x00, 0x0B, 0x08, 0x06, 0x0D, 
	0x02, 0x0E, 0x0C, 0x0B, 0x04, 0x02, 0x01, 0x0C, 0x07, 0x04, 0x0A, 0x07, 0x0B, 0x0D, 0x06, 0x01, 
	0x08, 0x05, 0x05, 0x00, 0x03, 0x0F, 0x0F, 0x0A, 0x0D, 0x03, 0x00, 0x09, 0x0E, 0x08, 0x09, 0x06, 
	0x04, 0x0B, 0x02, 0x08, 0x01, 0x0C, 0x0B, 0x07, 0x0A, 0x01, 0x0D, 0x0E, 0x07, 0x02, 0x08, 0x0D, 
	0x0F, 0x06, 0x09, 0x0F, 0x0C, 0x00, 0x05, 0x09, 0x06, 0x0A, 0x03, 0x04, 0x00, 0x05, 0x0E, 0x03, 
	0x07, 0x0D, 0x0D, 0x08, 0x0E, 0x0B, 0x03, 0x05, 0x00, 0x06, 0x06, 0x0F, 0x09, 0x00, 0x0A, 0x03, 
	0x01, 0x04, 0x02, 0x07, 0x08, 0x02, 0x05, 0x0C, 0x0B, 0x01, 0x0C, 0x0A, 0x04, 0x0E, 0x0F, 0x09, 
	0x0A, 0x03, 0x06, 0x0F, 0x09, 0x00, 0x00, 0x06, 0x0C, 0x0A, 0x0B, 0x01, 0x07, 0x0D, 0x0D, 0x08, 
	0x0F, 0x09, 0x01, 0x04, 0x03, 0x05, 0x0E, 0x0B, 0x05, 0x0C, 0x02, 0x07, 0x08, 0x02, 0x04, 0x0E, 
	0x0A, 0x0D, 0x00, 0x07, 0x09, 0x00, 0x0E, 0x09, 0x06, 0x03, 0x03, 0x04, 0x0F, 0x06, 0x05, 0x0A, 
	0x01, 0x02, 0x0D, 0x08, 0x0C, 0x05, 0x07, 0x0E, 0x0B, 0x0C, 0x04, 0x0B, 0x02, 0x0F, 0x08, 0x01, 
	0x0D, 0x01, 0x06, 0x0A, 0x04, 0x0D, 0x09, 0x00, 0x08, 0x06, 0x0F, 0x09, 0x03, 0x08, 0x00, 0x07, 
	0x0B, 0x04, 0x01, 0x0F, 0x02, 0x0E, 0x0C, 0x03, 0x05, 0x0B, 0x0A, 0x05, 0x0E, 0x02, 0x07, 0x0C, 
	0x0F, 0x03, 0x01, 0x0D, 0x08, 0x04, 0x0E, 0x07, 0x06, 0x0F, 0x0B, 0x02, 0x03, 0x08, 0x04, 0x0E, 
	0x09, 0x0C, 0x07, 0x00, 0x02, 0x01, 0x0D, 0x0A, 0x0C, 0x06, 0x00, 0x09, 0x05, 0x0B, 0x0A, 0x05, 
	0x00, 0x0D, 0x0E, 0x08, 0x07, 0x0A, 0x0B, 0x01, 0x0A, 0x03, 0x04, 0x0F, 0x0D, 0x04, 0x01, 0x02, 
	0x05, 0x0B, 0x08, 0x06, 0x0C, 0x07, 0x06, 0x0C, 0x09, 0x00, 0x03, 0x05, 0x02, 0x0E, 0x0F, 0x09, 
	0x0E, 0x00, 0x04, 0x0F, 0x0D, 0x07, 0x01, 0x04, 0x02, 0x0E, 0x0F, 0x02, 0x0B, 0x0D, 0x08, 0x01, 
	0x03, 0x0A, 0x0A, 0x06, 0x06, 0x0C, 0x0C, 0x0B, 0x05, 0x09, 0x09, 0x05, 0x00, 0x03, 0x07, 0x08, 
	0x04, 0x0F, 0x01, 0x0C, 0x0E, 0x08, 0x08, 0x02, 0x0D, 0x04, 0x06, 0x09, 0x02, 0x01, 0x0B, 0x07, 
	0x0F, 0x05, 0x0C, 0x0B, 0x09, 0x03, 0x07, 0x0E, 0x03, 0x0A, 0x0A, 0x00, 0x05, 0x06, 0x00, 0x0D
};

u8 key_S[8] = 
{
	0x9E, 0xA4, 0x33, 0x81, 0x86, 0x0C, 0x52, 0x85
};

u8 key_S2[8] = 
{
	0xB2, 0xFE, 0xD9, 0x79, 0x8A, 0x02, 0xB1, 0x87
};

u8 key_S3[8] = 
{
	0x81, 0x08, 0xC1, 0xF2, 0x35, 0x98, 0x69, 0xB0 
};

u8 key_S4[8] =
{
	0x6D, 0x52, 0x1B, 0xA3, 0xC2, 0x36, 0xF9, 0x2B
};

u8 key_S5[8] =
{
	0xDB, 0x4E, 0x79, 0x41, 0xF5, 0x97, 0x30, 0xAD
};

u8 key_S6[8] =
{
	0xA6, 0x83, 0x0C, 0x2F, 0x63, 0x0B, 0x96, 0x29
};

u8 table_40[128] = 
{
	0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 
	0x00, 0x00, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 
	0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0x10, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 
	0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00
};


typedef struct
{
	u32 mklow;
	u32 mkhigh;
	u8 *key_S;
} TABLE_KEYS;

TABLE_KEYS table_keys[] =
{
	{ 0xb730e5c7, 0x95620b49, key_S  },
	{ 0x45c9dc95, 0x5a7b3d9d, key_S2 },
	{ 0x6F20585A, 0x4CCE495B, key_S3 },
	{ 0x620BF15A, 0x73F45262, key_S4 },
	{ 0xFD9D4498, 0xA664C8F8, key_S5 },
    { 0x3D6426E7, 0xD7BD7481, key_S6 }
};

static void GenerateSeed(void *out, int unused, u32 c1, u32 c2)
{
	u32 i, j, val1, val2, bit_insert, r1, r2, shr, base, wpar1, wpar2;
	u32 *out32 = (u32 *)out;
	
	bit_insert = 0x80000000;
	r1 = r2 = base = 0;

	for (i = 0; i < 0x38; i++)
	{
		val1 = key_C[i];
		
		if (val1 & 0x20)
		{
			val2 = (1 << val1);
			val1 = 0;			
		}
		else
		{
			val1 = (1 << val1);
			val2 = 0;
		}		

		val1 = (c1 & val1);
		val2 = (c2 & val2);
		
		if ((val1 | val2))
		{
			val1 = base;
			val2 = bit_insert;
		}
		else
		{
			val1 = 0;
			val2 = 0;
		}

		r1 = (r1 | val1);
		r2 = (r2 | val2);
		base = (base >> 1);
		base = (base & 0x7FFFFFFF) | ((bit_insert & 1) << 31); 
		bit_insert = (bit_insert >> 1);
	}

	wpar1 = (r2 >> 4);
	wpar2 = (r1 >> 8) & 0x00FFFFFF; 
	wpar2 = (wpar2  & 0xF0FFFFFF) | ((r2 & 0xF) << 24); 

	for (i = 0x10; i != 0; i--)
	{	
		r1 = 0x7efc;
		val1 = (wpar1 << 4);
		r1 = (r1 >> i);
		val2 = (wpar2 << 4);
		r1 = (r1 & 1);
		shr = (r1 ^ 0x1F);
		r1++;
		val1 = (val1 >> shr);
		val2 = (val2 >> shr);
		wpar1 = (wpar1 << r1);
		wpar2 = (wpar2 << r1);
		wpar1 = (wpar1 | val1);
		wpar2 = (wpar2 | val2);
		wpar1 = (wpar1 & 0x0FFFFFFF);
		wpar2 = (wpar2 & 0x0FFFFFFF);
		c2 = (wpar2 >> 24);
		c2 = (c2&0xF) | ((wpar1 & 0x0FFFFFFF) << 4);
		c1 = (wpar2 << 8);

		base = r1 = r2 = 0;
		bit_insert = 0x80000000;
		
		for (j = 0; j < 0x30; j++)
		{
			val1 = key_Z[j];
			
			if (val1 & 0x20)
			{
				val2 = (1 << val1);
				val1 = 0;				
			}
			else
			{
				val1 = (1 << val1);
				val2 = 0;
			}

			val1 = (c1 & val1);
			val2 = (c2 & val2);
			
			if ((val1 |val2))
			{
				val1 = base;
				val2 = bit_insert;
			}
			else
			{
				val1 = 0;
				val2 = 0;
			}

			r1 = (r1 | val1);
			r2 = (r2 | val2);
			base = (base >> 1);
			base = (base & 0x7FFFFFFF) | ((bit_insert & 1) << 31); 
			bit_insert = (bit_insert >> 1);
		}

		out32[0] = r1;
		out32[1] = r2;
		out32 += 2;
	}
}

static void Sce_Insanity_1(u32 x1, u32 x2, u32 *r1, u32 *r2)
{
	u32 temp;
	
	temp = ((x2 >> 4) ^ x1) & 0x0F0F0F0F;
	x2 = x2 ^ (temp << 4);
	x1 = x1 ^ temp;
	temp = ((x2 >> 16) ^ x1) & 0xFFFF;
	x1 = x1 ^ temp;
	x2 = x2 ^ (temp << 16);
	temp = ((x1 >> 2) ^ x2) & 0x33333333;
	x1 = x1 ^ (temp << 2);
	x2 = x2 ^ temp;
	temp = ((x1 >> 8) ^ x2) & 0x00FF00FF;
	x2 = (x2 ^ temp);
	x1 = x1 ^ (temp << 8);
	temp = ((x2 >> 1) ^ x1) & 0x55555555;
	*r2 = x2 ^ (temp << 1);
	*r1 = x1 ^ temp;
}

static void Sce_Insanity_2(u32 x1, u32 x2, u32 *r1, u32 *r2)
{
	u32 h1, h2, h3, h4;

	h1 = (x2 & 1);
	h2 = (x2 >> 27) & 0x1F;
	h3 = (x2 >> 23) & 0x3F;
	h4 = (x2 >> 19) & 0x3F;
	*r2 = (x2 >> 15) & 0x3F;
	*r2 = (*r2 & 0xFF7FFFFF) | ((h1 & 1) << 23);
	*r2 = (*r2 & 0xFF83FFFF) | ((h2 & 0x1F) << 18);
	*r2 = (*r2 & 0xFFFC0FFF) | ((h3 & 0x3F) << 12);
	*r2 = (*r2 & 0xFFFFF03F) | ((h4 & 0x3F) << 6);
	h1 = (x2 >> 11) & 0x3F;
	h2 = (x2 >> 7) & 0x3F;
	h3 = (x2 >> 3) & 0x3F;
	h4 = (x2 & 0x1F);
	*r1 = (x2 >> 31) & 1;
	*r1 = (*r1 & 0xFF03FFFF) | ((h1 & 0x3F) << 18);
	*r1 = (*r1 & 0xFFFC0FFF) | ((h2 & 0x3F) << 12);
	*r1 = (*r1 & 0xFFFFF03F) | ((h3 & 0x3F) << 6);
	*r1 = (*r1 & 0xFFFFFFC1) | ((h4 & 0x1F) << 1);
	
	*r2 = ((*r2 << 8) | ((*r1 >> 16) & 0xFF));
	*r1 = (*r1 << 16);
}

static void Sce_Insanity_3(u32 x1, u32 x2, u32 *r1, u32 *r2)
{
	int i;
	u32 shifter = 0, val;
	u8 *p = key_M;

	*r2 = 0;

	for (i = 0; i < 8; i++)
	{
		val = p[x1&0x3F];
		p += 0x40;
		x1 = (x1 >> 6);
		x1 = (x1 & 0x03FFFFFF) | ((x2 & 0x3F) << 26);
		x2 = (x2 >> 6);
		*r2 |= (val << shifter);
		shifter += 4;
	}

	*r1 = 0;
}

static void Sce_Insanity_4(u32 x1, u32 x2, u32 *r1, u32 *r2)
{
	int i;
	u32 *table = (u32 *)table_40;

	*r1 = 0;
	*r2 = 0;

	for (i = 0; i < 0x20; i++)
	{
		if (x2 & 1)
		{
			*r2 |= table[i];
		}		

		x2 = (x2 >> 1);		
	}
}

static void Sce_Insanity_5(u32 x1, u32 x2, u32 *r1, u32 *r2)
{
	u32 temp;

	temp = ((x2 >> 1) ^ x1) & 0x55555555;
	x1 = x1 ^ temp;
	x2 = x2 ^ (temp << 1);
	temp = ((x1 >> 8) ^ x2) & 0x00FF00FF;
	x1 = x1 ^ (temp << 8);
	x2 = x2 ^ temp;
	temp = ((x1 >> 2) ^ x2) & 0x33333333;
	x2 = x2 ^ temp;
	x1 = x1 ^ (temp << 2);
	temp = ((x2 >> 16) ^ x1) & 0xFFFF;
	x2 = x2 ^ (temp << 16);
	x1 = x1 ^ temp;
	temp = ((x2 >> 4) ^ x1) & 0x0F0F0F0F;
	*r1 = x1 ^ temp;
	*r2 = x2 ^ (temp << 4);
}

static void Sce_Paranoia(u8 *buf, u32 unused, u32 *p1, u32 *p2)
{
	u32 x1 = *p1;
	u32 x2 = *p2;
	u32 r1, r2, rot1, rot2, rot3, rot4, ro1, ro2, base;
	int i;
	u8 *p = buf+0x78;

	Sce_Insanity_1(x1, x2, &r1, &r2); 

	rot1 = 0;
	rot2 = 0;
	rot3 = r1;
	rot4 = r2;

	for (i = 0; i < 0x10; i++)
	{
		Sce_Insanity_2(rot1, rot3, &r1, &r2);

		ro1 = r1;
		ro2 = r2;
		r1 = *(u32 *)&p[0];
		r2 = *(u32 *)&p[4];
		p -= 8;
		base = (ro2 ^ r2);
		x1 = (base << 16);
		
		Sce_Insanity_3(((ro1 ^ r1) >> 16) | x1, base >> 16, &r1, &r2);
		Sce_Insanity_4(r1, r2, &r1, &r2);

		x1 = (r1 ^ rot2);
		x2 = (r2 ^ rot4);
		rot2 = rot1;
		rot4 = rot3;
		rot1 = x1;
		rot3 = x2;
	}

	Sce_Insanity_5(x1 | rot4, x2, p1, p2);
}

static void DecryptT(u8 *buf, int size, int mode)
{
	u8 m1[0x400];
	u8 m2[8];
	int i, j;

	memset(m1, 0, sizeof(m1));
	
	GenerateSeed(m1, 0x33333333, table_keys[mode].mklow, table_keys[mode].mkhigh);
	
	memcpy(m1+0x80, table_keys[mode].key_S, 8);

	for (i = 0; i < size; i++)
	{
		for (j = 0; j < 8; j++)
		{
			m2[7-j] = buf[j];
		}

		Sce_Paranoia(m1, 0x33333333, (u32 *)&m2[0], (u32 *)&m2[4]);

		for (j = 0; j < 8; j++)
		{
			m1[0x90+j] = m2[7-j] ^ m1[0x80+j];
			m1[0x80+j] = buf[j];
		}

		*(u32 *)&buf[0] = *(u32 *)&m1[0x90];
		*(u32 *)&buf[4] = *(u32 *)&m1[0x94];

		buf += 8;
	}	
}

static int pspDecryptTable_(u32 *arg)
{
	u8 *buf1 = (u8 *)arg[0];
	u8 *buf2 = (u8 *)arg[1];
	int size = (int)arg[2];
	int mode = (int)arg[3];
	int retsize;

	DecryptT(buf1, size >> 3, mode);

	if (buf1 != buf2) memcpy(buf2, buf1, size);
	
/*
	if ((*(u32 *)&buf2[0xD0]) == 0xD82310F0)
	{
		retsize = DecryptPRX2(buf2, buf1, size, 0xD82310F0);		
	}
	else if ((*(u32 *)&buf2[0xD0]) == 0xD8231EF0)
	{
		retsize = DecryptPRX2(buf2, buf1, size, 0xD8231EF0);		
	}
	else
*/
	retsize = pspDecryptPRX(buf2, buf1, size);
	if (retsize < 0)
	{	
		int res = sceMesgd_driver_102DC8AF(buf1, size, &retsize);
		if (res < 0)
		{
			retsize = -1;				
		}
	}

	return retsize;
}

int pspDecryptTable(u8 *buf1, u8 *buf2, int size, int mode)
{
	u32 arg[4];
	int retsize;
	int k1 = pspSdkSetK1(0);

	arg[0] = (u32)buf1;
	arg[1] = (u32)buf2;
	arg[2] = (u32)size;
	arg[3] = (u32)mode;

	retsize = sceKernelExtendKernelStack(0x2000, (void *)pspDecryptTable_, arg);

	pspSdkSetK1(k1);
	return retsize;
}
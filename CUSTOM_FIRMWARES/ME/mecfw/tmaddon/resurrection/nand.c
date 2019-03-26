#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <stdio.h>
#include <string.h>
#include <psppower.h>

#include "main.h"
#include "dcman.h"

#define NAND_DUMP "ms0:/nand-dump.bin"

extern int BIG_BUFFER_SIZE;
extern u8 *big_buffer;

int set_progress(int cur, int total)
{
	return ((100 * cur) / total);
}

int dump_nand_start()
{
	myDebugScreenClear();

	SceUID fd;
	u32 pagesize, ppb, totalblocks, extrasize, blocksize, totalpages;
	int nbfit;
	int i, j, res;
	int badblocks[28], nbb = 0;	
	
	dcGetNandInfo(&pagesize, &ppb, &totalblocks);

	extrasize = pagesize / 32;
	blocksize = (pagesize+extrasize) * ppb;
	totalpages = totalblocks*ppb;

	nbfit = BIG_BUFFER_SIZE / blocksize;

	fd = sceIoOpen(NAND_DUMP, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0)
	{
		printf("Error 0x%08X creating %s\n\n", fd, NAND_DUMP);

		sub_01544();
		myDebugScreenClear();

		return fd;
	}
	
	printf("Dumping nand... 0%%\n\n");

	for (i = 0; i < totalpages; )
	{
		u8 *p;
		memset(big_buffer, 0xff, nbfit*blocksize);

		p = big_buffer;
		
		for (j = 0; j < nbfit && i < totalpages; j++)
		{
			dcLockNand(0);
			
			if (dcReadNandBlock(i, p) == -1)
			{
				if (nbb < 28)
				{
					badblocks[nbb++] = i / ppb;
				}
			}

			dcUnlockNand();

			i += ppb;
			p += (528*ppb);

			scePowerTick(0);

			myDebugScreenSetXY(16, 0);
			printf("%d%%\n\n", set_progress(i, totalpages));
		}

		res = sceIoWrite(fd, big_buffer, j*blocksize);
		if (res != j*blocksize)
		{
			sceIoClose(fd);
			printf("Error 0x%08X writing %s\n\n", res, NAND_DUMP);

			sub_01544();
			myDebugScreenClear();

			return res;
		}
	}

	sceIoClose(fd);

	printf("Dump completed.\n");

	if (nbb > 0)
	{
		printf("The following bad blocks were found:\n\n");

		for (i = 0; i < nbb; i++)
		{
			printf("%d%s%s", badblocks[i], (i == (nbb - 1)) ? "." : ", ", (i && (i % 7) == 0) ? "\n" : "");
		}
	}

	sub_01544();
	myDebugScreenClear();

	return 0;
}

int restore_nand_start()
{
	u32 pagesize, ppb, totalblocks, extrasize, blocksize, totalpages, totalsize, ppn = 0;
	int nbfit, i, j, k, n, m, error = 0;
	u8 *user, *spare, *p, *q, *r;
	SceUID fd;
	SceIoStat stat;

	myDebugScreenClear();

	dcSetCancelMode(1);
	dcGetNandInfo(&pagesize, &ppb, &totalblocks);

	extrasize = pagesize / 32;
	blocksize = (pagesize+extrasize) * ppb;
	totalpages = totalblocks * ppb;
	totalsize = totalpages * (pagesize + extrasize);
	nbfit = BIG_BUFFER_SIZE / blocksize;

	user = malloc64(ppb * pagesize);
	spare = malloc64(ppb * extrasize);

	if (totalsize != (33 * 1024 * 1024) && totalsize != (66 * 1024 * 1024))
	{
		dcSetCancelMode(0);
		printf("Nand info not expected.\n");

		sub_01544();
		myDebugScreenClear();

		return -1;
	}

	fd = sceIoOpen(NAND_DUMP, PSP_O_RDONLY, 0);
	if (fd < 0)
	{
		dcSetCancelMode(0);
		printf("Error 0x%08X opening %s\n", fd, NAND_DUMP);

		sub_01544();
		myDebugScreenClear();

		return fd;
	}

	sceIoGetstat(NAND_DUMP, &stat);

	if (stat.st_size != totalsize)
	{
		sceIoClose(fd);
		dcSetCancelMode(0);
		printf("Error %s has not the correct size.\n", NAND_DUMP);

		sub_01544();
		myDebugScreenClear();

		return -1;
	}

	n = totalblocks / nbfit;
	
	if ((totalblocks % nbfit) != 0)
	{
		n++;
	}

	dcLockNand(1);
	
	printf("Restoring nand... 0%%\n\n");

	for (i = 0; i < n; i++)
	{
		sceIoRead(fd, big_buffer, nbfit * blocksize);
		p = big_buffer;

		if (i == (n-1))
		{
			m = totalblocks % nbfit;
			if (m == 0)
			{
				m = nbfit;
			}
		}
		else
		{
			m = nbfit;
		}

		for (j = 0; j < m; j++)
		{
			q = user;
			r = spare;
			
			for (k = 0; k < 32; k++)
			{
				memcpy(q, p, 512);
				memcpy(r, p + 512, 16);

				p += 528;
				q += 512;
				r += 16;
			}

			if (ppn >= totalpages)
			{
				dcUnlockNand();
				sceIoClose(fd);
				dcSetCancelMode(0);

				printf("Break\n");
				sub_01544();
				myDebugScreenClear();

				return -1;
			}
			
			if (1)
			{
				if (dcEraseNandBlock(ppn) >= 0)
				{
					if (dcWriteNandBlock(ppn, user, spare) < 0)
					{
						error++;
//						printf("Error writing block 0x%08X\n", ppn);
					}
				}
				else
				{
					error++;
//					printf("Error erasing block 0x%08X\n", ppn);
				}			
			}

			if (error > 100)
			{
				dcUnlockNand();
				sceIoClose(fd);
				dcSetCancelMode(0);

				printf("There are being too many write/erase errors.\n");
				sub_01544();
				myDebugScreenClear();

				return -1;
			}

			ppn += 32;
			myDebugScreenSetXY(18, 0);
			printf("%d%%\n\n", set_progress(ppn, totalpages));
		}
	}

	dcUnlockNand();
	sceIoClose(fd);

	dcSetCancelMode(0);
	
	printf("Restore completed.\n");

	p_mfree(user);
	p_mfree(spare);

	sub_01544();
	myDebugScreenClear();
	scePowerRequestStandby();

	return 0;
}

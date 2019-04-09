/*
	hostemu.c
	DOSFS Embedded FAT-Compatible Filesystem
	Host-Side Emulation Code	
	(C) 2005 Lewin A.R.W. Edwards (sysadm@zws.com)
*/

#include <stdio.h>
#include <stdlib.h>

#include "dosfs.h"
#include "hostemu.h"

//===================================================================
// Globals
//FILE *hostfile;			// references host-side image file

/*
	Attach emulation to a host-side disk image file
	Returns 0 OK, nonzero for any error
*/
/*
int DFS_HostAttach(char *imagefile)
{
	hostfile = fopen(imagefile, "r+b");
	if (hostfile == NULL)
		return -1;

	return 0;	// OK
}
*/

/*
	Read sector from image
	Returns 0 OK, nonzero for any error
*/
int DFS_HostReadSector(uint8_t *buffer, uint32_t sector, uint32_t count)
{
	unsigned char *read_buff = (unsigned char *)buffer;
	while(count --)
	{
		if( pspMsReadSector( sector , read_buff ) < 0) return -1;
		sector++;
		read_buff += 0x200;
	}

	/*
	if (fseek(hostfile, sector * SECTOR_SIZE, SEEK_SET))
		return -1;

	fread(buffer, SECTOR_SIZE, count, hostfile);
	*/
	
	return 0;
}

/*
	Write sector to image
	Returns 0 OK, nonzero for any error
*/
int DFS_HostWriteSector(uint8_t *buffer, uint32_t sector, uint32_t count)
{
	/*
	if (fseek(hostfile, sector * SECTOR_SIZE, SEEK_SET))
		return -1;

	fwrite(buffer, SECTOR_SIZE, count, hostfile);
	fflush(hostfile);
	*/
	return -1;
}

static uint8_t sector[SECTOR_SIZE];
static VOLINFO vi;
static FILEINFO fi;

int init_ms()
{
	uint32_t pstart, psize;
	uint8_t pactive , ptype;

	pspMsInit();


	pstart = DFS_GetPtnStart(0, sector, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) 
	{
		return -1;
	}

	if (DFS_GetVolInfo(0, sector, pstart, &vi)) 
	{
		return -1;
	}

	return 0;
}

int open_ms_file(const char *path )
{
	if (DFS_OpenFile(&vi,  path , DFS_READ, sector, &fi))
	{
//		printf("error opening file\n");
		return -1;
	}

	return 0;
}

int read_ms_file(void *buff , int max_size)
{
	uint32_t  i = 0;
	DFS_ReadFile(&fi, sector, buff , &i,  max_size );
	return (int)i;
}

int close_ms_file()
{
	return 0;
}
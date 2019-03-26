#ifndef __DAXREAD_H__
#define __DAXREAD_H__

#define DEV_NAME			"DAX"

#define DAX_FILE_SIGNATURE	0x00584144
#define DAX_FILE_VERSION_0	0x00000000
#define DAX_FILE_VERSION_1	0x00000001
#define DAX_FRAME_SIZE		0x00002000
#define DAX_INDEX_SIZE		0x00000200
#define MAX_NCAREAS			0x000000C0

typedef struct
{
	u32 signature;		// DAX format magic.
	u32 decompsize;		// Size of original non-compressed file.
	u32 version;		// DAX format version.
	u32 nNCareas;		// Number of non-compressed areas.
	u32 reserved[4];	// Reserved for future use. It must be zero.
} DAXHeader;

typedef struct
{
	u32 frame;	// First frame of the NC Area.
	u32 size;	// Size of the NC Area in frames.
} NCArea;

int DaxfileGetDiscSize(int umdfd);
int DaxfileReadSectors(void *buf, int nsectors, int lba);
int DaxOpen(int umdfd);

#endif

#ifndef _DAXFILE_H_
#define _DAXFILE_H_

#define DAXFORMAT_VERSION_0	0x00
#define DAXFORMAT_VERSION_1	0x01

#define DAXFILE_SIGNATURE	0x00584144
#define DAX_FRAME_SIZE		0x00002000
#define MAX_NCAREAS			0x000000C0

typedef struct
{
	unsigned int signature;		// DAX format magic ('D', 'A', 'X', '\0').
	unsigned int decompsize;	// Size of original non-compressed file.
	unsigned int version;		// DAX format version.
	unsigned int nNCareas;		// Number of non-compressed areas.
	unsigned int reserved[4];	// Reserved for future use. It must be zero.
} DAXHeader;

typedef struct
{
	unsigned int frame;			// First frame of the NC Area.
	unsigned int size;			// Size of the NC Area in frames.
} NCArea;

#endif

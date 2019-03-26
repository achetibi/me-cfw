/*
 * iso9660search.h
*/


#ifndef _ISO9660SEARCH_H_
#define _ISO9660SEARCH_H_

#include <stdio.h>

#define SECTOR_SIZE	2048

#define ISO_FLAG_FILE			0x00
#define ISO_FLAG_EXISTENCE		0x01
#define ISO_FLAG_DIRECTORY		0x02
#define ISO_FLAG_ASSOCIATED		0x04
#define ISO_FLAG_RECORD			0x08
#define ISO_FLAG_PROTECTION		0x10
#define ISO_FLAG_MULTIEXTENT	0x80

#ifdef WIN32
#pragma pack(1)
#endif

typedef struct 
#ifdef GCC_COMPILER
__attribute__((packed))
#endif
{
	unsigned char	DirectoryLength;
    unsigned char    XARlength;
	unsigned int		ExtentLocation;
	unsigned int		ExtentLocationBE;
	unsigned int		DataLength;
	unsigned int		DataLengthBE;
	unsigned char	DateTime[7];
	unsigned char	FileFlags;
	unsigned char    FileUnitSize;
	unsigned char	InterleaveGapSize;
	unsigned int		VolumeSequenceNum;
	unsigned char	FileNameLength;
	// Really it should be char FileName[FileNameLength]
	char	FileName[256]; 
} DirectoryRecord;

#define FILTER_BY_NAME			0x00000001
#define FILTER_BY_TYPE			0x00000002
#define FILTER_BY_SIZE			0x00000004
#define FILTER_BY_DATA			0x00000008

#define SIZE_EQUAL		0
#define SIZE_GREATER	1
#define SIZE_LOWER		2

typedef struct
{
	unsigned int		flags;
	char	*name;
	unsigned int		type;
	unsigned int		size;
	unsigned int		sizetype;
	unsigned char	*data;
	unsigned int		datasize;
} SearchFilter;

int isoSearchFiles(FILE *isofile, DirectoryRecord *output, int max, SearchFilter *filter);



#endif /* _ISO9660SEARCH_H_ */

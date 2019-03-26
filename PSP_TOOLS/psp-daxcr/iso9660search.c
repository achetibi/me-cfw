#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "iso9660search.h"

int isoScanFiles(FILE *isofile, DirectoryRecord *output, int max, SearchFilter *filter, int i)
{
	DirectoryRecord dr;
	int fpointer, add;
	int oldDirLen = 0;

	fpointer = ftell(isofile);

	while (1)
	{
		memset(&dr, 0, sizeof(DirectoryRecord));
		fread(&dr, 1, sizeof(DirectoryRecord), isofile);

		if (dr.DirectoryLength == 0)
		{
			if (SECTOR_SIZE - (fpointer % SECTOR_SIZE) <= oldDirLen)
			{
				fpointer += (SECTOR_SIZE - (fpointer % SECTOR_SIZE));
				fseek(isofile, fpointer, SEEK_SET);
				fread(&dr, 1, sizeof(DirectoryRecord), isofile);

				if (dr.DirectoryLength == 0)
					break;
			}

			else
			{
				break;
			}				
		}

		if (dr.FileName[0] != 0 && dr.FileName[0] != 1)
		{
			add = 1;
			
			if (filter)
			{
				if (filter->flags & FILTER_BY_NAME)
				{
					if (strstr(dr.FileName, filter->name) == NULL)
						add = 0;
				}

				if (filter->flags & FILTER_BY_TYPE)
				{
					if (filter->type == ISO_FLAG_FILE
						&& (dr.FileFlags & ISO_FLAG_DIRECTORY))
					{
						add = 0;
					}

					else if (filter->type == ISO_FLAG_DIRECTORY
							&& !(dr.FileFlags & ISO_FLAG_DIRECTORY))
					{
						add = 0;
					}					
				}

				if (filter->flags & FILTER_BY_SIZE)
				{
					if (filter->sizetype == SIZE_EQUAL &&
						dr.DataLength != filter->size)
					{
						add = 0;
					}

					else if (filter->sizetype == SIZE_GREATER &&
							dr.DataLength <= filter->size)
					{
						add = 0;
					}

					else if (filter->sizetype == SIZE_LOWER &&
							dr.DataLength >= filter->size)
					{
						add = 0;
					}
				}

				else if (filter->flags & FILTER_BY_DATA)
				{
					unsigned char *data;

					data = (unsigned char *)malloc(filter->datasize);
					fseek(isofile, dr.ExtentLocation * SECTOR_SIZE, SEEK_SET);
					fread(data, 1, filter->datasize, isofile);

					if (memcmp(data, filter->data, filter->datasize) != 0)
						add = 0;
				}
			}

			if (dr.FileFlags & ISO_FLAG_DIRECTORY)
			{
				fseek(isofile, dr.ExtentLocation * SECTOR_SIZE, SEEK_SET);
				i = isoScanFiles(isofile, output, max, filter, i);
			}

			if (i == max)
				break;

			if (add)
			{
				memcpy(&output[i], &dr, sizeof(DirectoryRecord));
				i++;				
			}
		}

		fpointer += dr.DirectoryLength;
		oldDirLen = dr.DirectoryLength;
		fseek(isofile, fpointer, SEEK_SET);
	}

	return i;
}

int isoSearchFiles(FILE *isofile, DirectoryRecord *output, int max, SearchFilter *filter)
{
	DirectoryRecord dr;

	fseek(isofile, 16 * SECTOR_SIZE + 156, SEEK_SET);
	fread(&dr, 1, sizeof(DirectoryRecord), isofile);
	fseek(isofile, dr.ExtentLocation * SECTOR_SIZE, SEEK_SET);

	return isoScanFiles(isofile, output, max, filter, 0);
}

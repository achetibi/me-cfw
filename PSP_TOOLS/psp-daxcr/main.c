/*
   DAX Creator 0.4 (Update by Rahim-US)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <zlib.h>

#ifndef WEHAVE_XY_FUNCTIONS
#ifdef WIN32
#include <windows.h>
#define WEHAVE_XY_FUNCTIONS

void GoToXY(int xpos, int ypos)
{
	COORD scrn;
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	scrn.X = xpos; 
	scrn.Y = ypos;

	SetConsoleCursorPosition(hOutput, scrn);
}

int GetX()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOutput, &csbi);

	return csbi.dwCursorPosition.X;	
}

int GetY()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(hOutput, &csbi);

	return csbi.dwCursorPosition.Y;	
}

#endif /* WIN32 */
#endif /* WEHAVE_XY_FUNCTIONS */

/* Include these here to not get errors if windows.h was included */
#include "daxfile.h"
#include "iso9660search.h"

/* Refresh speeds for the percentage (the bigger the slower) */
#define COMP_REFRESH_SPEED		600
#define DECOMP_REFRESH_SPEED	600
#define ANALYZE_REFRESH_SPEED	1000
  
/* Global vars */
FILE *isofile = NULL, *daxfile = NULL, *infofile = NULL;
unsigned int *offsets = NULL; 
unsigned short *lengths = NULL;
unsigned char combuf[DAX_FRAME_SIZE+1024], decbuf[DAX_FRAME_SIZE];
NCArea ncareas[MAX_NCAREAS];
int waitinput;


/* Functions */
int getfilesize(char *input)
{
	FILE *f = fopen(input, "rb");
	if (!f) {
		printf("Error : cannot open file %s\n", input);
		return -1;
	}
	
	fseek (f, 0, SEEK_END);
	int res = ftell(f);
	fclose(f);

	return res;
}

void release_and_exit(int status)
{
	if (isofile)
		fclose(isofile);

	if (daxfile)
		fclose(daxfile);

	if (infofile)
		fclose(infofile);

	if (offsets)
		free(offsets);

	if (lengths)
		free(lengths);

	if (waitinput)
	{
		printf("Press any key to continue");
		getchar();
	}

	exit(status);
}

int IsNCArea(unsigned int frame, int count)
{
	int i;

	for (i = 0; i < count; i++)
	{
		if (frame >= ncareas[i].frame &&
			((frame - ncareas[i].frame) < ncareas[i].size))
		{
			return 1;
		}
	}

	return 0;
}

int searchMediaFiles(char *input, int videofiles, int audiofiles)
{
	DirectoryRecord files[MAX_NCAREAS];
	SearchFilter filter;
	int i, n = 0, isosize;

	isosize = getfilesize(input);

	isofile = fopen(input, "rb");
	if (!isofile)
	{
		printf("Error: cannot open input file %s\n", input);
		release_and_exit(-1);
	}

	printf("Scanning media files...\n");

	if (videofiles)
	{
		memset(&filter, 0, sizeof(SearchFilter));
		filter.flags = FILTER_BY_TYPE | FILTER_BY_DATA;
		filter.type = ISO_FLAG_FILE;
		filter.data = (unsigned char *)"PSMF";
		filter.datasize = 4;
		n = isoSearchFiles(isofile, files, MAX_NCAREAS, &filter);		
	}

	if (audiofiles)
	{
		memset(&filter, 0, sizeof(SearchFilter));
		filter.flags = FILTER_BY_TYPE | FILTER_BY_DATA;
		filter.type = ISO_FLAG_FILE;
		filter.data = (unsigned char *)"RIFF";
		filter.datasize = 4;
		n += isoSearchFiles(isofile, files+n, MAX_NCAREAS-n, &filter);	
	}

	for (i = 0; i < n; i++)
	{
		unsigned int offset, frame, size;
		
		offset = files[i].ExtentLocation * SECTOR_SIZE;
		frame = offset / DAX_FRAME_SIZE;
		size = files[i].DataLength / DAX_FRAME_SIZE;
		
		if ((files[i].DataLength % DAX_FRAME_SIZE) != 0)
			size++;
		
		if ((isosize % DAX_FRAME_SIZE) != 0)
		{
			if (frame + size >= (unsigned int)((isosize / DAX_FRAME_SIZE) + 1))
				size--;
		}

		ncareas[i].frame = frame;
		ncareas[i].size = size;

		printf("Media file found: %s (offset=%08X,size=%dKB)\n", 
			files[i].FileName, offset, files[i].DataLength / 1024);
	}
	
	printf("%d media files found and forced to be NC areas.\n", n);
	
	fclose(isofile);
	
	return n;
}

int analyzeFile(char *input, char *infof, int complevel, int startIndex, unsigned int limit)
{
	unsigned int i, fsize, nframes, csize;
	int j, inNCarea;
	NCArea currNCA;

#ifdef WEHAVE_XY_FUNCTIONS
	int x, y;	
#endif

/*	
	Note: we don't check here if fsize is a multiple of 
	DAX_FRAME_SIZE, so here nframes refers to the number of
	frames of DAX_FRAME_SIZE size. This means that if fsize is not
	a multiple of DAX_FRAME_SIZE, the last frame of the file
	cannot be part of a NC area (anyways the last frame of an
	iso file is usually well compressed)
*/
	fsize = getfilesize(input);

	isofile = fopen(input, "rb");
	
	if (!isofile)
	{
		printf("Error: cannot open input file %s\n", input);
		release_and_exit(-1);
	}
	
	if (infof)
	{
		infofile = fopen(infof, "w");
		if (!infofile)
			printf("Warning: cannot create info file %s\n", infof);
	}

	else
	{
		infofile = NULL;
	}

	nframes = fsize / DAX_FRAME_SIZE;

	i = 0;
	j = startIndex;
	inNCarea = 0;
	currNCA.size = 0;
	currNCA.frame = 0;

	printf("Analyzing file...\n");

#ifdef WEHAVE_XY_FUNCTIONS
	x = 19;
	y = GetY() - 1;	
#endif
		
	for (i = 0; i < nframes; i++)
	{
		int res, read;

		read = fread(decbuf, 1, DAX_FRAME_SIZE, isofile);

		if (IsNCArea(i, startIndex))
			continue;

		if (read != DAX_FRAME_SIZE)
		{
			printf("Error analyzing input file %s.\n", input);
			release_and_exit(-1);
		}

		csize = DAX_FRAME_SIZE + 1024;

		res = compress2(combuf, (unsigned long *)&csize, decbuf, read, complevel);
			
		if (res != Z_OK)
		{
			printf("Error analyzing input file %s.\n", input);
			release_and_exit(-1);
		}

		if (csize >= limit)
		{
			if (inNCarea)
			{
				currNCA.size++;
			}

			else
			{
				currNCA.frame = i;
				currNCA.size = 1;
				inNCarea = 1;
			}
		}

		else
		{
			if (inNCarea)
			{
				if (j < MAX_NCAREAS)
				{
					ncareas[j].frame = currNCA.frame;
					ncareas[j].size = currNCA.size;
					j++;
				}

				else
				{
					unsigned int lowsize;
					int lowindex, k;

					lowsize = ncareas[startIndex].size;
					lowindex = startIndex;

					for (k = startIndex+1; k < MAX_NCAREAS; k++)
					{
						if (ncareas[k].size < lowsize)
						{
							lowsize = ncareas[k].size;
							lowindex = k;
						}
					}

					if (currNCA.size > lowsize)
					{
						ncareas[lowindex].frame = currNCA.frame;
						ncareas[lowindex].size = currNCA.size;
					}
				}

				inNCarea = 0;
			}
		}		
		
#ifdef WEHAVE_XY_FUNCTIONS

		if ((i % ANALYZE_REFRESH_SPEED) == 0)
		{
			if (((i * 100) / nframes) != 100)
				GoToXY(x, y);
			else
				GoToXY(x-1, y);
			
			printf("%d%%\n", (i * 100) / nframes);
		}
#endif
	}

#ifdef WEHAVE_XY_FUNCTIONS
	GoToXY(x-1, y);
	printf("100%%\n");
#endif

	if (infofile)
	{
		for (i = 0; i < (unsigned int)j; i++)
		{
			fprintf(infofile, "NCAREA #%d: framestart=%d," 
					"size=%d frames\n", i, ncareas[i].frame, ncareas[i].size);
		}

		fprintf(infofile, "\n\n");
	}

	printf("Numbers of NC Areas: %d\n", j);

	fclose(isofile);

	if (infofile)
		fclose(infofile);

	return j;
}

void compressFile(char *input, char *output, char *infof, int complevel, int nNCAreas)
{
	DAXHeader header;
	unsigned int nframes, read, written, csize;
	int i, j, fpointer;	
	char *report;
	
#ifdef WEHAVE_XY_FUNCTIONS
	int x, y;	
#endif

	memset(&header, 0, sizeof(header));
	header.signature = DAXFILE_SIGNATURE;
	header.decompsize = getfilesize(input);

	isofile = fopen(input, "rb");
	if (!isofile)
	{
		printf("Error: cannot open input file %s\n", input);
		release_and_exit(-1);
	}	

	daxfile = fopen(output, "wb");
	if (!daxfile)
	{
		printf("Error: cannot create output file %s\n", output);
		release_and_exit(-1);
	}

	if (infof)
	{
		infofile = fopen(infof, (nNCAreas) ? "a" : "w");
		if (!infofile)
			printf("Warning: cannot open/create info file %s\n", infof);
	}
	else
	{
		infofile = NULL;
	}

	if (header.decompsize < 0)
	{
		printf("Error getting file size.\n");
		release_and_exit(-1);
	}

	nframes = header.decompsize / DAX_FRAME_SIZE;

	if ((header.decompsize % DAX_FRAME_SIZE) != 0)
		nframes++;

	printf("N of frames: %d\n", nframes);

	if (nNCAreas == 0)
	{
		header.version = DAXFORMAT_VERSION_0;
	}

	else
	{
		header.version = DAXFORMAT_VERSION_1;
		header.nNCareas = nNCAreas;
	}

	offsets = (unsigned int *)malloc(nframes * 4);
	lengths = (unsigned short *)malloc(nframes * 2);

	fwrite(&header, 1, sizeof(header), daxfile);
	fwrite(offsets, 1, nframes * 4, daxfile);
	fwrite(lengths, 1, nframes * 2, daxfile);

	if (nNCAreas > 0)
		fwrite(ncareas, 1, sizeof(ncareas), daxfile); 	

	printf("Compressing...\n");

#ifdef WEHAVE_XY_FUNCTIONS
	x = 16;
	y = GetY() - 1;	
#endif

	i = 0;
	fpointer = ftell(daxfile);

	do
	{
		int res;

		read = fread(decbuf, 1, DAX_FRAME_SIZE, isofile);

		if (IsNCArea(i, nNCAreas))
		{
			fwrite(decbuf, 1, read, daxfile);
			csize = DAX_FRAME_SIZE;
			goto next_iteration;			
		}

		if (read > 0)
		{
			csize = DAX_FRAME_SIZE + 1024;
			res = compress2(combuf, (unsigned long *)&csize, decbuf, read, complevel);
			
			if (res == Z_BUF_ERROR)
			{
				printf("Not enough memory in compressed buffer!\n");
				release_and_exit(-1);
			}

			else if (res != Z_OK)
			{
				printf("Memory error while compressing.\n");
				release_and_exit(-1);
			}

			written = fwrite(combuf, 1, csize, daxfile);
			
			if (written != csize)
			{	
				printf("I/O write error.\n");
				release_and_exit(-1);
				break;
			}

			if (infofile)
			{
				fprintf(infofile, "Frame #%d: %d -> %d",
						i, DAX_FRAME_SIZE, csize);

				fprintf(infofile, "(%d%%)\n", 
							(csize * 100) / DAX_FRAME_SIZE);
			}	

next_iteration:
			offsets[i] = fpointer;
			lengths[i] = (unsigned short)csize;
			i++;
			fpointer += csize;

#ifdef WEHAVE_XY_FUNCTIONS

			if ((i % COMP_REFRESH_SPEED) == 0)
			{
				if (((i * 100) / nframes) != 100)
					GoToXY(x, y);
				else
					GoToXY(x-1, y);
				
				printf("%d%%\n", (i * 100) / nframes);
			}
#endif
		}
		
	} while (read == DAX_FRAME_SIZE);

#ifdef WEHAVE_XY_FUNCTIONS
	GoToXY(x-1, y);
	printf("100%%\n");
#endif

	printf("Total frames written: %d\n", i);
	
	printf("Writing tables...\n");
	fseek(daxfile, sizeof(header), SEEK_SET);

	if (ftell(daxfile) != sizeof(header))
	{
		release_and_exit(-1);
	}

	fwrite(offsets, 1, nframes * 4, daxfile);
	fwrite(lengths, 1, nframes * 2, daxfile);
	printf("Table size: %d+%d=%d\n", nframes*4, nframes*2, nframes*6);

	printf("Finished.\n\n");
	
	i = header.decompsize / 1024;
	j = getfilesize(output) / 1024;

	report = "Report: %d KB -> %d KB (%d%%)\n\n"; 
	printf(report, i, j, (100 * j) / i);
	if (infofile)
		fprintf(infofile, report);
	
	release_and_exit(0);
}

void decompressFile(char *input, char *output)
{
	DAXHeader header;
	unsigned int nframes, read, written, dsize;
	int i;

#ifdef WEHAVE_XY_FUNCTIONS
	int x, y;
#endif

	daxfile = fopen(input, "rb");

	if (!daxfile)
	{
		printf("Cannot open input file %s\n", input);
		release_and_exit(-1);
	}	

	isofile = fopen(output, "wb");

	if (!isofile)
	{
		printf("Cannot create output file %s\n", output);
		release_and_exit(-1);
	}

	read = fread(&header, 1, sizeof(header), daxfile);
		
	if (read != sizeof(header) || header.signature != DAXFILE_SIGNATURE)
	{
		printf("Input is not a valid DAX file.\n");
		release_and_exit(-1);
	}

	if (header.version > DAXFORMAT_VERSION_1)
	{
		printf("The version of the file is greater than supported.\n");
		release_and_exit(-1);
	}

	nframes = header.decompsize / DAX_FRAME_SIZE;

	if ((header.decompsize % DAX_FRAME_SIZE) != 0)
		nframes++;

	printf("N of frames: %d\n", nframes);

	offsets = (unsigned int *)malloc(nframes * 4);
	lengths = (unsigned short *)malloc(nframes * 2);

	read = fread(offsets, 1, nframes * 4, daxfile);
	if (read != nframes * 4)
	{
		printf("Corrupted input file.\n");
		release_and_exit(-1);
	}

	read = fread(lengths, 1, nframes * 2, daxfile);
	if (read != nframes * 2)
	{
		printf("Corrupted input file.\n");
		release_and_exit(-1);
	}
	
	if (header.nNCareas > 0)
	{
		read = fread(ncareas, 1, sizeof(ncareas), daxfile);
		if (read != sizeof(ncareas))
		{
			printf("Corrupted input file.\n");
			release_and_exit(-1);
		}
	}

	printf("Decompressing...\n");

#ifdef WEHAVE_XY_FUNCTIONS
	x = 18;
	y = GetY() - 1;	
#endif
	
	for (i = 0; i < (int)nframes; i++)
	{
		int res;

		fseek(daxfile, offsets[i], SEEK_SET);
		read = fread(combuf, 1, lengths[i], daxfile);

		if (IsNCArea(i, header.nNCareas))
		{
			fwrite(combuf, 1, DAX_FRAME_SIZE, isofile);
			continue;
		}

		if (read != lengths[i])
		{
			printf("Input seems to be corrupted.\n");
			release_and_exit(-1);
		}

		dsize = DAX_FRAME_SIZE;

		res = uncompress(decbuf, (unsigned long *)&dsize, combuf, lengths[i]);

		if (res != Z_OK)
		{
			printf("Error while decompressing (corrupt input?)\n");
			release_and_exit(-1);
		}

		written = fwrite(decbuf, 1, dsize, isofile);

		if (written != dsize)
		{
			printf("I/O error writing to output file.\n");
			release_and_exit(-1);
		}

#ifdef WEHAVE_XY_FUNCTIONS

		if ((i % DECOMP_REFRESH_SPEED) == 0)
		{
			if (((i * 100) / nframes) != 100)
				GoToXY(x, y);
			else
				GoToXY(x-1, y);
			
			printf("%d%%\n", (i * 100) / nframes);
		}
#endif

	}

#ifdef WEHAVE_XY_FUNCTIONS
	GoToXY(x-1, y);
	printf("100%%\n");
#endif
	
	printf("Finished.\n\n");	
	release_and_exit(0);
}

void help()
{
	printf("Usage: daxcr [OPTIONS] inputfile outputfile [infofile]\n\n");
	printf("infofile: optional text file to be created with information");
	printf(" about the compression of each frame.\n\n");
	printf("OPTIONS are: \n\n");
	printf("-d: decompress the inputfile to outputfile.\n");
	printf("    If not set, default operation is to compress inputfile to outputfile.\n");
	printf("-ln: where n is a number from [0-9]. Specifies the zlib compression level.\n");
	printf("    0=default compromise between speed and compression.\n");
	printf("    1=compress faster, but worse.\n");
	printf("    9=compress slower, but better.\n");
	printf("-an: where n is a number between 0-8192 or nothing.\n");
	printf("     It activate the search of not compressed (NC) areas.\n");
	printf("     n indicates the limit from which consider that a frame is not\n");
	printf("     enough compressed, and making it a candidate to form part of a NC area.\n");
	printf("     If n is not specified or if it's zero, then the value 8192 is used,\n");
	printf("     which is the same size that the frame size of the dax format, meaning\n");
	printf("     that only frames that not were compressed at all, not even a single byte,\n");
	printf("     will be candidates to be part of a NC area.\n");
	printf("-v: it searchs for video files in the input file and forced them\n");
	printf("    to be part of NC areas without analyzing if they could be compressed.\n");
	printf("    After that, the search of other NC areas will be done, even if\n");
	printf("    \"-a\" was not specified.\n");
	printf("-s: it searchs for audio files in the input file and forced them\n");
	printf("    to be part of NC areas without analyzing if they could be compressed.\n");
	printf("    After that, the search of other NC areas will be done, even if\n");
	printf("    \"-a\" was not specified.\n");
	printf("-w: if this option is set, the program will wait for user input\n");
	printf("    before exiting.\n");
	release_and_exit(-1);
}

#define NUMBERS "0123456789"

int main(int argc, char *argv[])
{
	char *inputfile, *outputfile, *infof;
	int i, decompress, complevel, useNCareas;
	int videofilesNCA, audiofilesNCA;
	unsigned int limit;

	printf("DAX Creator 0.4 (Update by Rahim-US)\n\n");

	if (argc < 3)
		help();

	decompress = 0;
	complevel = 0;
	useNCareas = 0;
	limit = 0;
	videofilesNCA = 0;
	audiofilesNCA = 0;
	waitinput = 0;
	
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] != '-')
			break;

		switch (argv[i][1])
		{
			case 'd': case 'D':
				if (strlen(argv[i]) > 2)
					help();
				
				decompress = 1;
			break;

			case 'l': case 'L':
				if (strlen(argv[i]) > 3	|| !strpbrk(argv[i], NUMBERS))
					help();

				complevel = atoi(argv[i]+2);
			break;	

			case 'a': case 'A':
				useNCareas = 1;
				limit = atoi(argv[i]+2);

				if (limit < 0 || limit > DAX_FRAME_SIZE)
					help();
			break;

			case 'v': case 'V':
				if (strlen(argv[i]) > 2)
					help();

				useNCareas = 1;
				videofilesNCA = 1;
			break;

			case 's': case 'S':
				if (strlen(argv[i]) > 2)
					help();

				useNCareas = 1;
				audiofilesNCA = 1;
			break;

			case 'w': case 'W':
				if (strlen(argv[i]) > 2)
					help();

				waitinput = 1;
			break;
			
			default:
				printf("Warning: unknown option \"%s\"\n", argv[i]);
		}
	}

	if ((argc - i) < 2 || (argc - i) >= 4)
		help();

	if (complevel == 0)
		complevel = Z_DEFAULT_COMPRESSION;

	if (limit == 0)
		limit = DAX_FRAME_SIZE;

	inputfile = argv[i++];
	outputfile = argv[i++];
	infof = (i < argc && !decompress) ? argv[i] : NULL;	

	if (!decompress)
	{
		int n;

		if (useNCareas)
		{
			n = 0;

			if (videofilesNCA || audiofilesNCA)
				n = searchMediaFiles(inputfile, videofilesNCA, audiofilesNCA);
			
			n = analyzeFile(inputfile, infof, complevel, n, limit);
		}

		else
			n = 0;

		compressFile(inputfile, outputfile, infof, complevel, n);
	}

	else
	{
		decompressFile(inputfile, outputfile);
	}

	return 0;
}

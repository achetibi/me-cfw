#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <zlib.h>

#include "main.h"

u8 sce_header[64] = {
	0x7E, 0x53, 0x43, 0x45, 0x40, 0x00, 0x00, 0x00, 0x5C, 0x79, 0x72, 0x3D, 0x6B, 0x68, 0x5A, 0x30,
	0x5C, 0x7D, 0x34, 0x67, 0x57, 0x59, 0x34, 0x78, 0x79, 0x8A, 0x4E, 0x3D, 0x47, 0x4B, 0x44, 0x44,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

tagInfo tags[] = {
	{ 0xDADADAF0, 0x55668D96 },
	{ 0x38020AF0, 0x28796DAA },
	{ 0x457B06F0, 0x8555ABF2 },
	{ 0xADF305F0, 0x7316308C }
};

int ReadFile(char *file, void *buf, int size) {
	FILE *f = fopen(file, "rb");

	if (!f) {
		return -1;
	}

	int rd = fread(buf, 1, size, f);
	fclose(f);

	return rd;
}

int WriteFile(char *file, void *buf, int size) {
	FILE *f = fopen(file, "wb");

	if (!f) {
		return -1;
	}

	int wt = fwrite(buf, 1, size, f);
	fclose(f);

	return wt;
}

char* GenerateRandomBinName(int size) {
	int i;
	const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char *buf = malloc((size + 1 + 4 + 1) * sizeof(char));

	srand(time(NULL));

	strncpy(buf, "_", 1);
	
	for (i = 1; i < size; i++) {
		buf[i] = chars[rand() % strlen(chars)];
	}

	strncpy(buf+size, ".bin", 4);
	buf[size + 4] = '\0';

	return buf;
}

void GenerateRandom(u8 *buf, int size) {
	int i;

	for (i = 0; i < size; i++) {
		buf[i] = (rand() & 0xFF);
	}
}

int GetDevkit(char *devkit) {
	int d = strtol(devkit, NULL, 10);
	if(devkit == NULL || d == 0 || d < 150 || d > 661) {
		return -1;
	}

	int a = (d / 100);
	int b = (d - (a * 100)) / 10;
	int c = ((d - (a * 100)) - (b * 10));

	return (0x00000010 + (a * 0x01000000) + (b * 0x00010000) + (c * 0x00000100));
}

void scramble(unsigned char *target, int size, unsigned char *seed, int seed_size) {
	int seed_offset = 0;
	unsigned char *end_buffer = target + size;

	while(target < end_buffer) {
		if( seed_offset >= seed_size ) {
			seed_offset = 0;
		}

		*target ^= seed[seed_offset];
		seed_offset++;
		target++;
	}
}

void scramble_simple(unsigned int *target, unsigned int *seed, int size ) {
	unsigned int *end_buffer = target + size/sizeof(int);

	while(target < end_buffer) {
		*target ^= *seed;
		seed++;
		target++;
	}
}

int PspPack(u8 *in, int size, u8 *out, int pbp, int use_sce_header, int encrypt, u32 tag, u32 devkit, int attr) {
	PSP_Header header;
	Elf32_Ehdr *elf_header;
	Elf32_Phdr *segments;
	Elf32_Shdr *sections;
	char *strtab;
	char *bin_name;
	PspModuleInfo *modinfo;
	int i;

	memset(&header, 0, sizeof(header));

	// Fill simple fields
	header.signature = 0x5053507E;
	header.comp_attribute = 1;
	header.version = 1;
	header.elf_size = size;

	header._80 = 0x80;

	elf_header = (Elf32_Ehdr *)in;
	if (elf_header->e_magic != 0x464C457F) {
		if (elf_header->e_magic == 0x5053507E || elf_header->e_magic == 0x4543537E) {
			printf("Already packed.\n");
			return 0;
		}

		printf("Not a PRX.\n");
		return -1;
	}

	// Fill fields from elf header
	header.entry = elf_header->e_entry;
	header.nsegments = (elf_header->e_phnum > 2) ? 2 : elf_header->e_phnum;

	if (header.nsegments == 0) {
		printf("There are no segments.\n");
		return -1;
	}

	// Fill segements
	segments = (Elf32_Phdr *)&in[elf_header->e_phoff];

	for (i = 0; i < header.nsegments; i++) {
		header.seg_align[i] = segments[i].p_align;
		header.seg_address[i] = segments[i].p_vaddr;
		header.seg_size[i] = segments[i].p_memsz;
	}

	// Fill module info fields
	header.modinfo_offset = segments[0].p_paddr;
	modinfo = (PspModuleInfo *)&in[header.modinfo_offset&0x7FFFFFFF];
	header.attribute = modinfo->attribute;
	header.module_ver_lo = modinfo->module_ver_lo;
	header.module_ver_hi = modinfo->module_ver_hi;
	strncpy(header.modname, modinfo->modname, 28);

	sections = (Elf32_Shdr *)&in[elf_header->e_shoff];
	strtab = (char *)(sections[elf_header->e_shstrndx].sh_offset + in);

	header.bss_size = segments[0].p_memsz - segments[0].p_filesz;

	for (i = 0; i < elf_header->e_shnum; i++) {
		if (strcmp(strtab+sections[i].sh_name, ".bss") == 0) {
			header.bss_size = sections[i].sh_size;
			break;
		}
	}

	if (i == elf_header->e_shnum) {
		printf("Warning: .bss section not found.\n");
		header.bss_size = 0;
		//return -1;
	}

	if (header.attribute & 0x1000) {
		if (pbp) {
			printf("No PBP Kernel support!\n");
			return -1;
		}

		u32 oe_tag;
		u32 psp_tag;

		// ME TAG
		if (tag == 0x4C94DAF0 || tag == 0x4C94ACF0 || tag == 0x4C9494F0) {
			oe_tag = 0xC6BA41D3;
			psp_tag = tag;
		}
			// PRO TAG
		else if (tag == 0xC01DB15D) {
			oe_tag = tag;
			psp_tag = tags[MODULE_KERNEL].psp_tag;
		}
			// M33 TAG
		else {
			oe_tag = tags[MODULE_KERNEL].oe_tag;
			psp_tag = tags[MODULE_KERNEL].psp_tag;
		}

		header.devkitversion = devkit;
		header.oe_tag = oe_tag;
		header.tag = psp_tag;
		header.decrypt_mode = 2;
	}

	else if (header.attribute & 0x800) {
		if (pbp) {
			header.decrypt_mode = 0x0C;
			//printf("No PBP VSH support!\n");
			//return -1;
		}
		else {
			header.decrypt_mode = 3;
		}

		header.devkitversion = devkit;
		header.oe_tag = tags[MODULE_VSH].oe_tag;
		header.tag = tags[MODULE_VSH].psp_tag;

		//printf("No VSH prx support.\n");
		//return -1;
	}
	else {
		if (pbp) {
			if (header.attribute != 0x200 && !attr) {
				char ans;

				printf("PBP modules attribute have to be 0x200, do you want it to be automatically changed? (y/n) ");

				do {
					scanf("%c", &ans);
				} while ((ans != 'n' || ans != 'N') && (ans != 'y' || ans != 'Y'));

				if (ans == 'n' || ans == 'N') {
					printf("Aborted.\n");
					return -1;
				}
			}

			header.attribute = modinfo->attribute = 0x200;
			header.oe_tag = tags[MODULE_PBP].oe_tag;
			header.tag = tags[MODULE_PBP].psp_tag;
			header.decrypt_mode = 0x0D;
		}
		else {
			u32 oe_tag;
			u32 psp_tag;

			if (tag == 0x457BDAF0) {
				oe_tag = 0x8555ABF2;
				psp_tag = tag;
			}
			else {
				oe_tag = tags[MODULE_USER].oe_tag;
				psp_tag = tags[MODULE_USER].psp_tag;
			}

			header.devkitversion = devkit;
			header.oe_tag = oe_tag;
			header.tag = psp_tag;
			header.decrypt_mode = 4;
		}
	}

	// Fill key data with random bytes
	GenerateRandom(header.key_data0, 0x30);
	GenerateRandom(header.key_data1, 0x10);
	GenerateRandom((u8 *)&header.key_data2, 4);
	GenerateRandom(header.key_data3, 0x1C);

	bin_name = GenerateRandomBinName(8);
	gzFile comp = gzopen(bin_name, "wb");
	if (!comp) {
		printf("Cannot create temp file.\n");
		return -1;
	}

	if (gzwrite(comp, in, size) != size) {
		printf("Error in compression.\n");
		return -1;
	}

	gzclose(comp);
	free(in);

	if (!(in = malloc(SIZE_BUFFER))) {
		printf("Cannot reallocate memory for temp buffer.\n");
		return -1;
	}

	header.comp_size = ReadFile(bin_name, in, SIZE_BUFFER);

	if (encrypt != 0) {
		GenerateRandom(header.scheck+0x38, 0x20);
		scramble_simple((u32*)(in) , (u32 *)header.key_data1, 0x10);
		scramble(in, header.comp_size, &(header.scheck[0x38]), 0x20);
	}

	if (use_sce_header) {
		memcpy(out, sce_header, 0x40);
		out += 0x40;
	}

	remove(bin_name);

	header.psp_size = header.comp_size + 0x150;

	memcpy(out, &header, 0x150);
	memcpy(out+0x150, in, header.comp_size);

	if (use_sce_header) {
		header.psp_size += 0x40;
	}

	return header.psp_size;
}

void do_help() {
	printf("Usage : psp-packer [options] [-i input.prx] [-o output.prx]\n\n" \
           "  -s  Use sce header for packed module.\n" \
           "  -u  Unpack gzipped prx module.\n" \
           "  -t  Kernel tag.\n" \
           "  -x  Encrypted packed module.\n" \
           "  -d  Use devkit version for packed module.\n" \
		   "  -y  Change EBOOT attrib without ask.\n" \
           "  -i  Input module to (un)pack.\n" \
           "  -o  Output (un)packed module if not set input will be overwritten.\n"\
		   "  -h  Print the help.\n\n");
}

int main(int argc, char *argv[]) {
	int i;
	u32 tag = 0;
	int sce_header = 0;
	int mode = 1;
	int attr = 0;
	int encrypt = 0;
	u32 devkit = 0;
	char *input = NULL;
	char *output = NULL;
	char *bin_name;

	if (argc == 1 || (argc < 2 || argc > 11)) {
		do_help();
		return -1;
	}
	else {
		for (i = 1; i < argc; i++) {
			if ((*argv[i]) == '-') {
				const char *p = argv[i]+1;

				while ((*p) != '\0') {
					char c = *(p++);
					if ((c=='s') || (c=='S')) {
						sce_header = 1;
					}

					if ((c=='u') || (c=='U')) {
						mode = 0;
					}

					if ((c=='t') || (c=='T')) {
						tag = strtoll(argv[i+1], NULL, 0);
					}

					if ((c=='x') || (c=='X')) {
						encrypt = 1;
					}

					if ((c=='y') || (c=='Y')) {
						attr = 1;
					}

					if ((c=='h') || (c=='H')) {
						do_help();
						return -1;
					}

					if ((c=='d') || (c=='D')) {
						devkit = GetDevkit(argv[i+1]);
					}

					if ((c=='i') || (c=='I')) {
						input = argv[i+1];
					}

					if ((c=='o') || (c=='O')) {
						output = argv[i+1];
					}
				}
			}
		}
	}

	if (input == NULL) {
		do_help();
		return -1;
	}

	if (output == NULL) {
		output = input;
	}

	srand(time(0));

	FILE *f = fopen(input, "rb");
	if (!f) {
		printf("Cannot open %s\n", input);
		return -1;
	}

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	u8 *src = malloc(SIZE_BUFFER);
	if (!src) {
		printf("Cannot allocate memory for input buffer.\n");
		return -1;
	}

	u8 *dst = malloc(SIZE_BUFFER);
	if (!dst) {
		free(src);
		printf("Cannot allocate memory for output buffer.\n");
		return -1;
	}

	fread(src, 1, size, f);
	fclose(f);
	u32 *magic = (u32 *)src;

	int res = 0;

	if(mode == 1) {
		if (magic[0] == 0x50425000) {
			int prxpos = magic[8];
			int prxsize = magic[9] - magic[8];
			int psarsize = size - magic[9];

			if (src[prxpos] == 0x7F && memcmp(src+prxpos+1, "ELF", 3) == 0) {
				size = PspPack(src + prxpos, prxsize, dst, 1, sce_header, 0, tag, devkit, attr);
				if (size < 0) {
					printf("Error in PspPack.\n");
					res = -1;
				}
				else if (size != 0) {
					magic[9] = prxpos + size;
					f = fopen(output, "wb");
					if (!f) {
						printf("Error opening %s for writing.\n", output);
						res = -1;
					}
					else {
						fwrite(src, 1, prxpos, f);
						fwrite(dst, 1, size, f);
						fwrite(src+prxpos+prxsize, 1, psarsize, f);
						fclose(f);
					}
				}
			}
			else if (memcmp(src + prxpos, "~PSP", 4) == 0 || memcmp(src + prxpos, "~SCE", 4) == 0) {
				printf("Already packed.\n");
			}
			else {
				printf("Unknown file type in DATA.PSP: 0x%08X\n", magic[0]);
				res = -1;
			}
		}
		else if (magic[0] == 0x464C457F) {
			size = PspPack(src, size, dst, 0, sce_header, encrypt, tag, devkit, attr);

			if (size < 0) {
				printf("Error in PspPack.\n");
				res = -1;
			}
			else if (size != 0) {
				if (WriteFile(output, dst, size) != size) {
					printf("Error writing file %s.\n", output);
					res = -1;
				}
			}
		}
		else if (magic[0] == 0x5053507E || magic[0] == 0x4543537E) {
			printf("Already packed.\n");
		}
		else {
			printf("Unknown file type: 0x%08X\n", magic[0]);
			res = -1;
		}
	}
	else {
		int size_psp = 0;
		int size_elf = 0;

		if(magic[0] == 0x50425000) {
			PBP_Header *header = (PBP_Header *)src;
			size_psp = header->off_psar - header->off_psp;
			memcpy(src, src + header->off_psp, size_psp);
			size_elf = *(u32 *)&src[0x28];
			magic = (u32 *)src;

			if(magic[0] == 0x5053507E) {
				size_psp = size_psp - 0x150;
				size_elf = *(u32 *)&src[0x28];
				memcpy(src, src + 0x150, size - 0x150);
			}
			else if(magic[0] == 0x4543537E) {
				size_psp = size_psp-0x190;
				size_elf = *(u32 *)&src[0x28 + 0x40];
				memcpy(src, src + 0x190, size - 0x190);
			}
		}
		else if(magic[0] == 0x5053507E) {
			PSP_Header header;
			size_psp = size;
			size_elf = *(u32 *)&src[0x28];
			
			memcpy(&header, src, 0x150);
			memcpy(src, src + 0x150, size - 0x150);

			if (*(u16 *)&src[0] != 0x8B1F) {
				// unscramble
				scramble_simple((u32*)(src) , (u32 *)&header.key_data1, 0x10);
				scramble(src, header.comp_size, &(header.scheck[0x38]), 0x20);
			}
		}
		else if(magic[0] == 0x4543537E) {
			size_psp = size;
			size_elf = *(u32 *)&src[0x28 + 0x40];
			memcpy(src, src + 0x190, size - 0x190);
		}
		else {
			printf("Unknown file type: 0x%08X\n", magic[0]);
			res = -1;
		}
		
		bin_name = GenerateRandomBinName(8);

		if (WriteFile(bin_name, src, size_psp) != size_psp) {
			printf("Error writing file %s.\n", bin_name);
			res = -1;
		}

		gzFile file = gzopen(bin_name, "rb");
		if (file == NULL) {
			printf("Cannot open compressed temp file.\n");
			return -1;
		}

		strcpy((char*)dst, "garbage");
		size = gzread(file, dst, SIZE_BUFFER);

		if (!size) {
			printf("Cannot decompress temp file.\n");
			return -1;
		}

		gzclose(file);
		remove(bin_name);

		if (WriteFile(output, dst, size_elf) < 0) {
			printf("Error writing file %s.\n", output);
			res = -1;
		}
	}

	free(src);
	free(dst);

	return res;
}
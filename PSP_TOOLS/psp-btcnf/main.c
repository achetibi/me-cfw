#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

#define SIZE_BUFFER 1*1024*1024

char *OutputName(char *input, char *ext) {
	char *output;
	char *dot;

	if (input == NULL || ext == NULL || (output = malloc (strlen (input) + 1)) == NULL) {
		return NULL;
	}

	strcpy (output, input);
	dot = strrchr (output, '.');

	if (dot != NULL) {
		*dot = '\0';
	}

	strcpy (output + strlen(output), ext);

	return output;
}

int WriteFile(char *file, void *buffer, int size) {
	FILE *fd = fopen(file, "wb");

	if (!fd) {
		return -1;
	}

	int write = fwrite(buffer, 1, size, fd);
	fclose(fd);

	return write;
}

int CountModules(char *input) {
	FILE *fd;
	char file_name[256];
	int dump, max_modules = 0;

	fd = fopen(input, "r");
	if(!fd) {
		printf("Cannot open %s\n", input);
		return -1;
	}

	while(!feof(fd)) {
		memset(file_name, 0, 256);
		fscanf(fd, "%s %05X", file_name, &dump);

		if(strlen(file_name) == 0) {
			break;
		}

		max_modules++;
	}

	fclose(fd);

	return max_modules;
}

int CheckSignature(char *input) {
	FILE *fd;
	u32 signature;

	fd = fopen(input, "r");
	if(!fd) {
		printf("Cannot open %s\n", input);
		return -1;
	}

	fread(&signature, sizeof(u32), 1, fd);

	// ~PSP - still encrypted
	if(signature == 0x5053507E) {
		printf("The input file is encrypted.\n");
		return -1;
	}
	// Old version
	else if(signature == 0x0F803000) {
		printf("This %s file is from an older firmware.\n", input);
		return -1;
	}
	// current version
	else if(signature == 0x0F803001) {
		return 0;
	}

	fclose(fd);

	return -1;
}

int ExtractBtCnf(char *input, char *output) {
	FILE *fd;
	BtcnfHeader head;
	ModeEntry *modes;
	ModuleEntry module;
	char file_name[256];
	unsigned char *buffer = NULL;
	int i, size = 0;
	char *type = NULL;
	char *section = NULL;

	printf("Checking signature... ");
	if(CheckSignature(input)) {
		return -1;
	}
	printf("done.\n");

	fd = fopen(input, "r");
	if(!fd) {
		printf("Cannot open file %s.\n", input);
		return -1;
	}

	fread(&head, sizeof(BtcnfHeader), 1, fd);
	modes = (ModeEntry *)malloc(head.nmodes * sizeof(ModeEntry));
	fseek(fd, head.modestart, SEEK_SET);

	printf("Loading module list... ");
	for(i = 0; i < head.nmodes; i++) {
		fread(&modes[i], sizeof(ModeEntry), 1, fd);
	}
	printf("done.\n");

	printf("Writing module list... ");
	buffer = malloc(SIZE_BUFFER);
	if (buffer == NULL) {
		printf("Cannot allocate memory buffer.\n");
		return -1;
	}

	for(i = 0; i < head.nmodules; i++) {
		fseek(fd, head.modulestart + 0x20 * i, SEEK_SET);
		fread(&module, sizeof(ModuleEntry), 1, fd);
		fseek(fd, module.stroffset + head.modnamestart, SEEK_SET);
		fgets(file_name, 255, fd);

		if(module.loadmode & TYPE_DOLLAR) {
			type = "$";
			memcpy(buffer + size, type, strlen(type));
			size += strlen(type);
		}

		if(module.loadmode & TYPE_PERCENT) {
			type = "%";
			memcpy(buffer + size, type, strlen(type));
			size += strlen(type);
		}

		if(module.loadmode & TYPE_TWOPERCENT) {
			type = "%%";
			memcpy(buffer + size, type, strlen(type));
			size += strlen(type);
		}

		memcpy(buffer + size, file_name, strlen(file_name));
		size += strlen(file_name);

		memcpy(buffer + size, " ", 1);
		size += 1;

		if(module.flags & SECT_VSH) {
			section = "V";
			memcpy(buffer + size, section, strlen(section));
			size += strlen(section);
		}

		if(module.flags & SECT_GAME) {
			section = "G";
			memcpy(buffer + size, section, strlen(section));
			size += strlen(section);
		}

		if(module.flags & SECT_UPDATER) {
			section = "U";
			memcpy(buffer + size, section, strlen(section));
			size += strlen(section);
		}

		if(module.flags & SECT_POPS) {
			section = "P";
			memcpy(buffer + size, section, strlen(section));
			size += strlen(section);
		}

		if(module.flags & SECT_LICENSE) {
			section = "L";
			memcpy(buffer + size, section, strlen(section));
			size += strlen(section);
		}

		if(module.flags & SECT_APP) {
			section = "A";
			memcpy(buffer + size, section, strlen(section));
			size += strlen(section);
		}

		if(module.flags & SECT_UMDEMU) {
			section = "E";
			memcpy(buffer + size, section, strlen(section));
			size += strlen(section);
		}

		if(module.flags & SECT_MLNAPP) {
			section = "M";
			memcpy(buffer + size, section, strlen(section));
			size += strlen(section);
		}

		section = "\n";
		memcpy(buffer + size, section, strlen(section));
		size += strlen(section);
	}

	if (WriteFile(output, buffer, size) < 0) {
		printf("Error writing file %s.\n", output);
		return -1;
	}

	printf("done.\n");
	free(modes);
	free(buffer);

	return 0;
}

int BuildBtCnf(char *input, char *output) {
	FILE *fd;
	char file_name[256];
	char section_flags[256];
	int i, j, max_modules, flags, load_mode;
	int modules_count = 0, module_list_length = 0, found = 0, size = 0;
	char **module_files;
	BtcnfHeader head;
	ModeEntry mode_entry;
	ModuleEntry **modules;
	unsigned char *buffer = NULL;

	printf("Counting modules... ");
	if ((max_modules = CountModules(input)) < 0) {
		printf("Modules count is invalid.");
		return -1;
	}
	printf("done.\n");

	printf("Loading module list... ");
	fd = fopen(input, "r");
	if(!fd) {
		printf("Cannot open file %s.\n", input);
		return -1;
	}

	modules = (ModuleEntry **)malloc(max_modules * sizeof(ModuleEntry *));
	module_files = (char **)malloc(max_modules * sizeof(char *));
	fseek(fd, 0, SEEK_SET);

	while(!feof(fd)) {
		memset(file_name, 0, 256);
		fscanf(fd, "%s %s\n", file_name, section_flags);

		if(strlen(file_name) == 0) {
			break;
		}

		flags = 0;

		for(j = 0; j < strlen(section_flags); j++) {
			switch(section_flags[j]) {
				case 'v':
				case 'V':
					flags |= SECT_VSH;
					continue;

				case 'g':
				case 'G':
					flags |= SECT_GAME;
					continue;

				case 'u':
				case 'U':
					flags |= SECT_UPDATER;
					continue;

				case 'p':
				case 'P':
					flags |= SECT_POPS;
					continue;

				case 'l':
				case 'L':
					flags |= SECT_LICENSE;
					continue;

				case 'a':
				case 'A':
					flags |= SECT_APP;
					continue;

				case 'e':
				case 'E':
					flags |= SECT_UMDEMU;
					continue;

				case 'm':
				case 'M':
					flags |= SECT_MLNAPP;
					continue;

				default:
					continue;
			}
		}

		max_modules++;

		if(!strncmp(file_name, "$%%", 3)) {
			load_mode = 0x8004;
			strncpy(file_name, file_name+3, strlen(file_name)-2);
		}
		else if(!strncmp(file_name, "$%", 2)) {
			load_mode = 0x8002;
			strncpy(file_name, file_name+2, strlen(file_name)-1);
		}
		else if(!strncmp(file_name, "$", 1)) {
			load_mode = 0x8001;
			strncpy(file_name, file_name+1, strlen(file_name));
		}
		else if(!strncmp(file_name, "%%", 2)) {
			load_mode = 0x0004;
			strncpy(file_name, file_name+2, strlen(file_name)-1);
		}
		else if(!strncmp(file_name, "%", 1)) {
			load_mode = 0x0002;
			strncpy(file_name, file_name+1, strlen(file_name));
		}
		else {
			load_mode = 0x0001;
		}

		for(i = 0; i < modules_count; i++) {
			if(!strcmp(module_files[i], file_name)) {
				found = 1;

				if(modules[i]->flags != flags) {
					found = 0;
				}
			}
		}

		if(!found) {
			modules[modules_count] = (ModuleEntry *)malloc(sizeof(ModuleEntry));
			modules[modules_count]->stroffset = module_list_length;
			modules[modules_count]->loadmode = load_mode;
			modules[modules_count]->reserved = 0;
			modules[modules_count]->reserved2 = 0;
			modules[modules_count]->flags = flags;
			memset(modules[modules_count]->hash, 0, 16);

			module_files[modules_count] = (char *)malloc(strlen(file_name)+1);
			memcpy(module_files[modules_count], file_name, strlen(file_name)+1);
			module_list_length += strlen(file_name)+1;
			modules_count++;
		}
	}
	printf("done.\n");

	printf("Writing output file... ");
	buffer = malloc(SIZE_BUFFER);
	if (buffer == NULL) {
		printf("Cannot allocate memory buffer.\n");
		return -1;
	}

	head.signature = 0x0F803001;
	head.devkit = 0x03090010;
	head.unknown[0] = 0x6B8B4567;
	head.unknown[1] = 0x327B23C6;
	head.modestart = 0x40;
	head.nmodes = 0x07;
	head.unknown2[0] = 0x643C9869;
	head.unknown2[1] = 0x66334873;
	head.modulestart = 0x120;
	head.nmodules = modules_count;
	head.unknown3[0] = 0x74B0DC51;
	head.unknown3[1] = 0x19495CFF;
	head.modnamestart = head.modulestart + 0x20 * head.nmodules;
	head.modnameend = head.modnamestart + module_list_length;
	head.unknown4[0] = 0x2AE8944A;
	head.unknown4[1] = 0x625558EC;

	memcpy(buffer, &head, sizeof(BtcnfHeader));
	size += sizeof(BtcnfHeader);

	mode_entry.maxsearch = modules_count;
	mode_entry.searchstart = 0;
	mode_entry.modeflag = SECT_VSH;
	mode_entry.mode2 = 2;
	memset(mode_entry.reserved, 0, 5 * sizeof(int));
	memcpy(buffer + size, &mode_entry, sizeof(ModeEntry));
	size += sizeof(ModeEntry);

	mode_entry.maxsearch = modules_count;
	mode_entry.searchstart = 0;
	mode_entry.modeflag = SECT_GAME;
	mode_entry.mode2 = 1;
	memset(mode_entry.reserved, 0, 5 * sizeof(int));
	memcpy(buffer + size, &mode_entry, sizeof(ModeEntry));
	size += sizeof(ModeEntry);

	mode_entry.maxsearch = modules_count;
	mode_entry.searchstart = 0;
	mode_entry.modeflag = SECT_UPDATER;
	mode_entry.mode2 = 3;
	memset(mode_entry.reserved, 0, 5 * sizeof(int));
	memcpy(buffer + size, &mode_entry, sizeof(ModeEntry));
	size += sizeof(ModeEntry);

	mode_entry.maxsearch = modules_count;
	mode_entry.searchstart = 0;
	mode_entry.modeflag = SECT_POPS;
	mode_entry.mode2 = 4;
	memset(mode_entry.reserved, 0, 5 * sizeof(int));
	memcpy(buffer + size, &mode_entry, sizeof(ModeEntry));
	size += sizeof(ModeEntry);

	mode_entry.maxsearch = modules_count;
	mode_entry.searchstart = 0;
	mode_entry.modeflag = SECT_APP;
	mode_entry.mode2 = 6;
	memset(mode_entry.reserved, 0, 5 * sizeof(int));
	memcpy(buffer + size, &mode_entry, sizeof(ModeEntry));
	size += sizeof(ModeEntry);

	mode_entry.maxsearch = modules_count;
	mode_entry.searchstart = 0;
	mode_entry.modeflag = SECT_UMDEMU;
	mode_entry.mode2 = 7;
	memset(mode_entry.reserved, 0, 5 * sizeof(int));
	memcpy(buffer + size, &mode_entry, sizeof(ModeEntry));
	size += sizeof(ModeEntry);

	mode_entry.maxsearch = modules_count;
	mode_entry.searchstart = 0;
	mode_entry.modeflag = SECT_MLNAPP;
	mode_entry.mode2 = 8;
	memset(mode_entry.reserved, 0, 5 * sizeof(int));
	memcpy(buffer + size, &mode_entry, sizeof(ModeEntry));
	size += sizeof(ModeEntry);

	for(i = 0; i < modules_count; i++) {
		if(modules[i] != NULL) {
			if(modules[i]->loadmode & 1) {
				memcpy(buffer + size, modules[i], sizeof(ModeEntry));
				size += sizeof(ModeEntry);
				free(modules[i]);
			}
		}
	}

	for(i = 0; i < modules_count; i++) {
		if(modules[i] != NULL) {
			if(modules[i]->loadmode & (2 | 4) ) {
				memcpy(buffer + size, modules[i], sizeof(ModeEntry));
				size += sizeof(ModeEntry);
				free(modules[i]);
			}
		}
	}

	for(i = 0; i < modules_count; i++) {
		if(modules[i] != NULL) {
			memcpy(buffer + size, module_files[i], strlen(module_files[i]));
			size += strlen(module_files[i]);

			memcpy(buffer + size, "\0"+1, 1);
			size += 1;

			free(module_files[i]);
		}
	}

	if (WriteFile(output, buffer, size) < 0) {
		printf("Error writing file %s.\n", output);
		return -1;
	}

	printf("done.\n");
	free(module_files);
	free(modules);
	free(buffer);

	return 0;
}

void PrintVersion() {
	printf("psp-btcnf Version 0.3 (Updated by Rahim-US)\n\n");
}

void PrintHelp() {
	printf("Usage : psp-btcnf [options] [input] [output]\n\n" \
           "  -b, --build     Build binary file from text file.\n" \
           "  -e, --extract   Extract binary file to text file.\n" \
           "  -v, --version   Print version information.\n" \
		   "  -h, --help      Print the help.\n\n");
}

int main(int argc, char *argv[]) {
	int i;
	char *p;
	char *input = NULL;
	char *output = NULL;
	int build = 0, extract = 0;

	if (argc == 1 || (argc < 2 || argc > 4)) {
		PrintHelp();
		return -1;
	}
	else {
		for (i = 1; i < argc; i++) {
			if (strncmp(argv[i], "--", 2) == 0) {
				p = argv[i]+2;

				if (strcmp(strlwr(p), "build") == 0) {
					build = 1;
					extract = 0;
					input = argv[i+1];
					output = argv[i+2] != NULL ? argv[i+2] : OutputName(input, ".bin");
				}
				
				if (strcmp(strlwr(p), "extract") == 0) {
					extract = 1;
					build = 0;
					input = argv[i+1];
					output = argv[i+2] != NULL ? argv[i+2] : OutputName(input, ".txt");
				}
				
				if (strcmp(strlwr(p), "version") == 0) {
					PrintVersion();
					return -1;
				}
				
				if (strcmp(strlwr(p), "help") == 0) {
					PrintHelp();
					return -1;
				}
			} else if (strncmp(argv[i], "-", 1) == 0) {
				p = argv[i]+1;

				if (strcmp(strlwr(p), "b") == 0) {
					build = 1;
					extract = 0;
					input = argv[i+1];
					output = argv[i+2] != NULL ? argv[i+2] : OutputName(input, ".bin");
				}
				
				if (strcmp(strlwr(p), "e") == 0) {
					extract = 1;
					build = 0;
					input = argv[i+1];
					output = argv[i+2] != NULL ? argv[i+2] : OutputName(input, ".txt");
				}
				
				if (strcmp(strlwr(p), "v") == 0) {
					PrintVersion();
					return -1;
				}
				
				if (strcmp(strlwr(p), "h") == 0) {
					PrintHelp();
					return -1;
				}
			}
		}
	}

	if (input == NULL || output == NULL) {
		PrintHelp();
		return -1;
	}

	if (build == 1) {
		if (BuildBtCnf(input, output) < 0) {
			return -1;
		}
	}
	
	if (extract == 1) {
		if (ExtractBtCnf(input, output) < 0) {
			return -1;
		}
	}

	return 0;
}
#include <pspsdk.h>
#include <pspkernel.h>
#include <psputility_sysparam.h>
#include <string.h>
#include <stdio.h>

#include "trans.h"

const char * g_messages_en[] = {
	/* Menu & Titles */
	"Universal Unbricker",
	"Custom Firmwares",
	"Official Firmwares",
	"Nand Operation",
	"Hardware Info",
	"Test M33",
	"Shutdown",
	"Reboot",
	"Install OFW %s",
	"Install CFW %s",
	"Dump Nand",
	"Restore Nand",
	"Check Nand",
	"Format Flash",
	"IdStorage Tools",
	"New IdStorage",
	"Change Region",
	"Fix MAC Address",
	"Backup IdStorage",
	"Restore IdStorage",

	/* Firmware install */
	"Flashing %s %0.2f",
	"%i.PBP not found at root.",
	"Invalid %i.PBP file size.",
	"This custom firmware is not supported by your PSP.",
	"Custom Firmware",
	"Official Firmware",
	"Verifying %i.PBP...",
	"Invalid or corrupted %i.PBP MD5 mismatch.",
	"Extracting updater modules...",
	"Cannot decrypt DATA.PSP",
	"Error extracting module %s",
	"Loading updater modules...",
	"Error loading updater modules.",
	"Unassign Flash...",
	"Formatting Flash%i...",
	"Flash%i format failed: 0x%08X",
	"Assigning Flashes...",
	"Flash%i assign failed: 0x%08X",
	"Creating directories...",
	"Error creating flash%i directories.",
	"Error opening %i.PBP: 0x%08X",
	"Loading PSAR...",
	"Error reading %i.PBP",
	"pspPSARInit failed.",
	"PSAR decode error: 0x%08X",
	"Cannot find path of %s",
	"Flashing: %s",
	"Error flashing %s",
	"Cannot decrypt PSP Slim IPL.",
	"Error in %s.",
	"Cannot decrypt common table.",
	"Cannot decrypt 0%ig table.",
	"Restoring registry...",
	"Install is complete.\nA shutdown is required. A normal battery is\nrequired to boot this firmware on this PSP.",
	"Install is complete.\nA shutdown or a reboot is required.",

	/* Hardware Informations */
	/* TAB 01 */
	"PSP Informations",
	"Model: %s",
	"Generation: PSP %02ig",
	"CPU Speed: %i Mhz",
	"BUS Speed: %i Mhz",
	"Free RAM: %i MB",
	"Name: %s",
	"Version: PSP-%04i",
	"Region: %s",
	"Original Firmware: %s",
	"Motherboard: %s",
	"WLAN Status: %s",
	"MAC: %02X:%02X:%02X:%02X:%02X:%02X",
	/* TAB 02 */
	"System Informations",
	"Tachyon: 0x%08X",
	"Baryon: 0x%08X",
	"Pommel: 0x%08X",
	"Kirk Version: %c%c%c%c",
	"Spock Version: %c%c%c%c",
	"EEPROM Access: %s",
	"Fuse Id: 0x%llX",
	"Fuse Config: 0x%08X",
	"Nand Seed: 0x%08X",
	"Nand Size: %i MB",
	"UMD Firmware: %s",
	"Time Machine: %s",
	/* TAB 03 */
	"Memory Stick Informations",
	"Boot Status: 0x%02X",
	"Signature: 0x%04X",
	"Partition Type: 0x%02X",
	"Starting Head: 0x%02X",
	"Last Head: 0x%02X",
	"MS size: %i MB",
	"Available IPL Space: %i KB",
	"Abs Sector: 0x%08X",
	"Total Sectors: 0x%08X",
	"Start Sec/Clu: 0x%04X",
	"Last Sec/Clu: 0x%04X",
	"Free MS Space: %i MB",
	/* TAB 04 */
	"Battery Informations",
	"Battery Status: %s",
	"Charge",
	"In Use",
	"Power Source: %s",
	"Battery",
	"External",
	"Battery Level: %i%%",
	"Hours Left: %i:%02i'",
	"Mode: %s",
	"Pandora",
	"AutoBoot",
	"Normal",
	"Unsupported",
	"Remain Capacity: %i mAh",
	"Total Capacity: %i mAh",
	"Battery Temp: %iºC",
	"Battery Voltage: %0.3fV",
	"Serial: %s",

	/* Nand Dump */
	"An existing nand dump was found on the Memory Stick.\n\nDo you want to overwrite it?",
	"Your Memory Stick has insufficient free space.",
	"Dumping nand...",
	"Error 0x%08X creating nand-dump_%08X.bin",
	"Dump is complete.\n",
	"The following bad blocks were found:\n\n",
	"No bad block were found.\n",

	/* Nand Restore */
	"nand-dump_%08X.bin not found at root.",
	"nand-dump_%08X.bin has not the correct size.",
	"Physical nand restore can be dangerous if your nand has more bad blocks than when you did the dump.\n\nDo you want to coninue?",
	"Restoring nand...",
	"Nand info not expected.",
	"Error 0x%08X opening nand-dump_%08X.bin",
	"Break!",
	"There are being too many write/erase errors.",
	"Nand restore is complete.\nA shutdown or a reboot is required.",

	/* Nand Verification*/
	"Verifying Nand",
	"Verification is complete.\n",

	/* Format Nand */
	"Flash%i size:",
	"Press %s to begin flash format.",
	"Loading modules...",
	"Error loading modules.",
	"Physical format...",
	"Error 0x%08X in physical format.",
	"Creating partitions...",
	"Error 0x%08X in fdisk.",
	"Logical format (Flash%i)...",
	"Format is complete.",

	/* Create IdStorage */
	"Select a region",
	"Get Real MAC",
	"The WLAN switch is off.",
	"Cannot get real mac address.",
	"Skip this step to generate a random MAC",
	"MAC: (random)",
	"(random)",
	"Press %s to create new IdStorage.",
	"This operation will delete your current IdStorage and create a new one.\nThis process should only be done when a part of the psp is malfunctioning (UMD, WLAN, etc).\n\nDo you want to continue?",
	"Formatting IdStorage...",
	"Error 0x%08X formatting IdStorage.",
	"Creating IdStorage index...",
	"Error 0x%08X creating key 0x%04X",
	"Ugly bug in idsRegenerationGetIndex code.",
	"Error 0x%08X creating keys 0x%04X/0x%04X",
	"Generating certificates and UMD keys...",
	"Error 0x%08X creating certificate and UMD keys.",
	"Error 0x%08X writing key 0x%04X.",
	"sceIdStorageFlush failed: 0x%08X",
	"Verifying certificates...",
	"Certificates verification failed: 0x%08X",
	"Creating other keys...",
	"IdStorage succesfully created.",

	/* Change Region */
	"After this operation, your current PSN original games will stop working until you download/buy them again, and your PSP will be identified by PS3 and PC as a new device.\n\nDo you want to continue?",
	"Changing region",
	"Error 0x%08X writing keys 0x%04X/0x%04X",
	"Region changed succesfully.",

	/* Fix Mac */
	"Restoring originl MAC",
	"Error 0x%08X writing to IdStorage.",
	"Original MAC written succesfully.",

	/* Dump IdStorage */
	"An existing IdStorage dump was found on the Memory Stick.\n\nDo you want to overwrite it?",
	"Dumping IdStorage...",
	"Error 0x%08X creating id-storage_%08X.bin",
	"Saving IdStorage Key: %04X",

	/* Restore IdStorage */
	"id-storage_%08X.bin not found at root.",
	"id-storage_%08X.bin has not the correct size.",
	"The IdStorage dump file found on the Memory Stick has been modified and may cause a brick.\n\nDo you want to continue?",
	"This process is capable of bricking the PSP if the dump did not originate from this console.\n\nDo you want to continue?",
	"Restoring IdStorage...",
	"Restoring IdStorage Key: %04X",
	"Restore is complete.",

	/* Regions */
	"Japan",
	"America",
	"Australia",
	"United Kingdom",
	"Europe",
	"Korea",
	"HongKong",
	"Taiwan",
	"Russia",
	"China",
	"Mexico",

	/* Others */
	"Unknown",
	"On",
	"Off",
	"Yes",
	"No"
};

#define READ_BUF_SIZE 1024
static char *read_buf = NULL;
static char *read_ptr = NULL;
static int read_cnt = 0;
static SceUID g_vpl_uid = -1;
int cur_language = 0;
const char **g_messages = g_messages_en;

void vpl_init(void)
{
	g_vpl_uid = sceKernelCreateVpl("VPL", 2, 0, 0x8000, NULL);
}

void vpl_finish(void)
{
	sceKernelDeleteVpl(g_vpl_uid);
}

void *vpl_alloc(int size)
{
	void *p;
	int ret;

	ret = sceKernelAllocateVpl(g_vpl_uid, size, &p, NULL);

	if(ret == 0)
		return p;

	return NULL;
}

char *vpl_strdup(const char *str)
{
	int len;
	char *p;

	len = strlen(str) + 1;
	p = vpl_alloc(len);

	if(p == NULL) {
		return p;
	}

	strcpy(p, str);

	return p;
}

void vpl_free(void *p)
{
	int ret;

	ret = sceKernelFreeVpl(g_vpl_uid, p);
}

void *vpl_realloc(void *ptr, size_t size)
{
	void *p;

	if(size == 0 && ptr != NULL) {
		vpl_free(ptr);

		return NULL;
	}

	p = vpl_alloc(size);

	if(p == NULL) {
		return p;
	}

	if(ptr == NULL) {
		memset(p, 0, size);
	} else {
		memcpy(p, ptr, size);
		vpl_free(ptr);
	}

	return p;
}

static int buf_read(SceUID fd, char *p)
{
	if(read_cnt <= 0) {
		read_cnt = sceIoRead(fd, read_buf, READ_BUF_SIZE);

		if(read_cnt < 0) {
			return read_cnt;
		}

		if(read_cnt == 0) {
			return read_cnt;
		}

		read_ptr = read_buf;
	}

	read_cnt--;
	*p = *read_ptr++;

	return 1;
}

static int read_lines(SceUID fd, char *lines, size_t linebuf_size)
{
	char *p;
	int ret;
	size_t re;

	if(linebuf_size == 0) {
		return -1;
	}

	p = lines;
	re = linebuf_size;

	while(re -- != 0) {
		ret = buf_read(fd, p);

		if(ret < 0) {
			break;
		}

		if(ret == 0) {
			if(p == lines) {
				ret = -1;
			}

			break;
		}

		if(*p == '\r') {
			continue;
		}

		if(*p == '\n') {
			break;
		}

		p++;
	}

	if(p < lines + linebuf_size) {
		*p = '\0';
	}

	return ret >= 0 ? p - lines : ret;
}

static void set_translate_table_item(char ***table, char *linebuf, int pos, int nr_trans)
{
	if(*table == NULL) {
		*table = (char**)vpl_alloc(sizeof(char*) * nr_trans);
		memset(*table, 0, sizeof(char*) * nr_trans);
	}

	(*table)[pos] = vpl_strdup(linebuf);
}

void replace_char(char *buf, char target, char ch)
{
	char *p;

	for (p = buf; *p != L'\0'; p++)
	{
		if (*p == target)
		{
			*p = ch;
		}
	}
}

int load_translate_table(char ***table, char *file, int nr_trans)
{
	SceUID fd;
	char linebuf[512];
	char *read_alloc_buf;
	int i;

	if (table == NULL) {
		return -1;
	}

	*table = NULL;

	linebuf[sizeof(linebuf) - 1] = '\0';
	fd = sceIoOpen(file, PSP_O_RDONLY, 0);

	if(fd < 0) {
		return fd;
	}
	else
	{
		u32 magic;
		sceIoRead(fd, &magic, sizeof(magic));
		sceIoLseek(fd, (magic &0xFFFFFF) == 0xBFBBEF ? 3 : 0, PSP_SEEK_SET);
	}

	read_alloc_buf = vpl_alloc(READ_BUF_SIZE + 64);

	if(read_alloc_buf == NULL) {
		sceIoClose(fd);
		return -1;
	}

	read_buf = (void*)(((u32)read_alloc_buf & (~(64-1))) + 64);
	i = 0;

	while(i < nr_trans) {
		if (read_lines(fd, linebuf, sizeof(linebuf) - 1) < 0) {
			break;
		}

		replace_char(linebuf, L'\\', L'\n');
		set_translate_table_item(table, linebuf, i, nr_trans);
		i++;
	}

	if (i < nr_trans) {
		sceIoClose(fd);
		vpl_free(read_alloc_buf);
		return -1;
	}

	sceIoClose(fd);
	vpl_free(read_alloc_buf);

	return 0;
}

void free_translate_table(char **table, int nr_trans)
{
	int i;

	if(table == NULL)
		return;

	for(i=0; i<nr_trans; ++i) {
		vpl_free(table[i]);
	}

	vpl_free(table);
}

void clear_language(void)
{
	if (g_messages != g_messages_en) {
		free_translate_table((char**)g_messages, MSG_END);
	}

	g_messages = g_messages_en;
}

static char ** apply_language(char *translate_file)
{
	char path[128];
	char **message = NULL;
	int ret;

	sprintf(path, "flash0:/lang/%s", translate_file);
	ret = load_translate_table(&message, path, MSG_END);

	if(ret >= 0) {
		return message;
	}

	return (char**) g_messages_en;
}

void select_language(void)
{
	int ret, value;

	ret = sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &value);
	if(ret != 0) {
		value = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
	}

	cur_language = value;
	clear_language();

	switch(value) {

		case PSP_SYSTEMPARAM_LANGUAGE_ENGLISH:
			g_messages = (const char**)apply_language("vunbricker_en.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_FRENCH:
			g_messages = (const char**)apply_language("vunbricker_fr.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_SPANISH:
			g_messages = (const char**)apply_language("vunbricker_es.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_GERMAN:
			g_messages = (const char**)apply_language("vunbricker_de.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_ITALIAN:
			g_messages = (const char**)apply_language("vunbricker_it.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_DUTCH:
			g_messages = (const char**)apply_language("vunbricker_nl.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_PORTUGUESE:
			g_messages = (const char**)apply_language("vunbricker_pt.txt");
			break;
		default:
			g_messages = g_messages_en;
			break;
	}

	if(g_messages == g_messages_en) {
		cur_language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
	}
}
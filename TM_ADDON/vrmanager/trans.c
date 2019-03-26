#include <pspsdk.h>
#include <pspkernel.h>
#include <psputility_sysparam.h>
#include <string.h>
#include <stdio.h>

#include "trans.h"

const char * g_messages_en[] = {
	"Create Magic Memory Stick",
	"Battery Operations",
	"Exit to XMB",
	"Convert Battery to Normal",
	"Convert Battery to Pandora",
	"Convert Battery to Autoboot",
	"Despertar del Cementerio %s",
	"Battery Operations",
	"Your current firmware version is unsupported. Please update to a newer custom firmware.",
	"Select your version",
	"Your Memory Stick has insufficient reserved sector space to install Time Machine IPL.",
	"%i.PBP not found at root.",
	"An existing installation was found on your Memory Stick and it will be owerwritten by this installation.\n\nDo you want to continue?",
	"Invalid %i.PBP file size.",
	"Creating Magic Memory Stick",
	"Verifying %i.PBP",
	"%i.PBP md5 mismatch.",
	"Creating directories...",
	"Error creating directories.",
	"Extracting updater modules...",
	"Enable to decrypt DATA.PSP",
	"Error extracting modules.",
	"Error 0x%08X opening %i.PBP",
	"Loading PSAR...",
	"Error reading %i.PBP",
	"pspPSARInit failed.",
	"PSAR decode error, pos: 0x%08X",
	"Cannot find path of %s",
	"Installing: %s",
	"Error installing %s",
	"Cannot decrypt common table.",
	"Cannot decrypt 0%ig table.",
	"Injecting IPL...",
	"Error 0x%08X opening msstor:",
	"Saving registery...",
	"Please hold the key(s) which you want to\nuse to boot from the Magic Memory Stick",
	"Magic Memory Stick has been created",
	"The PSP hardware does not support read or write from/to the Battery EEPROM.",
	"Convert Battery",
	"Normal Battery",
	"Pandora Battery",
	"Autoboot Battery",
	"Converting to %s",
	"The current battery serial is already set to 0x%04X%04X. There is no need to change the battery serial.",
	"Unable to write the serial to EEPROM.",
	"Battery Mode: %s\n\nBattery Serial: 0x%08X"
};

#define READ_BUF_SIZE 1024
static char *read_buf = NULL;
static char *read_ptr = NULL;
static int read_cnt = 0;
static SceUID g_vpl_uid = -1;
int cur_language = 0;
const char **g_messages = g_messages_en;
extern char *ebootpath;

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

	sprintf(path, "%s/lang/%s", ebootpath, translate_file);
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
			g_messages = (const char**)apply_language("vrmanager_en.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_FRENCH:
			g_messages = (const char**)apply_language("vrmanager_fr.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_SPANISH:
			g_messages = (const char**)apply_language("vrmanager_es.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_GERMAN:
			g_messages = (const char**)apply_language("vrmanager_de.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_ITALIAN:
			g_messages = (const char**)apply_language("vrmanager_it.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_DUTCH:
			g_messages = (const char**)apply_language("vrmanager_nl.txt");
			break;
		case PSP_SYSTEMPARAM_LANGUAGE_PORTUGUESE:
			g_messages = (const char**)apply_language("vrmanager_pt.txt");
			break;
		default:
			g_messages = g_messages_en;
			break;
	}

	if(g_messages == g_messages_en) {
		cur_language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
	}
}
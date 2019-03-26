#include <pspkernel.h>
#include <pspctrl.h>
#include <pspreg.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <psprtc.h>
#include "kubridge.h"
#include "kirk_engine.h"
#include "zlib.h"

#include "../ppatch_config.h"
#include "permanent.h"
#include "systemctrl_me.h"

typedef struct header_keys {
    u8 AES[16];
    u8 CMAC[16];
} header_keys;

/* VSH module can write F0 */
PSP_MODULE_INFO("Installer", 0x800, 1, 0);
PSP_HEAP_SIZE_KB(1*1024);

#define CTRL_DELAY   100000
#define CTRL_DEADZONE_DELAY 500000

#define printf pspDebugScreenPrintf

static u8 buf[64*1024] __attribute__((aligned(64)));
static u8 buf1[64*1024] __attribute__((aligned(64)));
static u8 buf2[64*1024] __attribute__((aligned(64)));

static u8 vshmain_pspheader[0x150] __attribute__((aligned(64)));
static u8 vshmain_kirkheader[0x110] __attribute__((aligned(64)));

static u8 in_buffer[512*1024] __attribute__((aligned(64)));
static u8 out_buffer[512*1024] __attribute__((aligned(64)));

static u8 kirk_raw[512*1024] __attribute__((aligned(64)));
static u8 kirk_enc[512*1024] __attribute__((aligned(64)));
static u8 elf[512*1024] __attribute__((aligned(64)));

extern int get_kirk_header(void *buf);
extern int kuKernelGetModel(void);

Module *modules = NULL;

void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	pspDebugScreenPrintf(msg);

	sceKernelDelayThread(milisecs*1000);
	sceKernelExitGame();
}

int ReassignFlash()
{
	int ret = 0;

	ret = sceIoUnassign("flash0:");
	if(ret < 0) {
		return ret;
	}
	else {
		ret = sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDWR, NULL, 0);
		if (ret < 0) {
			return ret;
		}
	}

	return ret;
}

int load_start_module(char *path)
{
	int ret;
	SceUID modid;
	int status;

	modid = kuKernelLoadModule(path, 0, NULL);

	status = 0;
	ret = sceKernelStartModule(modid, strlen(path) + 1, path, &status, NULL);

	return ret;
}

int compare_file(const char *src, const char *dst)
{
	SceUID fd = -1, fdd = -1;
	int ret, ret2;
	SceIoStat srcstat, dststat;

	ret = sceIoGetstat(src, &srcstat);
	
	if (ret != 0) {
		goto not_equal;
	}

	ret = sceIoGetstat(dst, &dststat);
	
	if (ret != 0) {
		goto not_equal;
	}

	if (dststat.st_size != srcstat.st_size) {
		goto not_equal;
	}

	ret = sceIoOpen(src, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto not_equal;
	}

	fd = ret;

	ret = sceIoOpen(dst, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto not_equal;
	}

	fdd = ret;
	ret = sizeof(buf1);
	ret = sceIoRead(fd, buf1, ret);

	while (ret > 0) {
		ret2 = sceIoRead(fdd, buf2, ret);

		if (ret2 != ret) {
			goto not_equal;
		}

		if (memcmp(buf1, buf2, ret)) {
			goto not_equal;
		}

		ret = sceIoRead(fd, buf1, ret);
	}

	if (ret < 0) {
		goto not_equal;
	}

	sceIoClose(fd);
	sceIoClose(fdd);

	return 0;

not_equal:
	if (fd >= 0)
		sceIoClose(fd);

	if (fdd >= 0)
		sceIoClose(fdd);

	return 1;
}

int copy_file(const char *src, const char *dst)
{
	SceUID fd = -1, fdw = -1;
	int ret;

	ret = sceIoOpen(src, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto error;
	}

	fd = ret;

	ret = sceIoOpen(dst, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (ret < 0) {
		goto error;
	}

	fdw = ret;
	ret = sizeof(buf);
	ret = sceIoRead(fd, buf, ret);

	while (ret > 0) {
		ret = sceIoWrite(fdw, buf, ret);

		if (ret < 0) {
			goto error;
		}

		ret = sceIoRead(fd, buf, ret);
	}

	if (ret < 0) {
		goto error;
	}

	sceIoClose(fd);
	sceIoClose(fdw);

	return 0;

error:
	sceIoClose(fd);
	sceIoClose(fdw);

	return ret;
}

int write_file(const char *path, unsigned char *buf, int size)
{
	SceUID fd;
	int ret;

	fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0) {
		goto error;
	}

	ret = sceIoWrite(fd, buf, size);

	if (ret < 0) {
		goto error;
	}

	sceIoClose(fd);

	return 0;
error:
	if (fd >= 0)
		sceIoClose(fd);

	return -1;
}

int load_binary(const char *path, void *buf, int offset, int size)
{
	SceUID fd;
	int ret;

	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		return fd;
	}

	sceIoLseek32(fd, offset, PSP_SEEK_SET);
	ret = sceIoRead(fd, buf, size);

	if(ret != size) {
		sceIoClose(fd);

		return -1;
	}

	sceIoClose(fd);
	
	return ret;
}

int get_vshmain_prx_header(void)
{
	int ret;

	ret = load_binary(VSHORIG, vshmain_pspheader, 0, sizeof(vshmain_pspheader));

	if(ret < 0)
		return ret;

	return 0;
}

int is_file_exist(const char *path)
{
	SceIoStat stat;
	int ret;

	ret = sceIoGetstat(path, &stat);

	return ret == 0 ? 1 : 0;
}

int is_already_installed(void)
{
	if(!is_file_exist(VSHORIG)) {
		return 0;
	}

	if(0 == compare_file(VSHMAIN, VSHORIG)) {
		return 0;
	}

	return 1;
}

static u32 g_last_btn = 0;
static u32 g_last_tick = 0;
static u32 g_deadzone_tick = 0;

u32 ctrl_read(void)
{
	SceCtrlData ctl;

	sceCtrlReadBufferPositive(&ctl, 1);

	if (ctl.Buttons == g_last_btn) {
		if (ctl.TimeStamp - g_deadzone_tick < CTRL_DEADZONE_DELAY) {
			return 0;
		}

		if (ctl.TimeStamp - g_last_tick < CTRL_DELAY) {
			return 0;
		}

		g_last_tick = ctl.TimeStamp;

		return g_last_btn;
	}

	g_last_btn = ctl.Buttons;
	g_deadzone_tick = g_last_tick = ctl.TimeStamp;

	return g_last_btn;
}

int confirm_install(void)
{
	u32 key;
	int ret = 0;

	printf("Press X to install.\n");
	printf("Press R to exit.\n\n");

	while ( 1 ) {
		key = ctrl_read();

		if(key & PSP_CTRL_RTRIGGER) {
			ret = 0;
			break;
		} else if(key & PSP_CTRL_CROSS) {
			ret = 1;
			break;
		}
	}

	return ret;
}

int confirm_uninstall(void)
{
	u32 key;
	int ret = 0;

	printf("Press X to uninstall.\n");
	printf("Press R to exit.\n\n");

	while ( 1 ) {
		key = ctrl_read();

		if(key & PSP_CTRL_RTRIGGER) {
			ret = 0;
			break;
		} else if(key & PSP_CTRL_CROSS) {
			ret = 1;
			break;
		}
	}

	return ret;
}

int load_elf(const char *path)
{
	SceIoStat stat;
	int ret;

	ret = sceIoGetstat(path, &stat);

	if(ret < 0) {
		return ret;
	}

	ret = load_binary(path, elf, 0, stat.st_size);

	return ret;
}

int gzip_compress(u8 *dst, const u8 *src, int size)
{
	int ret;
	z_stream strm;
	u8 *elf_compress;
	const int compress_max_size = 512*1024;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	elf_compress = malloc(compress_max_size);

	if(elf_compress == NULL) {
		return -1;
	}

	ret = deflateInit2(&strm, 9, Z_DEFLATED, 15+16, 8, Z_DEFAULT_STRATEGY);

	if(ret != Z_OK) {
//		printf("%s: compress error\n", __func__);
		free(elf_compress);
		
		return -2;
	}

	strm.avail_in = size;
	strm.next_in = (void*)src;
	strm.avail_out = compress_max_size;
	strm.next_out = elf_compress;

	ret = deflate(&strm, Z_FINISH);

	if(ret == Z_STREAM_ERROR) {
		deflateEnd(&strm);
//		printf("%s: compress error\n", __func__);
		free(elf_compress);

		return -3;
	}

	memcpy(dst, elf_compress, strm.total_out);
	deflateEnd(&strm);
	free(elf_compress);

	return 0;
}

int get_kirk_size(u8 *key_hdr)
{
	int krawSize = *(u32*)(key_hdr+0x70);

	if(krawSize % 0x10) {
		krawSize += 0x10 - (krawSize % 0x10); // 16 bytes aligned
	}

	krawSize += 0x110;

	return krawSize;
}

int get_elf_size(u8 *kirk_header)
{
//	return *(int*)(kirk_header+0xB8);
	return 391096;
}

int forge_vshmain(u8 *psp_header, u8 *kirk_header)
{
	header_keys keys;
	u8 rawkheaderBk[0x90];
	int ret;

	memset(in_buffer, 0, sizeof(in_buffer));
	memset(out_buffer, 0, sizeof(out_buffer));
	memset(kirk_raw, 0, sizeof(kirk_raw));
	memset(kirk_enc, 0, sizeof(kirk_enc));
	memset(elf, 0, sizeof(elf));

	kirk_init();

	int elfSize = load_elf(PRXPATH);

	if(elfSize < 0) {
//		printf("cannot load %s\n", PRXPATH);
		
		return elfSize;
	}

	int krawSize = get_kirk_size(kirk_header);

	if(elfSize > krawSize - 0x150) {
//		printf("PRX is too big!\n");

		return -1;
	}

	elfSize = get_elf_size(psp_header);
	ret = gzip_compress(elf, elf, elfSize);

	if(ret < 0) {
//		printf("Could not compress elf\n");
		
		return ret;
	}

	memcpy(kirk_raw, kirk_header, 0x110);
	memcpy(rawkheaderBk, kirk_raw, sizeof(rawkheaderBk));

	kirk_decrypt_keys((u8*)&keys, kirk_raw);
	memcpy(kirk_raw, &keys, sizeof(header_keys));
	memcpy(kirk_raw+0x110, elf, elfSize);

	if(kirk_CMD0(kirk_enc, kirk_raw, sizeof(kirk_enc), 0) != 0) {
//		printf("Could not encrypt elf\n");

		return -2;
	}

	memcpy(kirk_enc, rawkheaderBk, sizeof(rawkheaderBk));

	ret = kirk_forge(kirk_enc, sizeof(kirk_enc));

	if(ret != 0) {
//		printf("Could not forge cmac block 0x%08X\n", ret);

		return -3;
	}

	memcpy(out_buffer, psp_header, 0x150);
	memcpy(out_buffer+0x150, kirk_enc+0x110, krawSize-0x110);

	ret = write_file(VSHTEMP, out_buffer, (krawSize-0x110)+0x150);

	if(ret != 0) {
//		printf("Could not write file 0x%08X\n", ret);
		
		return ret;
	}

	return 0;
}

int get_vshmain_kirk_header(void)
{
	int ret;
	SceUID modid;

	ret = load_start_module(MODPATH);

	if(ret < 0)
		return ret;

	sceKernelDelayThread(100000);

	modid = kuKernelLoadModule(VSHMAIN, 0, NULL);

	if(modid < 0) {
		return modid;
	}

	ret = get_kirk_header(vshmain_kirkheader);

	if(ret < 0) {
		return ret;
	}

	sceKernelUnloadModule(modid);

	return 0;
}

int is_module_loadable(const char *path)
{
	SceUID modid;
	int result = 0;

	modid = kuKernelLoadModule(path, 0, NULL);

	if (modid >= 0) {
		sceKernelUnloadModule(modid);
		result = 1;
	}

	return result;
}

int final_check(void)
{
	int ret;

	ret = is_module_loadable(VSHTEMP);

	if(!ret) {
		ret = -1;
		goto exit;
	}

	ret = is_file_exist(VSHORIG);

	if(!ret) {
		ret = -2;
		goto exit;
	}

	ret = compare_file(VSHORIG, VSHTEMP);

	if(ret == 0) {
		ret = -3;
		goto exit;
	}

	ret = 0;
exit:
	return ret;
}

void uninstall(void)
{
	int ret, i;

	printf("Removing %s... ", VSHMAIN);
	ret = sceIoRemove(VSHMAIN);
	if(ret != 0) {
		printf("Failed\n");
	} else {
		printf("OK\n");
	}

	printf("Restoring %s... ", VSHMAIN);
	ret = copy_file(VSHORIG, VSHMAIN);
	if(ret != 0) {
		printf("Failed\n");
	} else {
		printf("OK\n");
	}

	printf("Removing %s... ", VSHORIG);
	ret = sceIoRemove(VSHORIG);
	if(ret != 0) {
		printf("Failed\n");
	} else {
		printf("OK\n");
	}

	for (i = 0; i < MODULES_COUNT; i++) {
		printf("Removing %s... ", modules[i].file_dest);
		ret = sceIoRemove(modules[i].file_dest);
		if(ret != 0) {
			printf("Failed\n");
		} else {
			printf("OK\n");
		}
	}

	for (i = 0; i < MODULES_COUNT; i++) {
		printf("Flashing %s... ", modules[i].file_dest);
		ret = write_file(modules[i].file_dest, modules[i].file_orig, modules[i].file_orig_size);
		if(ret != 0) {
			printf("Failed\n");
		} else {
			printf("OK\n");
		}
	}

	ErrorExit(5000, "\nUninstall completed. Exitting in 5 seconds...\n");
}

void install(void)
{
	int ret, i;

	printf("Removing %s... ", VSHMAIN);
	ret = sceIoRemove(VSHMAIN);
	if(ret != 0) {
		printf("Failed\n");
	} else {
		printf("OK\n");
	}

	printf("Creating %s... ", VSHMAIN);
	ret = copy_file(VSHTEMP, VSHMAIN);
	if(ret != 0) {
		printf("Failed\n");
	} else {
		printf("OK\n");
	}

	printf("Removing %s... ", VSHTEMP);
	ret = sceIoRemove(VSHTEMP);
	if(ret != 0) {
		printf("Failed\n");
	} else {
		printf("OK\n");
	}

	for (i = 0; i < MODULES_COUNT; i++) {
		printf("Removing %s... ", modules[i].file_dest);
		ret = sceIoRemove(modules[i].file_dest);
		if(ret != 0) {
			printf("Failed\n");
		} else {
			printf("OK\n");
		}
	}

	for (i = 0; i < MODULES_COUNT; i++) {
		printf("Flashing %s... ", modules[i].file_dest);
		ret = write_file(modules[i].file_dest, modules[i].file_perm, modules[i].file_perm_size);
		if(ret != 0) {
			printf("Failed\n");
		} else {
			printf("OK\n");
		}
	}
	
	ErrorExit(5000, "\nInstall completed. Exitting in 5 seconds...\n");
}

int main()
{
	pspDebugScreenInit();

	// check devkit and se version
	u32 fw_version = sceKernelDevkitVersion();
	if(fw_version != 0x06020010 || sctrlSEGetVersion() < 0x00010009) {
		ErrorExit(5000, "This program requires 6.20 LME-1.9 minimum.\n");
	}

	// get psp model
	int model = kuKernelGetModel();
	if(model == 0 ) {
		modules = (Module *)modules_01g;
	}
	else if(model == 1) {
		modules = (Module *)modules_02g;
	}
	else if(model == 2) {
		modules = (Module *)modules_03g;
	}
	else if(model == 3) {
		modules = (Module *)modules_04g;
	}
	else if(model == 4) {
		modules = (Module *)modules_05g;
	} else {
		ErrorExit(5000, "Unsupported PSP Model %02dg.\n", model+1);
	}

	int ret = ReassignFlash();
	if(ret < 0) {
		ErrorExit(5000, "Error reassign flash (0x%08X).\n", ret);
	}

	printf("6.20 LME Permanent Patch By Rahim-US\n\n");

	if(is_already_installed() != 0) {
		if(confirm_uninstall()) {
			uninstall();
		} else {
			ErrorExit(5000, "Uninstall canceled by user.\n");
		}
	}

	if (!confirm_install()) {
		ErrorExit(5000, "Install canceled by user.\n");
	}

	// backup vshmain
	printf("Backup %s... ", VSHMAIN);
	ret = copy_file(VSHMAIN, "ms0:/vshorig.prx");
	if(ret < 0) {
		ret = copy_file(VSHMAIN, "ef0:/vshorig.prx");
		if(ret < 0) {
			ErrorExit(5000, "Backup Failed.\n");
		} else {
			printf("OK\n");
		}
	} else {
		printf("OK\n");
	}

	ret = copy_file(VSHMAIN, VSHORIG);
	if(ret < 0) {
		ErrorExit(5000, "Creating %s failed.\n", VSHORIG);
	}

	ret = get_vshmain_prx_header();
	if(ret < 0) {
		ErrorExit(5000, "Get header from original vshmain failed.\n");
	}

	ret = get_vshmain_kirk_header();
	if(ret < 0) {
		ErrorExit(5000, "Get kirk header from original vshmain failed.\n");
	}

	ret = forge_vshmain(vshmain_pspheader, vshmain_kirkheader);
	if(ret < 0) {
		ErrorExit(5000, "Forge fake vshmain module failed.\n");
	}

	ret = final_check();
	if(ret < 0) {
		ErrorExit(5000, "Final check failed.\n");
	}

	install();
	
	return 0;
}

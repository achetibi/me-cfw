#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psputilsforkernel.h>
#include <pspopenpsid.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemctrl_me.h>

#include "main.h"

static int (*pspNpDrmRenameCheck)(char *fn);
static int (*pspNpDrmEdataSetupKey)(int fd);
static SceOff (*pspNpDrmEdataGetDataSize)(int fd);
static int (*pspKernelLoadModuleNpDrm)(char *fn, int flag, void *opt);

extern int (*sceKernelLoadModuleUser)(const char *path, int flags, SceKernelLMOption *option);

struct NoDrmFd {
	SceUID fd;
	int asyncKeySetup;
	struct NoDrmFd *next;
};

static struct NoDrmFd g_head, *g_tail = &g_head;

// found in KHBBS
static u8 g_drm_magic_1[8] = {
	0x00, 0x50, 0x53, 0x50, 0x45, 0x44, 0x41, 0x54 // PSPEDATA
};

// found in Valkyrie 3
static u8 g_drm_magic_2[4] = {
	0x00, 0x50, 0x47, 0x44 // PGD
};

static SceUID g_nodrm_sema = -1;

int check_memory(const void *addr, int size)
{
	const void *end_addr;
	u32 k1;

	k1 = pspSdkGetK1();
	end_addr = addr + size - 1;

	if((int)(((u32)end_addr | (u32)addr) & (k1 << 11)) < 0) {
		return 0;
	}

	return 1;
}

static int check_file_is_encrypted(int fd)
{
	int ret;
	u32 k1;
	char p[8 + 64], *buf;

	k1 = pspSdkSetK1(0);
	buf = (char*)((((u32)p) & ~(64-1)) + 64);
	ret = sceIoRead(fd, buf, 8);
	pspSdkSetK1(k1);
	sceIoLseek32(fd, 0, PSP_SEEK_SET);

	if (ret != 8)
		return 0;

	if (!memcmp(buf, g_drm_magic_1, sizeof(g_drm_magic_1))) {
		return 1;
	}

	if (!memcmp(buf, g_drm_magic_2, sizeof(g_drm_magic_2))) {
		return 1;
	}

	return 0;
}

static int check_file_is_encrypted_by_path(const char* path)
{
	SceUID fd;
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (fd >= 0) {
		ret = check_file_is_encrypted(fd);
	} else {
		ret = 1;
	}

	sceIoClose(fd);
	pspSdkSetK1(k1);

	return ret;
}

static inline void lock(void)
{
	u32 k1;

	k1 = pspSdkSetK1(0);
	sceKernelWaitSema(g_nodrm_sema, 1, 0);
	pspSdkSetK1(k1);
}

static inline void unlock(void)
{
	u32 k1;

	k1 = pspSdkSetK1(0);
	sceKernelSignalSema(g_nodrm_sema, 1);
	pspSdkSetK1(k1);
}

static inline int is_encrypted_flag(int flag)
{
	if (flag == 0x40004001 || flag == 0x40000001)
		return 1;

	return 0;
}

static struct NoDrmFd *find_nodrm_fd(SceUID fd)
{
	struct NoDrmFd *fds;

	if (fd < 0)
		return NULL;

	for(fds = g_head.next; fds != NULL; fds = fds->next) {
		if(fds->fd == fd)
			break;
	}

	return fds;
}

static int is_nodrm_fd(SceUID fd)
{
	return find_nodrm_fd(fd) != NULL ? 1 : 0;
}

static int add_nodrm_fd(SceUID fd)
{
	struct NoDrmFd *slot;

	if (fd < 0)
		return -1;

	lock();
	slot = (struct NoDrmFd*)sctrlKernelMalloc(sizeof(*slot));

	if(slot == NULL) {
		unlock();

		return -2;
	}

	slot->fd = fd;
	slot->asyncKeySetup = 0;

	g_tail->next = slot;
	g_tail = slot;
	slot->next = NULL;

	unlock();

	return slot->fd;
}

static int remove_nodrm_fd(SceUID fd)
{
	int ret;
	struct NoDrmFd *fds, *prev;

	lock();

	for(prev = &g_head, fds = g_head.next; fds != NULL; prev = fds, fds = fds->next) {
		if(fd == fds->fd) {
			break;
		}
	}

	if(fds != NULL) {
		prev->next = fds->next;

		if(g_tail == fds) {
			g_tail = prev;
		}

		sctrlKernelFree(fds);
		ret = 0;
	} else {
		ret = -1;
	}

	unlock();

	return ret;
}

int sceIoOpenPatched(const char *file, int flag, int mode)
{
	int fd;
	int encrypted;

	encrypted = is_encrypted_flag(flag);

	if (encrypted) {
		fd = sceIoOpen(file, PSP_O_RDONLY, mode);

		if (fd >= 0) {
			if (check_file_is_encrypted(fd)) {
				sceIoClose(fd);
			} else {
				int ret;

				ret = add_nodrm_fd(fd); 
				if (ret > 0) {
					fd = ret; 
				}

				goto exit;
			}
		}
	}

	fd = sceIoOpen(file, flag, mode);

exit:

	return fd;
}

int sceIoOpenAsyncPatched(const char *file, int flag, int mode)
{
	int fd;
	int is_plain = 0, encrypted;

	encrypted = is_encrypted_flag(flag);

	if (encrypted) {
		fd = sceIoOpen(file, PSP_O_RDONLY, mode);

		if (fd >= 0) {
			if (check_file_is_encrypted(fd)) {
				//printk("%s: %s is encrypted, nodrm disabled\n", __func__, file);
			} else {
				//printk("%s: %s is plain, nodrm enabled\n", __func__, file);
				is_plain = 1;
			}
		}

		sceIoClose(fd);
	}

	if(is_plain) {
		fd = sceIoOpenAsync(file, PSP_O_RDONLY, mode);

		if(fd >= 0) {
			int ret;

			ret = add_nodrm_fd(fd); 

			if (ret < 0) {
				//printk("%s: add_nodrm_fd -> %d\n", __func__, ret);
			} else {
				fd = ret; 
			}
		}
	} else {
		fd = sceIoOpenAsync(file, flag, mode);
	}

	return fd;
}

int sceIoClosePatched(SceUID fd)
{
	int ret;

	if (is_nodrm_fd(fd)) {
		ret = sceIoClose(fd);

		if (ret == 0) {
			ret = remove_nodrm_fd(fd);
			ret = 0;
		}
		
	} else {
		ret = sceIoClose(fd);
	}

	return ret;
}

int sceIoCloseAsyncPatched(SceUID fd)
{
	int ret;

	if (is_nodrm_fd(fd)) {
		ret = sceIoCloseAsync(fd);

		if (ret == 0) {
			ret = remove_nodrm_fd(fd);
			ret = 0;
		}
	} else {
		ret = sceIoCloseAsync(fd);
	}

	return ret;
}

int sceIoIoctlPatched(SceUID fd, unsigned int cmd, void * indata, int inlen, void * outdata, int outlen)
{
	int ret;

	if (cmd == 0x04100001 || cmd == 0x04100002) {
		if (is_nodrm_fd(fd)) {
			ret = 0;
			goto exit;
		}
	}

	ret = sceIoIoctl(fd, cmd, indata, inlen, outdata, outlen);

exit:

	return ret;
}

int sceIoIoctlAsyncPatched(SceUID fd, unsigned int cmd, void * indata, int inlen, void * outdata, int outlen)
{
	int ret;

	if (cmd == 0x04100001 || cmd == 0x04100002) {
		if (is_nodrm_fd(fd)) {
			ret = 0;
			find_nodrm_fd(fd)->asyncKeySetup = 1;
			goto exit;
		}
	} 

	ret = sceIoIoctlAsync(fd, cmd, indata, inlen, outdata, outlen);

exit:

	return ret;
}

int sceIoWaitAsyncCBPatched(SceUID fd, SceIores *result)
{
	int ret;

	ret = sceIoWaitAsyncCB(fd, result);

	{
		struct NoDrmFd *fds;

		fds = find_nodrm_fd(fd);

		if(fds != NULL && fds->asyncKeySetup) {
			fds->asyncKeySetup = 0;
			*result = 0LL;
			ret = 0;
		}
	}

	return ret;
}

int sceIoPollAsyncPatched( SceUID fd, SceIores *result)
{
	int ret;

	ret = sceIoPollAsync(fd, result);

	{
		struct NoDrmFd *fds;

		fds = find_nodrm_fd(fd);

		if(fds != NULL && fds->asyncKeySetup) {
			fds->asyncKeySetup = 0;
			*result = 0LL;
			ret = 0;
		}
	}

	return ret;
}

int sceNpDrmRenameCheckPatched(char *fn)
{
	int ret;

	// don't worry, it works without setting $k1 to 0
	if (sceKernelFindModuleByName("scePspNpDrm_Driver") == NULL) {
		ret = 0x8002013A;
		goto exit;
	}

	if(!check_memory(fn, strlen(fn) + 1)) {
		ret = 0x80550910;
		goto exit;
	}

	ret = check_file_is_encrypted_by_path(fn);

	if (ret == 0) {
		SceIoStat stat;
		u32 k1;
		
		k1 = pspSdkSetK1(0);
		ret = sceIoGetstat(fn, &stat) == 0 ? 0 : 0x80550901;
		pspSdkSetK1(k1);
	} else {
		if (pspNpDrmRenameCheck != NULL) {
			ret = (*pspNpDrmRenameCheck)(fn);
		} else {
			ret = 0x8002013A;
		}
	}

exit:

	return ret;
}

int sceNpDrmEdataSetupKeyPatched(SceUID fd)
{
	int ret;

	if (sceKernelFindModuleByName("scePspNpDrm_Driver") == NULL) {
		ret = 0x8002013A;
		goto exit;
	}

	if (is_nodrm_fd(fd)) {
		ret = 0;
	} else {
		if (pspNpDrmEdataSetupKey != NULL) {
			ret = (*pspNpDrmEdataSetupKey)(fd);
		} else {
			ret = 0x8002013A;
		}
	}

exit:

	return ret;	
}

SceOff sceNpDrmEdataGetDataSizePatched(SceUID fd)
{
	SceOff end;

	if (sceKernelFindModuleByName("scePspNpDrm_Driver") == NULL) {
		end = 0x8002013A;
		goto exit;
	}
	
	if (is_nodrm_fd(fd)) {
		SceOff off;
	   
		off = sceIoLseek(fd, 0, PSP_SEEK_CUR);
		end = sceIoLseek(fd, 0, PSP_SEEK_END);
		sceIoLseek(fd, off, PSP_SEEK_SET);
	} else {
		if (pspNpDrmEdataGetDataSize != NULL) {
			end = (*pspNpDrmEdataGetDataSize)(fd);
		} else {
			end = 0x8002013A;
		}
	}

exit:

	return end;
}

int sceKernelLoadModuleNpDrmPatched(char *fn, int flag, void *opt)
{
	int ret;

	ret = (*sceKernelLoadModuleUser)(fn, flag, opt);
	if (ret > 0) {
		return ret;
	}

	ret = (*pspKernelLoadModuleNpDrm)(fn, flag, opt);

	return ret;
}

static NoDrmHookEntry g_nodrm_hook_map[] = {
	{ "IoFileMgrForUser", 0x109F50BC, &sceIoOpenPatched },
	{ "IoFileMgrForUser", 0x89AA9906, &sceIoOpenAsyncPatched },
	{ "IoFileMgrForUser", 0x810C4BC3, &sceIoClosePatched },
	{ "IoFileMgrForUser", 0xFF5940B6, &sceIoCloseAsyncPatched },
	{ "IoFileMgrForUser", 0x63632449, &sceIoIoctlPatched },
	{ "IoFileMgrForUser", 0xE95A012B, &sceIoIoctlAsyncPatched },
	{ "IoFileMgrForUser", 0x35DBD746, &sceIoWaitAsyncCBPatched },
	{ "IoFileMgrForUser", 0x3251EA56, &sceIoPollAsyncPatched },
	{ "scePspNpDrm_user", 0x275987D1, &sceNpDrmRenameCheckPatched },
	{ "scePspNpDrm_user", 0x08D98894, &sceNpDrmEdataSetupKeyPatched },
	{ "scePspNpDrm_user", 0x219EF5CC, &sceNpDrmEdataGetDataSizePatched },
	{ "ModuleMgrForUser", 0xF2D8D1B4, &sceKernelLoadModuleNpDrmPatched },
};

void patch_drm_imports(SceModule *mod)
{
	u32 i;

	for(i = 0; i < NELEMS(g_nodrm_hook_map); ++i) {
		sctrlHookImportByNid(mod, g_nodrm_hook_map[i].libname, g_nodrm_hook_map[i].nid, g_nodrm_hook_map[i].addr, 1);
	}
}

int nodrm_get_npdrm_functions(void)
{
	pspNpDrmRenameCheck = (void *)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_user", 0x275987D1);
	pspNpDrmEdataSetupKey = (void *)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_user", 0x08D98894);
	pspNpDrmEdataGetDataSize = (void *)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_user", 0x219EF5CC);
	pspKernelLoadModuleNpDrm = (void *)sctrlHENFindFunction("sceModuleManager", "ModuleMgrForUser", 0xF2D8D1B4);

	if (pspNpDrmRenameCheck == NULL) return -1;
	if (pspNpDrmEdataSetupKey == NULL) return -2;
	if (pspNpDrmEdataGetDataSize == NULL) return -3;
	if (pspKernelLoadModuleNpDrm == NULL) return -4;

	return 0;
}

void fix_weak_imports(void)
{
	int i, count;
	int k1 = pspSdkGetK1();
	pspSdkSetK1(0);

	SceUID *modids = sctrlKernelMalloc(MAX_MODULE_NUMBER * sizeof(SceUID));
	if(modids == NULL) {
		goto exit;
	}

	memset(modids, 0, MAX_MODULE_NUMBER * sizeof(SceUID));

	int ret = sceKernelGetModuleIdList(modids, MAX_MODULE_NUMBER * sizeof(SceUID), &count);
	if (ret < 0) {
		sctrlKernelFree(modids);
		goto exit;
	}

	for(i=0; i<count; ++i) {
		SceModule2 *pMod;

		pMod = (SceModule2*)sceKernelFindModuleByUID(modids[i]);

		if (pMod != NULL && (pMod->attribute & 0x1000) == 0) {
			patch_drm_imports((SceModule*)pMod);
		}
	}

	sctrlKernelFree(modids);

exit:
	pspSdkSetK1(k1);
}

int NoDRM_Init(void)
{
	g_nodrm_sema = sceKernelCreateSema("", 0, 1, 1, NULL);
	g_head.next = NULL;
	g_tail = &g_head;

	return 0;
}
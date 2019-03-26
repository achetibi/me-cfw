#ifndef __IO_PATCH_H__
#define __IO_PATCH_H__


SceUID sceIoDopenPatched(const char *dirname);
int sceIoDreadPatched(SceUID fd, SceIoDirent *dir);
int sceIoDclosePatched(SceUID fd);
SceUID sceIoOpenPatched(const char *file, int flags, SceMode mode);
int sceIoClosePatched(SceUID fd);
int sceIoReadPatched(SceUID fd, void *data, SceSize size);
SceOff sceIoLseekPatched(SceUID fd, SceOff offset, int whence);
int sceIoLseek32Patched(SceUID fd, int offset, int whence);
int sceIoGetstatPatched(const char *file, SceIoStat *stat);
int sceIoChstatPatched(const char *file, SceIoStat *stat, int bits);
int sceIoRemovePatched(const char *file);
int sceIoRmdirPatched(const char *path);
int sceIoMkdirPatched(const char *dir, SceMode mode);

void IoPatches(void);

#endif

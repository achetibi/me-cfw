#ifndef __COMMON_H__
#define __COMMON_H__

int AssignFlash();
int UnassignFlash();
int Rand(int min, int max);
int FileExists(const char *filepath, ...);
int DirExists(char *dirpath, ...);
int GetFileSize(const char *filepath, ...);
int ReadFile(char *file, int seek, u8 *buf, int size);
int WriteFile(char *file, u8 *buf, int size);
int pspGetRegistryValue(const char *dir, const char *name, void *buf, int bufsize);
int UTF82Unicode(char *src, char *dst);
char *GetString(char *buf, u16 str);
void ErrorReturn(int handler, int pg_ctrl, char *fmt, ...);

#endif


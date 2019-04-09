
#define printk pspDebugScreenPrintf

#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a)

#if _PSP_FW_VERSION == 620
#define DEVKIT_VER	0x06020010
#define VER_STR	"6.20"

#elif _PSP_FW_VERSION == 639
#define DEVKIT_VER	0x06030910
#define VER_STR	"6.39"

#elif _PSP_FW_VERSION == 660
#define DEVKIT_VER	0x06060010
#define VER_STR	"6.60"

#elif _PSP_FW_VERSION == 661
#define DEVKIT_VER	0x06060110
#define VER_STR	"6.61"
#endif

extern void recovery_sysmem(void);
extern void doKernelExploit(const char* msg);

extern void sync_cache(void);
extern int kernel_permission_call(void);

extern int sceDisplaySetHoldMode(int);
extern int sceHttpStorageOpen(int a0, int a1, int a2);
extern int sceKernelPowerLock(unsigned int, unsigned int);

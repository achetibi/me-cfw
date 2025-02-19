#ifndef __DCMAN_H__
#define __DCMAN_H__

enum
{
	TA79v1,
	TA79v2,
	TA79v3,
	TA81,
	TA82,
	TA85,
	TA85v2,
	TA86,
	TA88,
	TAUN,
	DEVKIT
};

int dcGetHardwareInfo(u32 *ptachyon, u32 *pbaryon, u32 *ppommel, u32 *pmb, u64 *pfuseid, u32 *pfuseconfig, u32 *pnandsize);
int dcPatchModule(char *modname, int type, u32 addr, u32 word);
int dcPatchModuleString(char *modname, char *string, char *replace);
int dcGetCancelMode();
int dcSetCancelMode(int mode);
int dcLflashStartFatfmt(int argc, char *argv[]);
int dcGetNandInfo(u32 *pagesize, u32 *ppb, u32 *totalblocks);
int dcLockNand(int flag);
int dcUnlockNand();
int dcReadNandBlock(u32 page, u8 *block);
int dcWriteNandBlock(u32 page, u8 *user, u8 *spare);
int dcEraseNandBlock(u32 page);
void dcClearCache();
void SW(u32 word, u32 address);
u32  LW(u32 address);

#endif


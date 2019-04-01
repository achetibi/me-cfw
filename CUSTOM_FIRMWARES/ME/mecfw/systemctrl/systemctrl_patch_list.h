#ifndef __systemctrl_patch_list__
#define __systemctrl_patch_list__


struct sctrlLibFunc{
	u32 GetPartitionInfoFuncAddr;
	u32 FindDriverFuncAddr;
	u32 SetUserLevelPatchAddr;
	u32 SetDevkitPatchAddr;
	u32 LoadExecVshFuncAddr;
	u32 LoadExecVshFuncAddr_05g;
	u32 FatfsDeviceFlagAddr;
};
/*
struct UtilityList{
	u32 LoadModulePatchAddr;
	u32 UnloadModulePatchAddr;
	u32 LoadModuleAddr;
	u32 UnloadModuleAddr;
};

struct AddrList{
	struct InitList init_list;
	struct UtilityList utility_list;
};
*/

#if _PSP_FW_VERSION == 620
#define CFW_VER_MAJOR	0x0602
#define CFW_VER_MINOR   0x0010

#elif _PSP_FW_VERSION == 639
#define CFW_VER_MAJOR	0x0603
#define CFW_VER_MINOR   0x0910

#elif _PSP_FW_VERSION == 660
#define CFW_VER_MAJOR	0x0606
#define CFW_VER_MINOR   0x0010

#elif _PSP_FW_VERSION == 661
#define CFW_VER_MAJOR	0x0606
#define CFW_VER_MINOR   0x0110

#else
#error systemctrl_patch_list.h
#endif

#define CFW_VERSION		(CFW_VER_MAJOR << 16 | CFW_VER_MINOR)

#endif
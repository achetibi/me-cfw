#ifndef __ADDR_LIST_H__
#define __ADDR_LIST_H__


struct Galaxy_patch {
	u32 InitFuncPatchAddr;
	u32 IsoOpenPatchAddr;
	u32 IsoReadPatchAddr1;
	u32 IsoReadPatchAddr2;
	u32 IsoClosePatchAddr;
	u32 UmdCheckMediumAddr;
	u32 UmdMutexLockAddr;
	u32 UmdMutexUnlockAddr;
	u32 CreateThreadPatchAddr;
	u32 StartThreadPatchAddr;
	u32 IsoFdPatchAddr;
	u32 IsoInfoPatchAddr;
	u32 CallbackFlagPatchAddr;
};

#if _PSP_FW_VERSION == 620
static const struct Galaxy_patch galaxy_patch_addr = {
	.InitFuncPatchAddr	= 0x00003BD8,
	.IsoOpenPatchAddr	= 0x00003BF0,
	.IsoReadPatchAddr1	= 0x00004358,
	.IsoReadPatchAddr2	= 0x0000582C,
	.IsoClosePatchAddr	= 0x00007C28,
	.UmdCheckMediumAddr	= 0x00003624,
	.UmdMutexLockAddr	= 0x00004EAC,
	.UmdMutexUnlockAddr	= 0x00004F1C,
//	.CreateThreadPatchAddr	= 0x000191B4,
//	.StartThreadPatchAddr	= 0x00019358,
	.IsoFdPatchAddr		= 0x00000184 + 0x000087C0, // See 0x00004C58
	.IsoInfoPatchAddr	= 0x00005BB4 - 0x00005BA4 + 0x00000184 + 0x000087C0,
	.CallbackFlagPatchAddr	= 0x00000114 + 0x000087C0, // See 0x0000332C
};
#elif  _PSP_FW_VERSION == 639
static const struct Galaxy_patch galaxy_patch_addr = {
	.InitFuncPatchAddr	= 0x00003C5C,
	.IsoOpenPatchAddr	= 0x00003C78,
	.IsoReadPatchAddr1	= 0x00004414,
	.IsoReadPatchAddr2	= 0x0000596C,
	.IsoClosePatchAddr	= 0x00007D68,
	.UmdCheckMediumAddr	= 0x000036A8,
	.UmdMutexLockAddr	= 0x00004FEC,
	.UmdMutexUnlockAddr	= 0x0000505C,
//	.CreateThreadPatchAddr	= 0x000191B4,
//	.StartThreadPatchAddr	= 0x00019358,
	.IsoFdPatchAddr		= 0x00000188 + 0x00008900, // See 0x00004D98
	.IsoInfoPatchAddr	= 0x00005BB4 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.CallbackFlagPatchAddr	= 0x00000114 + 0x00008900, // See 0x000033B0
};
#elif _PSP_FW_VERSION == 660
static const struct Galaxy_patch galaxy_patch_addr = {
	.InitFuncPatchAddr	= 0x00003C5C,
	.IsoOpenPatchAddr	= 0x00003C78,
	.IsoReadPatchAddr1	= 0x00004414,
	.IsoReadPatchAddr2	= 0x0000596C,
	.IsoClosePatchAddr	= 0x00007D68,
	.UmdCheckMediumAddr	= 0x000036A8,
	.UmdMutexLockAddr	= 0x00004FEC,
	.UmdMutexUnlockAddr	= 0x0000505C,
//	.CreateThreadPatchAddr	= 0x00019264,
//	.StartThreadPatchAddr	= 0x00019408,
	.IsoFdPatchAddr		= 0x00000188 + 0x00008900, // See 0x00004D98
	.IsoInfoPatchAddr	= 0x00005BD8 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.CallbackFlagPatchAddr	= 0x00000114 + 0x00008900, // See 0x000033B0
};
#else
#error FW error
#endif

#endif


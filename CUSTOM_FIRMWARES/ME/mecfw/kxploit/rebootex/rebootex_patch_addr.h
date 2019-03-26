#ifndef __rebootex_patch_list__
#define __rebootex_patch_list__


struct FunctionList{
	u32 BootLfatOpen;
	u32 BootLfatRead;
	u32 BootLfatClose;
	u32 CheckPspConfig;
	u32 DcacheClearAddr;
	u32 IcacheClearAddr;
};

struct PatchAddrList{
	u32 BootLfatOpenPatch;
	u32 BootLfatReadPatch;
	u32 BootLfatClosePatch;
	u32 CheckPspConfigPatch;
	u32 KdebugPatchAddr;
	u32 BtHeaderPatchAddr;
	u32 LfatMountPatchAddr;
	u32 LfatSeekPatchAddr1;
	u32 LfatSeekPatchAddr2;
	u32 LoadCorePatchAddr;
	u32 HashCheckPatchAddr;
};

struct MmlmdPatchList {
	u32 ModuleOffsetAddr;
	u32 SigcheckPatchAddr;
	u32 SigcheckFuncAddr;
	u32 DecryptPatchAddr;
	u32 DecryptFuncAddr;
};

struct AddrList{
	struct FunctionList function_list_01g;
	struct PatchAddrList patch_list_01g;
	struct FunctionList function_list_02g;
	struct PatchAddrList patch_list_02g;
	struct MmlmdPatchList memlmd_list;
};

#if _PSP_FW_VERSION == 620
static const struct AddrList rebootex_patch_list = {
	.function_list_01g = {
		.BootLfatOpen	= 0x886082AC, // OK
		.BootLfatRead	= 0x88608420, // OK
		.BootLfatClose	= 0x886083C4, // OK
		.CheckPspConfig	= 0x8860565C, // OK
		.DcacheClearAddr= 0x88600938, // OK
		.IcacheClearAddr= 0x886001E4, // OK
	},
	.patch_list_01g = {
		.BootLfatOpenPatch	= 0x886026DC, // OK
		.BootLfatReadPatch	= 0x8860274C, // OK
		.BootLfatClosePatch	= 0x88602778, // OK
		.CheckPspConfigPatch= 0x886070F0, // OK
		.KdebugPatchAddr	= 0x88603798, // OK
		.BtHeaderPatchAddr	= 0x88605690, // OK
		.LfatMountPatchAddr	= 0x886026D4, // OK
		.LfatSeekPatchAddr1	= 0x88602728, // OK
		.LfatSeekPatchAddr2	= 0x88602740, // OK
		.LoadCorePatchAddr	= 0x88605550, // OK
		.HashCheckPatchAddr	= 0x88607388, // OK
	},

	.function_list_02g = {
		.BootLfatOpen	= 0x88608374, // OK
		.BootLfatRead	= 0x886084E8, // OK
		.BootLfatClose	= 0x8860848C, // OK
		.CheckPspConfig	= 0x88605724, // OK
		.DcacheClearAddr= 0x88600938, // OK
		.IcacheClearAddr= 0x886001E4, // OK
	},
	.patch_list_02g = {
		.BootLfatOpenPatch	= 0x886027A4, // OK
		.BootLfatReadPatch	= 0x88602814, // OK
		.BootLfatClosePatch	= 0x88602840, // OK
		.CheckPspConfigPatch= 0x886071B8, // OK
		.KdebugPatchAddr	= 0x88603860, // OK
		.BtHeaderPatchAddr	= 0x88605758, // OK
		.LfatMountPatchAddr	= 0x8860279C, // OK
		.LfatSeekPatchAddr1	= 0x886027F0, // OK
		.LfatSeekPatchAddr2	= 0x88602808, // OK
		.LoadCorePatchAddr	= 0x88605618, // OK
		.HashCheckPatchAddr	= 0x88607450, // OK
	},

	.memlmd_list = {
		.ModuleOffsetAddr	= 0x00000BC4, // OK
		.SigcheckPatchAddr	= 0x00006914, // OK
		.SigcheckFuncAddr	= 0x0000835C, // OK memlmd_2E208358
		.DecryptPatchAddr	= 0x000068F0, // OK
		.DecryptFuncAddr	= 0x00008374, // OK memlmd_CA560AA6
	},
};

#elif _PSP_FW_VERSION == 639
static const struct AddrList rebootex_patch_list = {
	.function_list_01g = {
		.BootLfatOpen	= 0x88608250,
		.BootLfatRead	= 0x886083C4,
		.BootLfatClose	= 0x88608368,
		.CheckPspConfig	= 0x8860569C,
		.DcacheClearAddr= 0x88600938,
		.IcacheClearAddr= 0x886001E4,
	},
	.patch_list_01g = {
		.BootLfatOpenPatch	= 0x88602768,
		.BootLfatReadPatch	= 0x886027D8,
		.BootLfatClosePatch	= 0x88602804,
		.CheckPspConfigPatch= 0x8860711C,
		.KdebugPatchAddr	= 0x88603848,
		.BtHeaderPatchAddr	= 0x886056D0,
		.LfatMountPatchAddr	= 0x88602760,
		.LfatSeekPatchAddr1	= 0x886027B4,
		.LfatSeekPatchAddr2	= 0x886027CC,
		.LoadCorePatchAddr	= 0x88605590,
		.HashCheckPatchAddr	= 0x886073B4,
	},

	.function_list_02g = {
		.BootLfatOpen	= 0x88608320,
		.BootLfatRead	= 0x88608494,
		.BootLfatClose	= 0x88608438,
		.CheckPspConfig	= 0x8860576C,
		.DcacheClearAddr= 0x88600938,
		.IcacheClearAddr= 0x886001E4,
	},
	.patch_list_02g = {
		.BootLfatOpenPatch	= 0x88602838,
		.BootLfatReadPatch	= 0x886028A8,
		.BootLfatClosePatch	= 0x886028D4,
		.CheckPspConfigPatch= 0x886071EC,
		.KdebugPatchAddr	= 0x88603918,
		.BtHeaderPatchAddr	= 0x886057A0,
		.LfatMountPatchAddr	= 0x88602830,
		.LfatSeekPatchAddr1	= 0x88602884,
		.LfatSeekPatchAddr2	= 0x8860289C,
		.LoadCorePatchAddr	= 0x88605660,
		.HashCheckPatchAddr	= 0x88607484,
	},

	.memlmd_list = {
		.ModuleOffsetAddr	= 0x00000BBC,
		.SigcheckPatchAddr	= 0x00005CC8,
		.SigcheckFuncAddr	= 0x00007AE8,//memlmd_3F2AC9C6
		.DecryptPatchAddr	= 0x00005CA4,
		.DecryptFuncAddr	= 0x00007B08,//memlmd_E42AFE2E
	},
};

#elif _PSP_FW_VERSION == 660
static const struct AddrList rebootex_patch_list = {
	.function_list_01g = {
		.BootLfatOpen	= 0x8860822C,
		.BootLfatRead	= 0x886083A0,
		.BootLfatClose	= 0x88608344,
		.CheckPspConfig	= 0x8860574C,
		.DcacheClearAddr= 0x88600890,
		.IcacheClearAddr= 0x8860013C,
	},
	.patch_list_01g = {
		.BootLfatOpenPatch	= 0x886027C4,//
		.BootLfatReadPatch	= 0x88602834,//
		.BootLfatClosePatch	= 0x88602860,//
		.CheckPspConfigPatch= 0x886070F8,//
		.KdebugPatchAddr	= 0x88603880,//
		.BtHeaderPatchAddr	= 0x88605780,//
		.LfatMountPatchAddr	= 0x886027BC,//
		.LfatSeekPatchAddr1	= 0x88602810,//
		.LfatSeekPatchAddr2	= 0x88602828,//
		.LoadCorePatchAddr	= 0x88605640,//
		.HashCheckPatchAddr	= 0x88607390,//
	},

	.function_list_02g = {
		.BootLfatOpen	= 0x886082EC,
		.BootLfatRead	= 0x88608460,
		.BootLfatClose	= 0x88608404,
		.CheckPspConfig	= 0x8860580C,
		.DcacheClearAddr= 0x88600890,
		.IcacheClearAddr= 0x8860013C,
	},
	.patch_list_02g = {
		.BootLfatOpenPatch	= 0x8860288C,
		.BootLfatReadPatch	= 0x886028FC,
		.BootLfatClosePatch	= 0x88602928,
		.CheckPspConfigPatch= 0x886071B8,
		.KdebugPatchAddr	= 0x88603948,
		.BtHeaderPatchAddr	= 0x88605840,
		.LfatMountPatchAddr	= 0x88602884,
		.LfatSeekPatchAddr1	= 0x886028D8,
		.LfatSeekPatchAddr2	= 0x886028F0,
		.LoadCorePatchAddr	= 0x88605700,
		.HashCheckPatchAddr	= 0x88607450,
	},

	.memlmd_list = {
		.ModuleOffsetAddr	= 0x00000AF8,
		.SigcheckPatchAddr	= 0x00005994,
		.SigcheckFuncAddr	= 0x00007824,//memlmd_6192F715
		.DecryptPatchAddr	= 0x00005970,
		.DecryptFuncAddr	= 0x0000783C,//memlmd_EF73E85B
	},
};

#else
#error Target PatchList is not found !
#endif

#endif
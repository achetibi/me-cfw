#ifndef __horoscope_patch_list__
#define __horoscope_patch_list__


struct InitList{
//	u32 IoGetStatPatchAddr;
//	u32 LoadModulePatchAddr;
	u32 GetGameInfoPatchAddr;
	u32 GetGameInfoFuncAddr;
	u32 ModuleStrInPatchAddr;
	u32 ModuleStrOutPatchAddr;
};

struct UtilityList{
	u32 LoadModulePatchAddr;
	u32 UnloadModulePatchAddr;
	u32 LoadModuleAddr;
	u32 UnloadModuleAddr;
};

struct AddrList{
	struct InitList init_list;
	struct UtilityList utility_list;
	u32 LoadModuleMs2HookAddr;
};


#if _PSP_FW_VERSION == 620
static const struct AddrList horoscope_list = {
	.init_list = {
//		.IoGetStatPatchAddr		= 0x000016F8,
//		.LoadModulePatchAddr	= 0x00001704,
		.GetGameInfoPatchAddr	= 0x000016D8,
		.GetGameInfoFuncAddr	= 0x00001D54,
		.ModuleStrInPatchAddr	= 0x000016DC,
		.ModuleStrOutPatchAddr	= 0x000016E0,
	},
	.utility_list = {
		.LoadModulePatchAddr	= 0x00004894,
		.UnloadModulePatchAddr	= 0x00004960,
		.LoadModuleAddr			= 0x00003F30,
		.UnloadModuleAddr		= 0x00004004,
	},
	.LoadModuleMs2HookAddr = 0x00001C14,
};

#elif _PSP_FW_VERSION == 639
static const struct AddrList horoscope_list = {
	.init_list = {
//		.IoGetStatPatchAddr		= 0x000016F8,
//		.LoadModulePatchAddr	= 0x00001704,
		.GetGameInfoPatchAddr	= 0x000016D8,
		.GetGameInfoFuncAddr	= 0x00001D04,
		.ModuleStrInPatchAddr	= 0x000016DC,
		.ModuleStrOutPatchAddr	= 0x000016E0,
	},
	.utility_list = {
		.LoadModulePatchAddr	= 0x00004908,
		.UnloadModulePatchAddr	= 0x000049D4,
		.LoadModuleAddr			= 0x00003FA4,
		.UnloadModuleAddr		= 0x00004078,
	},
	.LoadModuleMs2HookAddr = 0x00001C44,
};

#elif _PSP_FW_VERSION == 660
static const struct AddrList horoscope_list = {
	.init_list = {
		.GetGameInfoPatchAddr	= 0x000016D8,
		.GetGameInfoFuncAddr	= 0x00001D64,//0x00001D04,
		.ModuleStrInPatchAddr	= 0x000016DC,
		.ModuleStrOutPatchAddr	= 0x000016E0,
	},
	.utility_list = {
		.LoadModulePatchAddr	= 0x00004908,
		.UnloadModulePatchAddr	= 0x000049D4,
		.LoadModuleAddr			= 0x00003FA4,
		.UnloadModuleAddr		= 0x00004078,
	},
	.LoadModuleMs2HookAddr = 0x00001C64,//0x00001C44,
};

#elif _PSP_FW_VERSION == 661
static const struct AddrList horoscope_list = {
	.init_list = {
		.GetGameInfoPatchAddr	= 0x000016D8,
		.GetGameInfoFuncAddr	= 0x00001D64,//0x00001D04,
		.ModuleStrInPatchAddr	= 0x000016DC,
		.ModuleStrOutPatchAddr	= 0x000016E0,
	},
	.utility_list = {
		.LoadModulePatchAddr	= 0x00004908,
		.UnloadModulePatchAddr	= 0x000049D4,
		.LoadModuleAddr			= 0x00003FA4,
		.UnloadModuleAddr		= 0x00004078,
	},
	.LoadModuleMs2HookAddr = 0x00001C64,//0x00001C44,
};
#else
#error Target PatchList is not found !
#endif


#endif
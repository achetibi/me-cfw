#ifndef __horoscope_patch_list__
#define __horoscope_patch_list__


struct InitList{
	u32 IoGetStatPatchAddr;
	u32 LoadModulePatchAddr;
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
};


#if _PSP_FW_VERSION == 639
static const struct AddrList horoscope_list = {
	.init_list = {
		.IoGetStatPatchAddr	= 0x000016F8,
		.LoadModulePatchAddr= 0x00001704,
	},
	.utility_list = {
		.LoadModulePatchAddr	= 0x00004908,
		.UnloadModulePatchAddr	= 0x000049D4,
		.LoadModuleAddr			= 0x00003FA4,
		.UnloadModuleAddr		= 0x00004078,
	},
};
#else
#error Target PatchList is not found !
#endif


#endif
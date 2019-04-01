#ifndef __pos_patch_list__
#define __pos_patch_list__


struct PopsmanList{
	u32 DecryptPatchAddr1;
	u32 DecryptPatchAddr2;
	u32 DecryptPatchAddr3;
	u32 DecryptPatchAddr4;
//	u32 ExitVSHKernelAddr;
	u32 Document_fd_Addr;
	u32 RifCheckPatchAddr1;
	u32 RifCheckPatchAddr2;
	u32 IoOpenPatchAddr;
	u32 IoctlPatchAddr;
	u32 IoReadPatchAddr;
	u32 GetKeyPatchAddr1;
	u32 GetKeyPatchAddr2;
	u32 GetKeyAddr;
	u32 DevkitVerPatchAddr;
};

typedef struct {
	u32 IconPatchAddr;
//	u32 DecryptFuncAddr;
	u32 DecryptPatchAddr;
	u32 GameIdPatchAddr;
}PopsList;

struct AddrList{
	struct PopsmanList popsman_list;
	PopsList pops_list_01g;
	PopsList pops_list_02g;
	PopsList pops_list_04g;
	PopsList pops_list_05g;
	PopsList pops_list_07g;
	PopsList pops_list_11g;
};


#if _PSP_FW_VERSION == 620
static const struct AddrList pops_patch_list = {
	.popsman_list = {
		.DecryptPatchAddr1	= 0x00000B0C, // OK
		.DecryptPatchAddr2	= 0x00000ED4, // OK
		.DecryptPatchAddr3	= 0x00000A28, // OK
		.DecryptPatchAddr4	= 0x00000564, // OK
//		.ExitVSHKernelAddr	= 0x00001FE4, // OK
		.Document_fd_Addr	= 0x00004AE0, // OK
		.RifCheckPatchAddr1	= 0x00002E58, // OK
		.RifCheckPatchAddr2	= 0x00002E5C, // OK
		.IoOpenPatchAddr	= 0x00003CD4, // OK
		.IoctlPatchAddr		= 0x00003CE4, // OK
		.IoReadPatchAddr	= 0x00003CEC, // OK
		.GetKeyPatchAddr1	= 0x00002F90, // OK
		.GetKeyPatchAddr2	= 0x00000CB4, // OK
		.GetKeyAddr			= 0x000026F8, // OK
		.DevkitVerPatchAddr = 0x00001EA8, // OK
	},
	.pops_list_01g = {
		.IconPatchAddr		= 0x0003BCD0, // OK
//		.DecryptFuncAddr	= 0x00016690, // OK
		.DecryptPatchAddr	= 0x0000DE18, // OK
		.GameIdPatchAddr	= 0x0002971C, // OK
	},
	.pops_list_02g = { // 03g
		.IconPatchAddr		= 0x0003BCD0, // OK
//		.DecryptFuncAddr	= 0x00016690, // OK
		.DecryptPatchAddr	= 0x0000DE18, // OK
		.GameIdPatchAddr	= 0x0002971C, // OK
	},
	.pops_list_04g = {
		.IconPatchAddr		= 0x0003BCFC, // OK
//		.DecryptFuncAddr	= 0x0001667C, // OK
		.DecryptPatchAddr	= 0x0000DE1C, // OK
		.GameIdPatchAddr	= 0x00029748, // OK
	},
	.pops_list_05g = {
		.IconPatchAddr		= 0x0003DAE4, // OK
//		.DecryptFuncAddr	= 0x00016D64, // OK
		.DecryptPatchAddr	= 0x0000E534, // OK
		.GameIdPatchAddr	= 0x00029FDC, // OK
	},
};

#elif  _PSP_FW_VERSION == 639
static const struct AddrList pops_patch_list = {
	.popsman_list = {
		.DecryptPatchAddr1	= 0x00000B0C,
		.DecryptPatchAddr2	= 0x00000EAC,
		.DecryptPatchAddr3	= 0x00000A28,
		.DecryptPatchAddr4	= 0x00000564,
//		.ExitVSHKernelAddr	= 0x00001F58,
		.Document_fd_Addr	= 0x00004990,
		.RifCheckPatchAddr1	= 0x00002DCC,
		.RifCheckPatchAddr2	= 0x00002DD0,
		.IoOpenPatchAddr	= 0x00003BA8,
		.IoctlPatchAddr		= 0x00003BB8,
		.IoReadPatchAddr	= 0x00003BC0,
		.GetKeyPatchAddr1	= 0x00002F14,
		.GetKeyPatchAddr2	= 0x00000C8C,
		.GetKeyAddr			= 0x0000266C,
		.DevkitVerPatchAddr = 0x00001E80,
	},
	.pops_list_01g = {
		.IconPatchAddr		= 0x00036F54,
//		.DecryptFuncAddr	= 0x00016210,
		.DecryptPatchAddr	= 0x0000DC04,
		.GameIdPatchAddr	= 0x00025428,	
	},
	.pops_list_02g = { // 03g
		.IconPatchAddr		= 0x00037F90,
//		.DecryptFuncAddr	= 0x00016488,
		.DecryptPatchAddr	= 0x0000DC04,
		.GameIdPatchAddr	= 0x00025934,	
	},
	.pops_list_04g = {
		.IconPatchAddr		= 0x00037F90,
//		.DecryptFuncAddr	= 0x00016468,
		.DecryptPatchAddr	= 0x0000DC04,
		.GameIdPatchAddr	= 0x00025934,
	},
	.pops_list_05g = {
		.IconPatchAddr		= 0x00039D78,
//		.DecryptFuncAddr	= 0x00016B80,
		.DecryptPatchAddr	= 0x0000E31C,
		.GameIdPatchAddr	= 0x000261C8,
	},
	.pops_list_07g = { // 09g
		.IconPatchAddr		= 0x00037F90,
//		.DecryptFuncAddr	= 0x00016468,
		.DecryptPatchAddr	= 0x0000DC04,
		.GameIdPatchAddr	= 0x00025934,
	},
};

#elif _PSP_FW_VERSION == 660
static const struct AddrList pops_patch_list = {
	.popsman_list = {
		.DecryptPatchAddr1	= 0x00000B0C,
		.DecryptPatchAddr2	= 0x00000EAC,
		.DecryptPatchAddr3	= 0x00000A28,
		.DecryptPatchAddr4	= 0x00000564,
//		.ExitVSHKernelAddr	= 0x00001F58,
		.Document_fd_Addr	= 0x00004990,
		.RifCheckPatchAddr1	= 0x00002DCC,
		.RifCheckPatchAddr2	= 0x00002DD0,
		.IoOpenPatchAddr	= 0x00003B9C,//0x00003BA8,
		.IoctlPatchAddr		= 0x00003BAC,//0x00003BB8,
		.IoReadPatchAddr	= 0x00003BB4,//0x00003BC0,
		.GetKeyPatchAddr1	= 0x00002F14,
		.GetKeyPatchAddr2	= 0x00000C8C,
		.GetKeyAddr			= 0x0000266C,
		.DevkitVerPatchAddr = 0x00001E80,
	},
	.pops_list_01g = {
		.IconPatchAddr		= 0x00036D50,//0x00036F54,
//		.DecryptFuncAddr	= 0x00016078,//0x00016210,
		.DecryptPatchAddr	= 0x0000DB78,//0x0000DC04,
		.GameIdPatchAddr	= 0x00025254,//0x00025428,
	},
	.pops_list_02g = { // 03g
		.IconPatchAddr		= 0x00037D8C,
//		.DecryptFuncAddr	= 0x000162F0,
		.DecryptPatchAddr	= 0x0000DB78,
		.GameIdPatchAddr	= 0x00025760,
	},
	.pops_list_04g = {
		.IconPatchAddr		= 0x00037E04,
//		.DecryptFuncAddr	= 0x00016348,
		.DecryptPatchAddr	= 0x0000DBE8,
		.GameIdPatchAddr	= 0x000257D8,
	},
	.pops_list_05g = {
		.IconPatchAddr		= 0x00039BEC,
		.DecryptPatchAddr	= 0x0000E300,
		.GameIdPatchAddr	= 0x0002606C,
	},
	.pops_list_07g = { // 09g
		.IconPatchAddr		= 0x00037E1C,
//		.DecryptFuncAddr	= 0x00016360,//0x00016348,
		.DecryptPatchAddr	= 0x0000DBE8,//0x0000DBE8,
		.GameIdPatchAddr	= 0x000257F0,//0x000257D8,
	},
	.pops_list_11g = {
		.IconPatchAddr		= 0x00036DCC,//0x00037E04,
//		.DecryptFuncAddr	= 0x000160E8,//0x00016348,
		.DecryptPatchAddr	= 0x0000DBE8,//0x0000DBE8,
		.GameIdPatchAddr	= 0x000252D0,//0x000257D8,
	},

};

#elif _PSP_FW_VERSION == 661
static const struct AddrList pops_patch_list = {
	.popsman_list = {
		.DecryptPatchAddr1	= 0x00000B0C,
		.DecryptPatchAddr2	= 0x00000EAC,
		.DecryptPatchAddr3	= 0x00000A28,
		.DecryptPatchAddr4	= 0x00000564,
//		.ExitVSHKernelAddr	= 0x00001F58,
		.Document_fd_Addr	= 0x00004990,
		.RifCheckPatchAddr1	= 0x00002DCC,
		.RifCheckPatchAddr2	= 0x00002DD0,
		.IoOpenPatchAddr	= 0x00003B9C,//0x00003BA8,
		.IoctlPatchAddr		= 0x00003BAC,//0x00003BB8,
		.IoReadPatchAddr	= 0x00003BB4,//0x00003BC0,
		.GetKeyPatchAddr1	= 0x00002F14,
		.GetKeyPatchAddr2	= 0x00000C8C,
		.GetKeyAddr			= 0x0000266C,
		.DevkitVerPatchAddr = 0x00001E80,
	},
	.pops_list_01g = {
		.IconPatchAddr		= 0x00036D50,//0x00036F54,
//		.DecryptFuncAddr	= 0x00016078,//0x00016210,
		.DecryptPatchAddr	= 0x0000DB78,//0x0000DC04,
		.GameIdPatchAddr	= 0x00025254,//0x00025428,
	},
	.pops_list_02g = { // 03g
		.IconPatchAddr		= 0x00037D8C,
//		.DecryptFuncAddr	= 0x000162F0,
		.DecryptPatchAddr	= 0x0000DB78,
		.GameIdPatchAddr	= 0x00025760,
	},
	.pops_list_04g = {
		.IconPatchAddr		= 0x00037E04,
//		.DecryptFuncAddr	= 0x00016348,
		.DecryptPatchAddr	= 0x0000DBE8,
		.GameIdPatchAddr	= 0x000257D8,
	},
	.pops_list_05g = {
		.IconPatchAddr		= 0x00039BEC,
		.DecryptPatchAddr	= 0x0000E300,
		.GameIdPatchAddr	= 0x0002606C,
	},
	.pops_list_07g = { // 09g
		.IconPatchAddr		= 0x00037E1C,
//		.DecryptFuncAddr	= 0x00016360,//0x00016348,
		.DecryptPatchAddr	= 0x0000DBE8,//0x0000DBE8,
		.GameIdPatchAddr	= 0x000257F0,//0x000257D8,
	},
	.pops_list_11g = {
		.IconPatchAddr		= 0x00036DCC,//0x00037E04,
//		.DecryptFuncAddr	= 0x000160E8,//0x00016348,
		.DecryptPatchAddr	= 0x0000DBE8,//0x0000DBE8,
		.GameIdPatchAddr	= 0x000252D0,//0x000257D8,
	},

};

#else
#error Target PatchList is not found !
#endif

#endif

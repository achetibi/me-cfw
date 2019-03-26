#include <pspmoduleexport.h>
#define NULL ((void *) 0)

extern void module_bootstart;
extern void module_info;
extern void module_sdk_version;
static const unsigned int __syslib_exports[6] __attribute__((section(".rodata.sceResident"))) = {
	0xD3744BE0,
	0xF01D73A7,
	0x11B97506,
	(unsigned int) &module_bootstart,
	(unsigned int) &module_info,
	(unsigned int) &module_sdk_version,
};

extern void kuKernelLoadModule;
extern void kuKernelLoadModuleWithApitype2;
extern void kuKernelInitApitype;
extern void kuKernelInitFileName;
extern void kuKernelBootFrom;
extern void kuKernelInitKeyConfig;
extern void kuKernelInitKeyConfig;
extern void kuKernelGetUserLevel;
extern void kuKernelSetDdrMemoryProtection;
extern void kuKernelGetModel;
static const unsigned int __KUBridge_exports[20] __attribute__((section(".rodata.sceResident"))) = {
	0x4C25EA72,
	0x1E9F0498,
	0x8E5A4057,
	0x1742445F,
	0x60DDB4AE,
	0x501E983D,
	0xB0B8824E,
	0xA2ABB6D3,
	0xC4AF12AB,
	0x24331850,
	(unsigned int) &kuKernelLoadModule,
	(unsigned int) &kuKernelLoadModuleWithApitype2,
	(unsigned int) &kuKernelInitApitype,
	(unsigned int) &kuKernelInitFileName,
	(unsigned int) &kuKernelBootFrom,
	(unsigned int) &kuKernelInitKeyConfig,
	(unsigned int) &kuKernelInitKeyConfig,
	(unsigned int) &kuKernelGetUserLevel,
	(unsigned int) &kuKernelSetDdrMemoryProtection,
	(unsigned int) &kuKernelGetModel,
};

extern void sctrlKernelSetInitApitype;
extern void sctrlKernelSetInitFileName;
extern void sctrlKernelSetInitKeyConfig;
extern void sctrlKernelSetInitKeyConfig;
extern void sctrlKernelSetUserLevel;
extern void sctrlKernelSetDevkitVersion;
extern void sctrlHENIsSE;
extern void sctrlHENIsDevhook;
extern void sctrlHENGetVersion;
extern void sctrlHENFindDriver;
extern void sctrlHENFindFunction;
extern void sctrlHENSetMemory;
extern void sctrlSEGetVersion;
extern void sctrlKernelExitVSH;
extern void sctrlKernelLoadExecVSHDisc;
extern void sctrlKernelLoadExecVSHDiscUpdater;
extern void sctrlKernelLoadExecVSHMs1;
extern void sctrlKernelLoadExecVSHMs2;
extern void sctrlKernelLoadExecVSHMs3;
extern void sctrlKernelLoadExecVSHMs4;
extern void sctrlKernelLoadExecVSHWithApitype;
extern void sctrlKernelLoadExecVSHEf2;
extern void sctrlSESetConfig;
extern void sctrlSESetConfigEx;
extern void sctrlSEGetConfig;
extern void sctrlSEGetConfigEx;
extern void sctrlSERstConfig;
extern void sctrlSERstConfigEx;
extern void sctrlSESetMiniConfig;
extern void sctrlSESetMiniConfigEx;
extern void sctrlSEGetMiniConfig;
extern void sctrlSEGetMiniConfigEx;
extern void sctrlSERstMiniConfig;
extern void sctrlSERstMiniConfigEx;
extern void sctrlHENSetStartModuleHandler;
extern void sctrlSEMountUmdFromFile;
extern void sctrlSEUmountUmd;
extern void sctrlSESetDiscType;
extern void sctrlSESetDiscOut;
extern void sctrlKernelBootFrom;
extern void sctrlKernelMsIsEf;
extern void sctrlKernelRand;
extern void sctrlKernelUncompress;
extern void sctrlFindImportLib;
extern void sctrlFindImportByNid;
extern void sctrlHookImportByNid;
extern void sctrlHENPatchSyscall;
extern void sctrlKernelQuerySystemCall;
static const unsigned int __SystemCtrlForUser_exports[96] __attribute__((section(".rodata.sceResident"))) = {
	0x8D5BE1F0,
	0x128112C3,
	0xCB76B778,
	0xB551220C,
	0xEB74FE45,
	0xD8FF9B99,
	0xD339E2E9,
	0x2E2935EF,
	0x1090A2E1,
	0x78E46415,
	0x159AF5CC,
	0x745286D1,
	0xB47C9D77,
	0x2794CCF4,
	0x577AF198,
	0x94FE5E4B,
	0x75643FCA,
	0xABA7F1B0,
	0x7B369596,
	0xD690750F,
	0x2D10FB28,
	0xAF22D576,
	0x1DDDAD0C,
	0xAD4D5EA5,
	0x16C3B7EE,
	0x8E426F09,
	0x32414F2A,
	0x649D290E,
	0xEA85C82C,
	0x27A0CE8A,
	0x0A2C56A2,
	0x1A0FB936,
	0xEDB7E6AC,
	0x78AE7B1D,
	0x1C90BECB,
	0x85B520C6,
	0x512E0CD8,
	0x31C6160D,
	0xFFEFA034,
	0x053FAC1D,
	0xAD9849FE,
	0xB364FBB4,
	0x5FA70BEE,
	0xFAC22931,
	0x74F087FC,
	0x6555BEB2,
	0x826668E9,
	0x56CEAF00,
	(unsigned int) &sctrlKernelSetInitApitype,
	(unsigned int) &sctrlKernelSetInitFileName,
	(unsigned int) &sctrlKernelSetInitKeyConfig,
	(unsigned int) &sctrlKernelSetInitKeyConfig,
	(unsigned int) &sctrlKernelSetUserLevel,
	(unsigned int) &sctrlKernelSetDevkitVersion,
	(unsigned int) &sctrlHENIsSE,
	(unsigned int) &sctrlHENIsDevhook,
	(unsigned int) &sctrlHENGetVersion,
	(unsigned int) &sctrlHENFindDriver,
	(unsigned int) &sctrlHENFindFunction,
	(unsigned int) &sctrlHENSetMemory,
	(unsigned int) &sctrlSEGetVersion,
	(unsigned int) &sctrlKernelExitVSH,
	(unsigned int) &sctrlKernelLoadExecVSHDisc,
	(unsigned int) &sctrlKernelLoadExecVSHDiscUpdater,
	(unsigned int) &sctrlKernelLoadExecVSHMs1,
	(unsigned int) &sctrlKernelLoadExecVSHMs2,
	(unsigned int) &sctrlKernelLoadExecVSHMs3,
	(unsigned int) &sctrlKernelLoadExecVSHMs4,
	(unsigned int) &sctrlKernelLoadExecVSHWithApitype,
	(unsigned int) &sctrlKernelLoadExecVSHEf2,
	(unsigned int) &sctrlSESetConfig,
	(unsigned int) &sctrlSESetConfigEx,
	(unsigned int) &sctrlSEGetConfig,
	(unsigned int) &sctrlSEGetConfigEx,
	(unsigned int) &sctrlSERstConfig,
	(unsigned int) &sctrlSERstConfigEx,
	(unsigned int) &sctrlSESetMiniConfig,
	(unsigned int) &sctrlSESetMiniConfigEx,
	(unsigned int) &sctrlSEGetMiniConfig,
	(unsigned int) &sctrlSEGetMiniConfigEx,
	(unsigned int) &sctrlSERstMiniConfig,
	(unsigned int) &sctrlSERstMiniConfigEx,
	(unsigned int) &sctrlHENSetStartModuleHandler,
	(unsigned int) &sctrlSEMountUmdFromFile,
	(unsigned int) &sctrlSEUmountUmd,
	(unsigned int) &sctrlSESetDiscType,
	(unsigned int) &sctrlSESetDiscOut,
	(unsigned int) &sctrlKernelBootFrom,
	(unsigned int) &sctrlKernelMsIsEf,
	(unsigned int) &sctrlKernelRand,
	(unsigned int) &sctrlKernelUncompress,
	(unsigned int) &sctrlFindImportLib,
	(unsigned int) &sctrlFindImportByNid,
	(unsigned int) &sctrlHookImportByNid,
	(unsigned int) &sctrlHENPatchSyscall,
	(unsigned int) &sctrlKernelQuerySystemCall,
};

extern void sctrlKernelSetInitApitype;
extern void sctrlKernelSetInitFileName;
extern void sctrlKernelSetInitKeyConfig;
extern void sctrlKernelSetInitKeyConfig;
extern void sctrlKernelSetUserLevel;
extern void sctrlKernelSetDevkitVersion;
extern void sctrlHENIsSE;
extern void sctrlHENIsDevhook;
extern void sctrlHENGetVersion;
extern void sctrlHENFindDriver;
extern void sctrlHENFindFunction;
extern void sctrlSEGetVersion;
extern void sctrlKernelExitVSH;
extern void sctrlKernelLoadExecVSHDisc;
extern void sctrlKernelLoadExecVSHDiscUpdater;
extern void sctrlKernelLoadExecVSHMs1;
extern void sctrlKernelLoadExecVSHMs2;
extern void sctrlKernelLoadExecVSHMs3;
extern void sctrlKernelLoadExecVSHMs4;
extern void sctrlKernelLoadExecVSHWithApitype;
extern void sctrlKernelLoadExecVSHEf2;
extern void sctrlSESetConfig;
extern void sctrlSESetConfigEx;
extern void sctrlSEGetConfig;
extern void sctrlSEGetConfigEx;
extern void sctrlSERstConfig;
extern void sctrlSERstConfigEx;
extern void sctrlSESetMiniConfig;
extern void sctrlSESetMiniConfigEx;
extern void sctrlSEGetMiniConfig;
extern void sctrlSEGetMiniConfigEx;
extern void sctrlSERstMiniConfig;
extern void sctrlSERstMiniConfigEx;
extern void sctrlHENSetStartModuleHandler;
extern void sctrlSEMountUmdFromFile;
extern void sctrlSEUmountUmd;
extern void sctrlSESetDiscType;
extern void sctrlSEGetDiscType;
extern void sctrlSESetDiscOut;
extern void sctrlHENSetSpeed;
extern void sctrlHENSetMemory;
extern void sctrlSEGetUmdFile;
extern void sctrlSESetUmdFile;
extern void sctrlSEGetUmdFileEx;
extern void sctrlSESetUmdFileEx;
extern void sctrlSESetBootConfFileIndex;
extern void sctrlHENLoadModuleOnReboot;
extern void sctrlKernelMalloc;
extern void sctrlKernelFree;
extern void isofs_init;
extern void isofs_exit;
extern void isofs_open;
extern void isofs_read;
extern void isofs_close;
extern void isofs_lseek;
extern void isofs_getstat;
extern void isofs_fastinit;
extern void SetSpeed;
extern void sctrlSEApplyConfig;
extern void SystemCtrlForKernel_6A5F76B5;
extern void SystemCtrlForKernel_B86E36D1;
extern void SystemCtrlForKernel_07232EA5;
extern void SystemCtrlForKernel_AC0E84D1;
extern void SystemCtrlForKernel_1F3037FB;
extern void sctrlKernelBootFrom;
extern void sctrlSetStartModuleExtra;
extern void sctrlKernelSetNidResolver;
extern void sctrlKernelMsIsEf;
extern void sctrlKernelResolveNid;
extern void sctrlKernelRand;
extern void sctrlKernelUncompress;
extern void sctrlFindImportLib;
extern void sctrlFindImportByNid;
extern void sctrlHookImportByNid;
extern void sctrlHENPatchSyscall;
extern void sctrlKernelQuerySystemCall;
static const unsigned int __SystemCtrlForKernel_exports[152] __attribute__((section(".rodata.sceResident"))) = {
	0x8D5BE1F0,
	0x128112C3,
	0xCB76B778,
	0xB551220C,
	0xEB74FE45,
	0xD8FF9B99,
	0xD339E2E9,
	0x2E2935EF,
	0x1090A2E1,
	0x78E46415,
	0x159AF5CC,
	0xB47C9D77,
	0x2794CCF4,
	0x577AF198,
	0x94FE5E4B,
	0x75643FCA,
	0xABA7F1B0,
	0x7B369596,
	0xD690750F,
	0x2D10FB28,
	0xAF22D576,
	0x1DDDAD0C,
	0xAD4D5EA5,
	0x16C3B7EE,
	0x8E426F09,
	0x32414F2A,
	0x649D290E,
	0xEA85C82C,
	0x27A0CE8A,
	0x0A2C56A2,
	0x1A0FB936,
	0xEDB7E6AC,
	0x78AE7B1D,
	0x1C90BECB,
	0x85B520C6,
	0x512E0CD8,
	0x31C6160D,
	0xABEF849B,
	0xFFEFA034,
	0xCC9ADCF8,
	0x745286D1,
	0xAC56B90B,
	0xB64186D0,
	0xBBF21F64,
	0xCF817542,
	0x5CB025F0,
	0xCE0A654E,
	0xF9584CAD,
	0xA65E8BC4,
	0x260CA420,
	0xB078D9A0,
	0xE82E932B,
	0x324FB7B1,
	0xCAE6A8E1,
	0x3BC8E648,
	0xDC974FF8,
	0x7E6F2BBA,
	0x98012538,
	0x2F157BAF,
	0x6A5F76B5,
	0xB86E36D1,
	0x07232EA5,
	0xAC0E84D1,
	0x1F3037FB,
	0x053FAC1D,
	0x221400A6,
	0x603EE1D0,
	0xAD9849FE,
	0x32677DD3,
	0xB364FBB4,
	0x5FA70BEE,
	0xFAC22931,
	0x74F087FC,
	0x6555BEB2,
	0x826668E9,
	0x56CEAF00,
	(unsigned int) &sctrlKernelSetInitApitype,
	(unsigned int) &sctrlKernelSetInitFileName,
	(unsigned int) &sctrlKernelSetInitKeyConfig,
	(unsigned int) &sctrlKernelSetInitKeyConfig,
	(unsigned int) &sctrlKernelSetUserLevel,
	(unsigned int) &sctrlKernelSetDevkitVersion,
	(unsigned int) &sctrlHENIsSE,
	(unsigned int) &sctrlHENIsDevhook,
	(unsigned int) &sctrlHENGetVersion,
	(unsigned int) &sctrlHENFindDriver,
	(unsigned int) &sctrlHENFindFunction,
	(unsigned int) &sctrlSEGetVersion,
	(unsigned int) &sctrlKernelExitVSH,
	(unsigned int) &sctrlKernelLoadExecVSHDisc,
	(unsigned int) &sctrlKernelLoadExecVSHDiscUpdater,
	(unsigned int) &sctrlKernelLoadExecVSHMs1,
	(unsigned int) &sctrlKernelLoadExecVSHMs2,
	(unsigned int) &sctrlKernelLoadExecVSHMs3,
	(unsigned int) &sctrlKernelLoadExecVSHMs4,
	(unsigned int) &sctrlKernelLoadExecVSHWithApitype,
	(unsigned int) &sctrlKernelLoadExecVSHEf2,
	(unsigned int) &sctrlSESetConfig,
	(unsigned int) &sctrlSESetConfigEx,
	(unsigned int) &sctrlSEGetConfig,
	(unsigned int) &sctrlSEGetConfigEx,
	(unsigned int) &sctrlSERstConfig,
	(unsigned int) &sctrlSERstConfigEx,
	(unsigned int) &sctrlSESetMiniConfig,
	(unsigned int) &sctrlSESetMiniConfigEx,
	(unsigned int) &sctrlSEGetMiniConfig,
	(unsigned int) &sctrlSEGetMiniConfigEx,
	(unsigned int) &sctrlSERstMiniConfig,
	(unsigned int) &sctrlSERstMiniConfigEx,
	(unsigned int) &sctrlHENSetStartModuleHandler,
	(unsigned int) &sctrlSEMountUmdFromFile,
	(unsigned int) &sctrlSEUmountUmd,
	(unsigned int) &sctrlSESetDiscType,
	(unsigned int) &sctrlSEGetDiscType,
	(unsigned int) &sctrlSESetDiscOut,
	(unsigned int) &sctrlHENSetSpeed,
	(unsigned int) &sctrlHENSetMemory,
	(unsigned int) &sctrlSEGetUmdFile,
	(unsigned int) &sctrlSESetUmdFile,
	(unsigned int) &sctrlSEGetUmdFileEx,
	(unsigned int) &sctrlSESetUmdFileEx,
	(unsigned int) &sctrlSESetBootConfFileIndex,
	(unsigned int) &sctrlHENLoadModuleOnReboot,
	(unsigned int) &sctrlKernelMalloc,
	(unsigned int) &sctrlKernelFree,
	(unsigned int) &isofs_init,
	(unsigned int) &isofs_exit,
	(unsigned int) &isofs_open,
	(unsigned int) &isofs_read,
	(unsigned int) &isofs_close,
	(unsigned int) &isofs_lseek,
	(unsigned int) &isofs_getstat,
	(unsigned int) &isofs_fastinit,
	(unsigned int) &SetSpeed,
	(unsigned int) &sctrlSEApplyConfig,
	(unsigned int) &SystemCtrlForKernel_6A5F76B5,
	(unsigned int) &SystemCtrlForKernel_B86E36D1,
	(unsigned int) &SystemCtrlForKernel_07232EA5,
	(unsigned int) &SystemCtrlForKernel_AC0E84D1,
	(unsigned int) &SystemCtrlForKernel_1F3037FB,
	(unsigned int) &sctrlKernelBootFrom,
	(unsigned int) &sctrlSetStartModuleExtra,
	(unsigned int) &sctrlKernelSetNidResolver,
	(unsigned int) &sctrlKernelMsIsEf,
	(unsigned int) &sctrlKernelResolveNid,
	(unsigned int) &sctrlKernelRand,
	(unsigned int) &sctrlKernelUncompress,
	(unsigned int) &sctrlFindImportLib,
	(unsigned int) &sctrlFindImportByNid,
	(unsigned int) &sctrlHookImportByNid,
	(unsigned int) &sctrlHENPatchSyscall,
	(unsigned int) &sctrlKernelQuerySystemCall,
};

const struct _PspLibraryEntry __library_exports[4] __attribute__((section(".lib.ent"), used)) = {
	{ NULL, 0x0000, 0x8000, 4, 2, 1, &__syslib_exports },
	{ "KUBridge", 0x0000, 0x4001, 4, 0, 10, &__KUBridge_exports },
	{ "SystemCtrlForUser", 0x0000, 0x4001, 4, 0, 48, &__SystemCtrlForUser_exports },
	{ "SystemCtrlForKernel", 0x0000, 0x0001, 4, 0, 76, &__SystemCtrlForKernel_exports },
};

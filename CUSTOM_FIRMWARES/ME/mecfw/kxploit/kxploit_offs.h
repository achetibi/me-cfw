#if _PSP_FW_VERSION == 620
#define sceKernelIcacheInvalidateAll_Offs			0x88000E98
#define sceKernelDcacheWritebackInvalidateAll_Offs	0x88000744
#define sceKernelFindModuleByName_Offs				0x8801EB78 // 0x00017100 + 0x00007A78
#define sceKernelGetModel_Offs						0x8800A1C4
#define sceKernelPowerLockForUser_Offs				0x8800CCBC
#define sceKernelPowerLockForUser_data_offset		0x00004234

#elif _PSP_FW_VERSION == 639
#define sceKernelIcacheInvalidateAll_Offs			0x88000E98
#define sceKernelDcacheWritebackInvalidateAll_Offs	0x88000744
#define sceKernelFindModuleByName_Offs				0x8801E2D8 // 0x00017000 + 0x000072D8
#define sceKernelGetModel_Offs						0x8800A13C
#define sceKernelPowerLockForUser_Offs				0x8800CC34
#define sceKernelPowerLockForUser_data_offset		0x000040F4

#elif _PSP_FW_VERSION == 660
#define sceKernelIcacheInvalidateAll_Offs			0x88000E98
#define sceKernelDcacheWritebackInvalidateAll_Offs	0x88000744
#define sceKernelFindModuleByName_Offs				0x8801DF98 // 0x00017000 + 0x00006F98
#define sceKernelGetModel_Offs						0x8800A0B0
#define sceKernelPowerLockForUser_Offs				0x8800CBB8
#define sceKernelPowerLockForUser_data_offset		0x000040F8

#elif _PSP_FW_VERSION == 661
#define sceKernelIcacheInvalidateAll_Offs			0x88000E98
#define sceKernelDcacheWritebackInvalidateAll_Offs	0x88000744
#define sceKernelFindModuleByName_Offs				0x8801DF98 // 0x00017000 + 0x00006F98
#define sceKernelGetModel_Offs						0x8800A0B0
#define sceKernelPowerLockForUser_Offs				0x8800CBB8
#define sceKernelPowerLockForUser_data_offset		0x000040F8

#else
#error kxploit_offs.h
#endif
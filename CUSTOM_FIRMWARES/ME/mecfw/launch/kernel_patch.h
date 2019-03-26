#if _PSP_FW_VERSION == 620
#define sceKernelLoadModuleUser_Call1				0x00000760
#define sceKernelLoadModuleUser_Call2				0x000007C0

int MemlmdPatchAddr_01g[] =		{ 0x00000F10, 0x000010D8, 0x0000112C, 0x00000134, 0x00000E10, 0x00000E74 };
int MemlmdPatchAddr_02g[] =		{ 0x00000FA8, 0x00001170, 0x000011C4, 0x00000134, 0x00000EA8, 0x00000F0C };

#elif  _PSP_FW_VERSION == 639
#define sceKernelLoadModuleUser_Call1				0x00000760
#define sceKernelLoadModuleUser_Call2				0x000007C0

int MemlmdPatchAddr_01g[] =		{ 0x00000F88, 0x00001150, 0x000011A4, 0x00000134, 0x00000E88, 0x00000EEC };
int MemlmdPatchAddr_02g[] =		{ 0x00001078, 0x00001240, 0x00001294, 0x00000134, 0x00000F78, 0x00000FDC };

#elif _PSP_FW_VERSION == 660
#define sceKernelLoadModuleUser_Call1				0x00000760
#define sceKernelLoadModuleUser_Call2				0x000007C0

int MemlmdPatchAddr_01g[] =		{ 0x00001070, 0x00001238, 0x0000128C, 0x0000020C, 0x00000F70, 0x00000FD4 };
int MemlmdPatchAddr_02g[] =		{ 0x000010F8, 0x000012C0, 0x00001314, 0x0000020C, 0x00000FF8, 0x0000105C };

#else
#error kernel_patch.h
#endif
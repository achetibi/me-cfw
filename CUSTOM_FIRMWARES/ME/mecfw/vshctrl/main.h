#ifndef ___MAIN_H___
#define ___MAIN_H___

#include "inline.h"

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

//#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
//#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
//#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
//#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
//#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);


void ClearCaches();
void unmount_iso();

void Fix150Path(const char *file);
int GetIsoIndex(const char *file);

void SetSpeed(int cpuspeed, int busspeed);

int sceKernelQuerySystemCall(void* func );
void *sceKernelGetGameInfo();
int sceKernelCallSubIntrHandler(int unk1, u32 unk2, int unk3, int unk4);

int PatchUsbStart(const char* driverName, int size, void *args);
int PatchUsbStop(const char* driverName, int size, void *args);


#endif

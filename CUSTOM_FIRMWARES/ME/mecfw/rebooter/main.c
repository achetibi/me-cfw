#include <pspsdk.h>
#include <pspkernel.h>
//#include <psputilsforkernel.h>
#include <pspctrl.h>
#include <psploadexec.h>
#include <stdio.h>
#include <string.h>

PSP_MODULE_INFO("Reboot", 0x1007, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); 
#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) ((x & 0x3FFFFFFF) << 2)

int _sceKernelGzipDecompress();

//u8 *rebootex;

#if PERMANENT == 1
#if PSP_MODEL == 0
#include "../rebootex_lite/rebootex_01g_p.h"
#elif PSP_MODEL == 1
#include "../rebootex_lite/rebootex_02g_p.h"
/*
#elif PSP_MODEL == 2
#include "../rebootex_lite/rebootex_03g_p.h"
*/
#elif PSP_MODEL == 4
#include "../rebootex_lite/rebootex_05g_p.h"
#endif
#else
#if PSP_MODEL == 0
#include "../rebootex_lite/rebootex_01g.h"
#elif PSP_MODEL == 1
#include "../rebootex_lite/rebootex_02g.h"
/*
#elif PSP_MODEL == 2
#include "../rebootex_lite/rebootex_03g.h"
*/
#elif PSP_MODEL == 4
#include "../rebootex_lite/rebootex_05g.h"
#endif
#endif

//sub_00000000
void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

//static int (*func_rebootex)(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
/*
int rebootex_cp(unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5){

//	memcpy((void *) 0x88FC0000, rebootex_buff, 0x4000 );
	memset((void *) 0x88FB0000, 0, 256);

//		_sw(model, 0x88FB0000);
	void (* _sceKernelGzipDecompress)() = (void *)(0x88000000 + 0x0000F8FC);
	_sceKernelGzipDecompress( (void *)0x88fc0000 ,  0x4000 , rebootex ,  NULL );
	ClearCaches();
    
	return func_rebootex(a1, a2, a3, a4, a5);
}

*/
int module_start(SceSize args, void *argp)
{

//	SceModule2* (*_sceKernelFindModuleByName)(char *) = (void *) 0x8801E2D8;

//	SceModule2 *mod = _sceKernelFindModuleByName("sceLoadExec");
//	u32 text_addr = mod->text_addr;

//	MAKE_CALL( text_addr + 0x2D5C, (void *)(*(u32 *)0x88fb0000) );
//	_sw(0x3C0188FC, text_addr + 0x2DA8); // lui $at, 0x88FC

//	_sw( text_addr , 0x88fb0004 );
//	func_rebootex = text_addr;//(void *)(*(u32 *)0x88fb0004);

//	void (* _sceKernelGzipDecompress)() = (void *)(0x88000000 + 0x0000F8FC);
	_sceKernelGzipDecompress( (void *)0x88fc0000 ,  0x4000 , rebootex ,  NULL );
	ClearCaches();

	return 1;
}

int module_stop(void)
{
	return 0;
}


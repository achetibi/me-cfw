#ifndef ___MAIN_H___
#define ___MAIN_H___

//#include <systemctrl.h>
#include "systemctrl_me.h"
#include "inline.h"

#define MAX_MODULE_NUMBER 256

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

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

typedef struct
{
	u32		signature;  // 0
	u16		attribute; // 4  modinfo
	u16		comp_attribute; // 6
	u8		module_ver_lo;	// 8
	u8		module_ver_hi;	// 9
	char	modname[28]; // 0A
	u8		version; // 26
	u8		nsegments; // 27
	int		elf_size; // 28
	int		psp_size; // 2C
	u32		entry;	// 30
	u32		modinfo_offset; // 34
	int		bss_size; // 38
	u16		seg_align[4]; // 3C
	u32		seg_address[4]; // 44
	int		seg_size[4]; // 54
	u32		reserved[5]; // 64
	u32		devkitversion; // 78
	u32		decrypt_mode; // 7C 
	u8		key_data0[0x30]; // 80
	int		comp_size; // B0
	int		_80;	// B4
	int		reserved2[2];	// B8
	u8		key_data1[0x10]; // C0
	u32		tag; // D0
	u8		scheck[0x58]; // D4
	u32		key_data2; // 12C
	u32		oe_tag; // 130
	u8		key_data3[0x1C]; // 134
} __attribute__((packed)) PSP_Header;

typedef struct _NoDrmHookEntry {
	char *libname;
	u32 nid;
	void *addr;
} NoDrmHookEntry;

void ClearCaches();

extern int load_module_get_function(void);
extern int nodrm_get_npdrm_functions(void);
extern void patch_drm_imports(SceModule *mod);
extern void fix_weak_imports(void);
extern int NoDRM_Init(void);
extern void HideCFWDirs(SceModule * mod);
extern void patch_for_old_fw(SceModule *mod);
extern void get_functions_for_old_fw(void);

#endif

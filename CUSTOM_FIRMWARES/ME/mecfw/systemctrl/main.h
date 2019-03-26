#ifndef ___MAIN_H___
#define ___MAIN_H___


#include "malloc.h"
#include "systemctrl_me.h"
#include "nid_resolver.h"
#include "systemctrl_patch_list.h"

#include "inline.h"

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

/*
#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
//#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
//#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

static inline void MAKE_DUMMY_FUNCTION0(u32 a)
{
	_sw(0x03e00008, a );_sw(0x00001021, a + 4 );
}

static inline void MAKE_DUMMY_FUNCTION1(u32 a)
{
	_sw(0x03e00008, a); _sw(0x24020001, a+4);
}
*/

#define MIPS_ADDU(RD,RS,RT) (0x00000021|(RD<<11)|(RT<<16)|(RS<<21))

#define SCE_ERROR_ERRNO_ENOENT			0x80010002
#define SCE_ERROR_ERRNO_EIO				0x80010005
#define SCE_ERROR_ERRNO_EBADF			0x80010009
#define SCE_ERROR_ERRNO_ENODEV			0x80010013
#define SCE_ERROR_ERRNO_ENOTDIR			0x80010014
#define SCE_ERROR_ERRNO_EISDIR			0x80010015
#define SCE_ERROR_ERRNO_EINVAL			0x80010016
#define SCE_ERROR_ERRNO_EMFILE			0x80010018
#define SCE_ERROR_ERRNO_EFBIG			0x8001001B
#define SCE_ERROR_ERRNO_ENAMETOOLONG	0x80010024
#define SCE_ERROR_ERRNO_EFLAG			0x8001B004

int sceKernelCallSubIntrHandler(int unk1, u32 unk2, int unk3, int unk4);
int sceKernelGetSystemStatus(void);
int sceKernelSetQTGP3(void *);
int sceKernelQuerySystemCall(void *addr);

void *search_module_export(SceModule2 *pMod, const char *szLib, u32 nid);
void ClearCaches();
void ClearCachesRange(void *start, unsigned int size);

typedef struct
{
	u32 *meminfo;
	int offset;
	int size;
} MemPart;

typedef struct
{
	int signature;
	int version;
	u32 offset_key_table;
	u32 offset_values_table;
	u32 num_entries; 
} PSFHeader;

typedef struct
{
	u16 label_off;
	char rfu001;
	char data_type;
	int datafield_used;
	int datafield_size;
	int data_off;
} PSFSection;

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

/*PBP header*/
typedef struct { 
   u32 signature; //0
   u32 version; //4
   u32 offset[8]; //8[0],0xc[1],0x10[2],0x14[3],0x18[4],0x1c[5],0x20[6]
} PBP_Header; 

/* ELF file header */
typedef struct { 
	u32		e_magic;//0
	u8		e_class;//4
	u8		e_data;	//5
	u8		e_idver;//6
	u8		e_pad[9];//7
	u16		e_type; //0x10
	u16		e_machine; //0x12
	u32		e_version; //0x14
	u32		e_entry; //0x18
	u32		e_phoff; //0x1C
	u32		e_shoff; //0x20
	u32		e_flags; //0x24
	u16		e_ehsize;//0x28
	u16		e_phentsize;//0x2A
	u16		e_phnum; //0x2C
	u16		e_shentsize; //0x2E
	u16		e_shnum; //0x30
	u16		e_shstrndx; //0x32
} __attribute__((packed)) Elf32_Ehdr;

/* ELF section header */
typedef struct { 
	u32		sh_name; 
	u32		sh_type; 
	u32		sh_flags; 
	u32		sh_addr; 
	u32		sh_offset; 
	u32		sh_size; 
	u32		sh_link; 
	u32		sh_info; 
	u32		sh_addralign; 
	u32		sh_entsize; 
} __attribute__((packed)) Elf32_Shdr;

#if 0
typedef struct _SceLoadCoreExecFileInfo
{
  int unk_0;
  int unk_4; //attr? 0x1 = , 0x2 =
  int unk_8; //API
  int unk_C;
  int unk_10; //offset of start of file (after ~SCE header if it exists)
  int unk_14;
  int unk_18;
  int unk_1C;
  int elf_type; //20 - elf type - 1,2,3 valid
  int topaddr; //24 - address of gzip buffer
  int (*bootstart)(SceSize, void *); //28
  u32 text_addr;//2C
  int unk_30; //30 - size of PRX?
  int unk_34; //text_size
  int unk_38;//data_size
  int unk_3C;//bss_size
  int unk_40; //partition id
  int unk_44;
  int unk_48;
  int unk_4C;
  SceModuleInfo *module_info; //50 - pointer to module info i.e. PSP_MODULE_INFO(...)
  int unk_54;
  short unk_58; //attr as in PSP_MODULE_INFO - 0x1000 = kernel
  short unk_5A; //attr? 0x1 = use gzip
  int unk_5C; //size of gzip buffer to allocate
  int unk_60;
  int unk_64;
  int unk_68;
  int unk_6C;
  reglibin *export_libs; //70
  int num_export_libs; //74
  int unk_78;
  int unk_7C;
  int unk_80;
  unsigned char unk_84[4];
  unsigned int segmentaddr[4]; //88
  unsigned int segmentsize[4]; //98
  unsigned int unk_A8;
  unsigned int unk_AC;
  unsigned int unk_B0;
  unsigned int unk_B4;
  unsigned int unk_B8;
  unsigned int unk_BC;
} SceLoadCoreExecFileInfo;
#endif

#endif


#ifndef ___MAIN_H___

#define ___MAIN_H___

#define printk pspDebugScreenPrintf

#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);

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

extern void recovery_sysmem(void);
extern void doKernelExploit(const char* msg);

extern void sync_cache(void);
extern int kernel_permission_call(void);

extern int sceDisplaySetHoldMode(int);
extern int sceHttpStorageOpen(int a0, int a1, int a2);
extern int sceKernelPowerLock(unsigned int, unsigned int);

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
	u8		e_pad[9];//8
	u16		e_type; //0xa
	u16		e_machine; //0xc
	u32		e_version; 
	u32		e_entry; 
	u32		e_phoff; 
	u32		e_shoff; 
	u32		e_flags; 
	u16		e_ehsize; 
	u16		e_phentsize; 
	u16		e_phnum; 
	u16		e_shentsize; 
	u16		e_shnum; 
	u16		e_shstrndx; 
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

#endif


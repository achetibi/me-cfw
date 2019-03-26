#define SIZE_BUFFER 5*1024*1024

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int

typedef struct {
	u32		signature;			// 0x000
	u16		attribute;			// 0x004
	u16		comp_attribute;		// 0x006
	u8		module_ver_lo;		// 0x008
	u8		module_ver_hi;		// 0x009
	char	modname[28];		// 0x00A
	u8		version;			// 0x026
	u8		nsegments;			// 0x027
	int		elf_size;			// 0x028
	int		psp_size;			// 0x02C
	u32		entry;				// 0x030
	u32		modinfo_offset;		// 0x034
	int		bss_size;			// 0x038
	u16		seg_align[4];		// 0x03C
	u32		seg_address[4];		// 0x044
	int		seg_size[4];		// 0x054
	u32		reserved[5];		// 0x064
	u32		devkitversion;		// 0x078
	u32		decrypt_mode;		// 0x07C
	u8		key_data0[0x30];	// 0x080
	int		comp_size;			// 0x0B0
	int		_80;				// 0x0B4
	int		reserved2[2];		// 0x0B8
	u8		key_data1[0x10];	// 0x0C0
	u32		tag;				// 0x0D0
	u8		scheck[0x58];		// 0x0D4
	u32		key_data2;			// 0x12C
	u32		oe_tag;				// 0x130
	u8		key_data3[0x1C];	// 0x134
	u8		main_data;			//150
} __attribute__((packed)) PSP_Header;

typedef struct {
	u32		signature;			//0x00
	u32		unknown;			//0x04
	u32		off_sfo;			//0x08
	u32		off_icon0;			//0x0C
	u32		off_icon1;			//0x10
	u32		off_pic0;			//0x14
	u32		off_pic1;			//0x18
	u32		off_snd0;			//0x1C
	u32		off_psp;			//0x20
	u32		off_psar;			//0x24
} __attribute__((packed)) PBP_Header;

typedef struct {
	u32		e_magic;
	u8		e_class;
	u8		e_data;
	u8		e_idver;
	u8		e_pad[9];
	u16		e_type;
	u16		e_machine;
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

typedef struct {
	u32		p_type;
	u32		p_offset;
	u32		p_vaddr;
	u32		p_paddr;
	u32		p_filesz;
	u32		p_memsz;
	u32		p_flags;
	u32		p_align;
} __attribute__((packed)) Elf32_Phdr;

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

typedef struct {
	u16		attribute;
	u8		module_ver_lo;
	u8		module_ver_hi;
	char	modname[28];
} __attribute__((packed)) PspModuleInfo;

typedef struct {
	u8		key_data1[0x10];
	u8		scheck[0x58];
} Seed_Header;

typedef struct tagInfo {
	u32		psp_tag;
	u32		oe_tag;
} tagInfo;

enum moduleType {
	MODULE_KERNEL,
	MODULE_VSH,
	MODULE_USER,
	MODULE_PBP
};

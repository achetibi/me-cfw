#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int

enum {
	SECT_VSH 		= 0x01,
	SECT_GAME		= 0x02,
	SECT_UPDATER	= 0x04,
	SECT_POPS		= 0x08,
	SECT_LICENSE	= 0x10,
	SECT_APP		= 0x20,
	SECT_UMDEMU		= 0x40,
	SECT_MLNAPP		= 0x80
} section_flags;

enum {
	TYPE_SECT		= 0x0000,
	TYPE_NOPERCENT	= 0x0001,
	TYPE_PERCENT	= 0x0002,
	TYPE_TWOPERCENT	= 0x0004,
	TYPE_DOLLAR		= 0x8000
} prx_listing_types;

typedef struct BtcnfHeader
{
	int signature;		// 0x00
	int devkit;			// 0x04
	int unknown[2];		// 0x08
	int modestart;		// 0x10
	int nmodes;			// 0x14
	int unknown2[2];	// 0x18
	int modulestart;	// 0x20
	int nmodules;		// 0x24
	int unknown3[2];	// 0x28
	int modnamestart;	// 0x30
	int modnameend;		// 0x34
	int unknown4[2];	// 0x38
} BtcnfHeader;

typedef struct ModeEntry
{
	u16 maxsearch;
	u16 searchstart;
	int modeflag;
	int mode2;
	int reserved[0x05];
} ModeEntry;

typedef struct ModuleEntry
{
	u32 stroffset;
	int reserved;
	u16 flags;
	u16 loadmode;
	int reserved2;
	u8 hash[0x10];
} ModuleEntry;

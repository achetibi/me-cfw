#ifndef __TM_CTRL__
#define __TM_CTRL__


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


#if _PSP_FW_VERSION == 660
#define DEVKIT_VER 0x06060010
#define DEVKIT_VER_STR "660"
#elif _PSP_FW_VERSION == 661
#define DEVKIT_VER 0x06060110
#define DEVKIT_VER_STR "661"
#else
#error devkit_ver
#endif

typedef struct
{
	int index; //0
	int uid;//4
	int resume;//8 unk
	char filename[0xC0];//12
	int mode;//204
	int flags;//208
	int unk2;//212
	SceOff offset;//216
} OpenInfo;

typedef struct
{
	PspIoDrvFileArg *arg; 
	char *file;
	int flags;
	SceMode mode;
} OpenParams;

typedef struct
{
	PspIoDrvFileArg *arg;
	char *data;
	int len;
} ReadParams;

typedef struct
{
	PspIoDrvFileArg *arg;
	u32 ofs;
	int whence;
} LseekParams;

typedef struct
{
	PspIoDrvFileArg *arg;
	const char *filename;
} RemoveParams;

typedef struct
{
	PspIoDrvFileArg *arg;
	const char *dirname;
	SceMode mode;
} MkdirParams;

typedef struct
{
	PspIoDrvFileArg *arg;
	const char *dirname;
} DopenParams;

typedef struct
{
	PspIoDrvFileArg *arg;
	SceIoDirent *dirent;
} DreadParams;

typedef struct
{
	PspIoDrvFileArg *arg;
	const char *file;
	SceIoStat *stat;
} GetStatParams;

typedef struct
{
	PspIoDrvFileArg *arg;
	const char *file;
	SceIoStat *stat;
	int bits;
} ChStatParams;

typedef struct
{
	PspIoDrvFileArg *arg;
	const char *oldname;
	const char *newname;
} RenameParams;

#endif

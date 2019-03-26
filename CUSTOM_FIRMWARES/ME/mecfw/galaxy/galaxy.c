// galaxy.prx from M33 
//
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//#include <systemctrl.h>
#include <systemctrl_me.h>

#include "addr_list.h"

#include "csoread.h"
#include "daxread.h"
#include "galaxy_driver.h"

/*
#define JAL_OPCODE	0x0C000000
#define J_OPCODE	0x08000000
#define SC_OPCODE	0x0000000C
#define JR_RA		0x03e00008

#define NOP	0x00000000

#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 
#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))
*/


PSP_MODULE_INFO("NP9660_Controller", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

void sceKernelSetQTGP3(void *);

STMOD_HANDLER previous = NULL;

u32 module_sdk_version = 0x03060010;
int dataE10[4]={ -1 , -1 , -1 , -1};

SceUID umdfd =-1;				//dataF28
static SceInt64 umd_file_len = 0x7FFFFFFF;	//dataF2C

int umd_open = 0;					//dataF40
int umd_format;						//dataF44

void (* sceUmdCheckMedium)() = NULL;//data1200
int wait_flag = 0;//data1204
void (* _Np9660FdMutexLock)() = NULL;//data1208
u32 sceNp9660_text_addr=0;			//data120C

NP9660ReadParams params;//data1210

void (* _Np9660FdMutexUnLock)() = NULL;//data121C
char *iso_path = NULL;//data1220


//sub_00000168:
void ClearCaches()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int OnModuleStart(SceModule2 *mod)
{
//	u32 text_addr = mod->text_addr;
	char *modinfo=mod->modname;

	if (strcmp(modinfo, "sceNp9660_driver") == 0) 
	{
		sceNp9660_text_addr = mod->text_addr;

		// Patch InitForKernel_ function to return 0x80000000
		//lui	$v0, 0x8000
		_sw(0x3C028000, sceNp9660_text_addr + galaxy_patch_addr.InitFuncPatchAddr );

		// Replace the decryption function with our own
		MAKE_CALL(sceNp9660_text_addr + galaxy_patch_addr.IsoOpenPatchAddr , sub_00000588)

	
		// Patch functions called by the new np9660 ioread & iodevctl funcs
		MAKE_CALL(sceNp9660_text_addr + galaxy_patch_addr.IsoReadPatchAddr1 , umd9660_read );
		MAKE_CALL(sceNp9660_text_addr + galaxy_patch_addr.IsoReadPatchAddr2 , umd9660_read );

		MAKE_JUMP(sceNp9660_text_addr + galaxy_patch_addr.IsoClosePatchAddr , sceIoClosePatched );

		sceUmdCheckMedium = (void *)sceNp9660_text_addr		+ galaxy_patch_addr.UmdCheckMediumAddr;
		_Np9660FdMutexLock = (void *)sceNp9660_text_addr	+ galaxy_patch_addr.UmdMutexLockAddr;
		_Np9660FdMutexUnLock = (void *)sceNp9660_text_addr	+ galaxy_patch_addr.UmdMutexUnlockAddr;

		ClearCaches();
	}

	if( previous )  {  
		return previous( mod );
	}

	return 0;
}
//6.35 +
//#define CREATETHREAD_ADDR	0x000191B4
//#define STARTTHREAD_ADDR	0x00019358

//0x00000340
int module_start()
{
	char *umd; 
	SceUID fd;

	previous = sctrlHENSetStartModuleHandler(OnModuleStart);

	umd= sctrlSEGetUmdFile();
	
	while( ( fd =sceIoOpen(umd,1,0) ) < 0)
	{
		sceKernelDelayThread(10000);
		umd= sctrlSEGetUmdFile();
	}

	sceIoClose(fd);
	return 0;
}

int sceIoClosePatched(SceUID fd)
{
	int r = sceIoClose(fd);

	if(fd == umdfd)//
	{
		umdfd =-1;//
		_sw( 0xFFFFFFFF , sceNp9660_text_addr + galaxy_patch_addr.IsoFdPatchAddr );

		ClearCaches();
	}

	return r;
}

int sub_00000588(const char *path)
{
	//data1220
	iso_path= sctrlSEGetUmdFile();

	sceUmdCheckMedium();//data1200

	OpenIso();

 	int intr = sceKernelCpuSuspendIntr();

#define PATCH_ADDR1	galaxy_patch_addr.IsoInfoPatchAddr
#define PATCH_ADDR2 ( PATCH_ADDR1 + 0x8)
#define PATCH_ADDR3 ( PATCH_ADDR1 + 0x1C )
#define PATCH_ADDR4 ( PATCH_ADDR1 + 0x24 )
	_sw( 0xE0000800 , sceNp9660_text_addr + PATCH_ADDR1 );
 	_sw( 9			, sceNp9660_text_addr + PATCH_ADDR2 );
	_sw( umd_file_len , sceNp9660_text_addr + PATCH_ADDR3 );
	_sw( umd_file_len , sceNp9660_text_addr + PATCH_ADDR4 );

//#define PATCH_ADDR5	0x00008A14
	_sw( 0			, sceNp9660_text_addr + galaxy_patch_addr.CallbackFlagPatchAddr );//0x5B60

	sceKernelCpuResumeIntr(intr);

	if( wait_flag == 0)//data1204
	{
		wait_flag=1;

		sceKernelDelayThread( 800*1000 );
	}

	ClearCaches();

	sceKernelSetQTGP3( dataE10);

	return 0;
}

//sub_00000054 umd9660_read
int umd9660_read(int lba , u8 *buf, int read_size)
{
	_Np9660FdMutexLock();
	params.fpointer = lba;
	params.buf = buf;
	params.read_size = read_size;

	int r = sceKernelExtendKernelStack( 0x2000, (void *)umd9660_read2  , &params );
	_Np9660FdMutexUnLock();
	return r;
}

//sub_00000184//iso open
int OpenIso()
{
	SceUID fd;
	sceIoClose(umdfd);//dataF28

	umd_open =0;//

	while( (fd =sceIoOpen( iso_path ,PSP_O_RDONLY,0)) < 0)
	{
		sceKernelDelayThread(10000);
	}

	umdfd =fd;//dataF28
	umd_format = ISO;//dataF44

	_sw( fd , sceNp9660_text_addr + galaxy_patch_addr.IsoFdPatchAddr );

	if(CisoOpen(fd) >= 0)
	{
		umd_format = CSO;
	}
	else if(DaxOpen(fd) >= 0)
	{
		umd_format = DAX;
	}

	/*dataF2C*/
	umd_file_len = GetIsoDiscSize() - 1;

	umd_open=1;//dataF40

	return 0;
}
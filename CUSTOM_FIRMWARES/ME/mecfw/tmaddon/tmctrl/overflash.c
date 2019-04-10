#include <pspthreadman_kernel.h>

int loc_0000028C()
{
	return 0;
}

int loc_00000294()
{
	return 0x80010086;
}

int loc_0000029(int a0 , int a1 , int a2)
{
	return (a2) ? 0x80010086 : 0x80010016;
}

PspIoDrvFuncs lflash_func  =
{
	loc_0000028C,/* init */
	loc_0000028C,/* exit */
	loc_0000028C,/* open */
	loc_0000028C,/* close */
	loc_0000028C,/* read */
	loc_0000028C,/* write */
	(void *)loc_0000028C,/* lseek */
	loc_0000028C,/* ioctl */

	loc_00000294, /* remove */
	loc_00000294, /* mkdir */
	loc_00000294, /* rmdir */
	loc_00000294, /* dopen */
	loc_00000294, /* dclose */

	loc_00000294,/* dread */
	(void *)loc_0000029, /* getstat */
	loc_00000294,/* chstat */
	loc_00000294,/* rename */
	loc_00000294,/* chdir */

	loc_00000294,/* mount */
	loc_00000294,/* umount */
	loc_0000028C,/* devctl */
	loc_00000294 /* */
};

/*
//660
lflash_emu =  5C B6 00 00 | 04 00 00 00 | 00 02 00 00 | 00 00 00 00 | 5C BA 00 00

lflash_func =
 A8 84 00 00 |
 70 91 00 00 |
 3C 85 00 00 |
 44 86 00 00 |
 78 87 00 00 |
 44 89 00 00 |
 F4 8A 00 00 |
 88 8C 00 00 |

 9C 91 00 00 |
 A8 91 00 00 |
 B4 91 00 00 |
 C0 91 00 00 |
 CC 91 00 00 |
 
 D8 91 00 00 |
 E4 91 00 00 |
 FC 91 00 00 |
 08 92 00 00 |
 14 92 00 00 |

 20 92 00 00 |
 2C 92 00 00 |
 54 8D 00 00 |
 38 92 00 00 |
*/

//data4BC0
PspIoDrv lflash_emu = { "lflash" , 0x4		, 0x200	, NULL , &lflash_func };

//loc_00000B68
int flashfat_init()
{
	flashfat_sema = sceKernelCreateSema( "FlashSema" ,0 ,1 ,1 ,NULL);
	memset( open_info , 0 ,  32 * sizeof(OpenInfo) );//32 * 224//0x1C00
	return 0;
}

//loc_00000268
int flashfat_exit(PspIoDrvArg* arg)
{
	return 0;
}

//loc_00000640
int flashfat_open(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	OpenParams open_params;

	open_params.arg = arg;
	open_params.file = file;
	open_params.flags = flags;
	open_params.mode = mode;

	int res = sceKernelExtendKernelStack( 0x4000 ,(void *)flashfat_open2 , &open_params);//sub_00001A7C
	return res;
}

//loc_00000700
int flashfat_close(PspIoDrvFileArg *arg)
{
	int res = sceKernelExtendKernelStack( 0x4000 ,(void *)flashfat_close2 , arg );//sub_0000142C
	return res;
}

//loc_0000060C
int flashfat_read(PspIoDrvFileArg *arg, char *data, int len)
{
	ReadParams read_params;

	read_params.arg = arg;
	read_params.data = data;
	read_params.len = len;
	
	int res = sceKernelExtendKernelStack(0x4000,(void *)flashfat_read2 , &read_params );//sub_00001374
	return res;
}

//loc_000005D8
int flashfat_write(PspIoDrvFileArg *arg, char *data, int len)
{
	ReadParams read_params;

	read_params.arg = arg;
	read_params.data = data;
	read_params.len = len;
	int res = sceKernelExtendKernelStack(0x4000,(void *)flashfat_write2 , &read_params );//sub_000012A8
	return res;
}

//loc_000005A0
SceOff flashfat_lseek(PspIoDrvFileArg *arg, SceOff ofs, int whence)
{
	LseekParams lseek_params;

	lseek_params.arg = arg;
	lseek_params.ofs = (u32)ofs;
	lseek_params.whence = whence;
	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_lseek2 , &lseek_params );//sub_00001218
	return (SceOff)res;
}

//loc_00000A48
int flashfat_ioctl(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	sceKernelWaitSema( flashfat_sema , 1, NULL);
	int ret;

	switch( cmd ){
		case 0x8003:
		case 0x5001:
		case 0xB804:
		case 0x00208003:
		case 0x00208081:
		case 0x00208007:
		case 0x00208006:
			ret = 0;
			break;
		case 0x00208082:
			ret = 0x80010016;
			break;
		case 0x00208013:		
			ret = 0x80010016;
			if( arg->fs_num == 3 )
				if(sceKernelGetModel() != 0 )
					ret = 0;

			break;
		default:
	//		u32 buff = cmd;
		printf("unk ioctl: 0x%08X\n", cmd);
			//WriteFile( "fatms0:/unk_ioctl.bin", &cmd , sizeof(int));
//			WriteFile( "ms0:/unk_ioctl.bin", "aaa" , sizeof(int));
			//while(1){	sceKernelDelayThread( 1 * 1000 * 1000);}
			ret = 0;
			break;
	}

/*
	if( cmd < 0x00208008 )
	{
		if( cmd < 0x00208006 )
		{
			if( cmd == 0x8003 )
				return 0;

			if( cmd < 0x8004 )
			{
				if( cmd == 0x5001 )
					return 0;

				//goto unc
			}
			else
			{
				if( cmd == 0xB804)
					return 0;

				if( cmd == 0x00208003 )
					return 0;
			}
		}
		else
		{
			return 0;
		}

	}
	else
	{
		if( cmd == 0x00208081)
			return 0;

		if( cmd == 0x00208082)
			return 0x80010016;

		if( cmd == 0x00208013)
		{
			//if flash3
			if( arg->fs_num == 3 )
			{
				if(sceKernelGetModel() != 0 )
					return 0;
			}
			return 0x80010016;
		}
	}
*/
	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//loc_00000570
int flashfat_remove(PspIoDrvFileArg *arg, const char *filename)
{
	RemoveParams remove_params;

	remove_params.arg = arg;
	remove_params.filename = filename;
	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_remove2 , &remove_params );//sub_00001A10
	return res;
}

//loc_0000053C
int flashfat_mkdir(PspIoDrvFileArg *arg, const char *dirname , SceMode mode )
{
	MkdirParams mkdir_params;

	mkdir_params.arg = arg;
	mkdir_params.dirname = dirname;
	mkdir_params.mode = mode;

	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_mkdir2 , &mkdir_params );//sub_00001990
	return res;
}

//loc_0000050C
int flashfat_rmdir(PspIoDrvFileArg *arg, const char *filename)
{
	RemoveParams remove_params;

	remove_params.arg = arg;
	remove_params.filename = filename;
	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_rmdir2 , &remove_params );//sub_00001924
	return res;
}

//loc_000004DC
int flashfat_dopen(PspIoDrvFileArg *arg, const char *dirname)
{
	DopenParams dopen_params;

	dopen_params.arg = arg;
	dopen_params.dirname = dirname;
	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_dopen2 , &dopen_params );//sub_00001808
	return res;
}

//loc_000004C8
int flashfat_dclose(PspIoDrvFileArg *arg)
{
	return sceKernelExtendKernelStack( 0x4000,(void *)flashfat_dclose2 , arg );//sub_00001160
}

//loc_00000498
int flashfat_dread(PspIoDrvFileArg *arg, SceIoDirent *dirent)
{
	DreadParams dread_params;

	dread_params.arg = arg;
	dread_params.dirent = dirent;
	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_dread2 , &dread_params );//sub_000010BC
	return res;
}

//loc_00000464
int flashfat_getstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat)
{
	GetStatParams getstat_params;

	getstat_params.arg = arg;
	getstat_params.file = file;
	getstat_params.stat = stat;
	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_getstat2 , &getstat_params );//sub_00001788
	return res;
}

//loc_0000042C
int flashfat_chstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat , int bits)
{
	ChStatParams chstat_params;

	chstat_params.arg = arg;
	chstat_params.file = file;
	chstat_params.stat = stat;
	chstat_params.bits = bits;

	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_chstat2 , &chstat_params );//sub_000016F8
	return res;
}

//loc_000003F8
int flashfat_rename(PspIoDrvFileArg *arg, const char *oldname, const char *newname )
{
	RenameParams rename_params;

	rename_params.arg = arg;
	rename_params.oldname = oldname;
	rename_params.newname = newname;
	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_rename2 , &rename_params );//sub_0000167C
	return res;
}

//loc_000003C8
int flashfat_chdir(PspIoDrvFileArg *arg, const char *dirname)
{
	DopenParams chdir_params;

	chdir_params.arg = arg;
	chdir_params.dirname = dirname;
	int res = sceKernelExtendKernelStack( 0x4000,(void *)flashfat_chdir2 , &chdir_params );//sub_00001610
	return res;
}

//loc_00000270
int flashfat_mount(PspIoDrvFileArg *arg)
{
	printf("Mount\n");
	return 0;
}
//loc_00000278
int flashfat_umount(PspIoDrvFileArg *arg)
{
	return 0;
}
#if 1
//loc_000009D4
int flashfat_devctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	if( cmd == 0x00005802 )
		return 0;

	if( cmd == 0x00208813)
	{
		if( arg->fs_num == 3 && sceKernelGetModel() != 0)
		{
		    return 0;
		}
		
		return 0x80010016;
	}

	//u32 buff = cmd;
	printf("unk devctl: 0x%08X\n", cmd);
//	WriteFile("fatms0:/unk_devctl.bin", &buff , sizeof(int) );
//	WriteFile("ms0:/unk_devctl.bin", "aaa" , sizeof(int) );
//	while(1){				sceKernelDelayThread( 1 * 1000 * 1000);}

	return 0;
}
#else
int flashfat_devctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	if(cmd == 0x00005802)
	{
		return 0;
	}
	else if(cmd == 0x00208813)
	{
		if(arg->fs_num != 3 || sceKernelGetModel() == 0) return 0x80010016;
	}
	else
	{
		WriteFile("fatms0:/unk_devctl.bin", &cmd, sizeof(u32));
		while(1);
	}

	return 0;
}
#endif

//data4c40
PspIoDrvFuncs flashfat_func =
{
	flashfat_init,//loc_00000B68
	flashfat_exit,//loc_00000268
	flashfat_open,//loc_00000640
	flashfat_close,//loc_00000700
	flashfat_read,//loc_0000060C
    (void *)flashfat_write,//loc_000005D8
	(void *)flashfat_lseek,//loc_000005A0
	flashfat_ioctl,//loc_00000A48

	flashfat_remove,//loc_00000570
	flashfat_mkdir,//loc_0000053C
	flashfat_rmdir,//loc_0000050C
	flashfat_dopen,//loc_000004DC
	flashfat_dclose,//loc_000004C8

	flashfat_dread,//loc_00000498
	flashfat_getstat,//loc_00000464
	flashfat_chstat,//loc_0000042C
	flashfat_rename,//loc_000003F8
	flashfat_chdir,//loc_000003C8

	flashfat_mount,//loc_00000270
	flashfat_umount,//loc_00000278
	flashfat_devctl,//loc_000009D4
	loc_00000294
};


//data4BD4
PspIoDrv flashfat_emu = { "flashfat" , 0x001E0010	,  1	, "FAT over Flash" , &flashfat_func };

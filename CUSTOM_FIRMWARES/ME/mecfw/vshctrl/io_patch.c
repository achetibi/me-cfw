#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspreg.h>
#include <pspthreadman_kernel.h>
#include <psploadcore.h>
#include <pspctrl.h>
#include <psppower.h>
#include <psprtc.h>
#include <pspumd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "systemctrl_me.h"
#include "main.h"

#include "vshctrl_patch_list.h"

#include "virtualpbpmgr.h"
#include "isocache.h"

static SceUID gamedfd = -1, game150dfd = -1, isodfd = -1;
static int device_flag = 0;
static SceUID paramsfo = -1;
static u32 overiso = 0;
static u32 vpbpinited = 0;
static u32 isoindex = 0;

static SceUID iso_subfd = -1;
static char iso_subpath[128];
static int sub_device_flag = 0;

static VirtualPbp vpbp;

enum PSP_Custom_Dir
{
	PSP_DIR_150 = 0,
//	PSP_DIR_XXX = 1,
	PSP_DIR_ISO = 1,
	PSP_DIR_VIDEO = 2,
	PSP_DIR_PLUGIN = 3,
};

static const char *customdir_list[][2] = {
	{"ms0:/PSP/GAME150",		"ef0:/PSP/GAME150"		},
//	{"ms0:/PSP/GAME"CFW_NAME,	"ef0:/PSP/GAME"CFW_NAME	},
	{"ms0:/ISO",				"ef0:/ISO"				},
	{"ms0:/ISO/VIDEO",			"ef0:/ISO/VIDEO"		},
	{"ms0:/seplugins",			"ef0:/seplugins"		},
};

extern SEConfig config;
extern u32 iso_mount;

int CheckDevice(const char *path )
{	
	int ret = -1;
	u16 dev_char = *(u16 *)path;

	if( dev_char == 0x736D )//ms	
	{
		ret = 0;	
	}
	else if( dev_char == 0x6665 )//ef
	{
		ret = 1;
	}

	return ret;
}

void ApplyIsoNamePatch(SceIoDirent *dir)
{
	if (dir->d_name[0] != '.')
	{
		memset(dir->d_name, 0, 256);
		sprintf(dir->d_name, "MMMMMISO%d", isoindex++ );
	}
}

static const char *corruptfix_list[2][2] = {
	{"ms0:/PSP/GAME/%s%%/EBOOT.PBP"		,"ef0:/PSP/GAME/%s%%/EBOOT.PBP"		},
	{"ms0:/PSP/GAME150/%s%%/EBOOT.PBP"	,"ef0:/PSP/GAME150/%s%%/EBOOT.PBP"	},
};

int CorruptIconPatch(char *name, int g150, int device)
{
	char path[256];
	SceIoStat stat;
	const char *hidden_path = corruptfix_list[ (g150 == 0 )? 0:1 ][ ( device == 0)? 0:1 ];
/*
	if (g150 == 0) {
		hidden_path="ms0:/PSP/GAME/%s%%/EBOOT.PBP";
	}
	else {
		hidden_path="ms0:/PSP/GAME150/%s%%/EBOOT.PBP";
	}
*/
	sprintf(path, hidden_path , name);

	memset(&stat, 0, sizeof(stat));
	if (sceIoGetstat(path, &stat) >= 0)
	{
		strcpy(name, "__SCE"); // hide icon
		return 1;
	}

	return 0;
}

void ApplyNamePatch(SceIoDirent *dir, char *patch, int dev)
{
	int res = 0 ;
	if (dir->d_name[0] != '.')
	{
		if (config.hidecorrupt)
		{
			res = CorruptIconPatch(dir->d_name, 1, dev );
		}

		if(res == 0 )
			strcat(dir->d_name, patch);
	}
}

SceUID sceIoDopenPatched(const char *dirname)
{
	int res, g150 = 0, index;
	int k1 = pspSdkSetK1(0); 

	Fix150Path(dirname);

//	printf("dopen %s \n", dirname );

	index = GetIsoIndex(dirname);
	if (index >= 0)
	{
		int res = virtualpbp_open(index); 

		pspSdkSetK1(k1);
		return res;
	}

	char *p = strstr(dirname, "     ");	//__ISO
	if (p)
	{		
		int flag = CheckDevice( dirname );
		if( flag >= 0 )
		{
			sub_device_flag = flag;

			//ms0:/PSP/GAME/test     /
			char str[256];
			strcpy( str , customdir_list[PSP_DIR_ISO][flag] );
			strcat( str , "/" );
			
//			strcpy( str , "ms0:/ISO/");
			strncpy( str+ 9, dirname + 14 , p-(dirname + 14));	
			strcpy( str+ 9 + (p-(dirname+14)), p+5);
//			strcat( str + 9 , p+5 );
			strcpy( iso_subpath , str + 9);

			res = sceIoDopen(str);
			iso_subfd = res;
			pspSdkSetK1(k1);
			return res;
		}			
	}

//	if (strcmp(dirname, "ms0:/PSP/GAME") == 0)
	if (strcmp( dirname + 2, "0:/PSP/GAME") == 0)
	{
		int flag = CheckDevice( dirname );
		if( flag >= 0 )
		{
			device_flag = flag;
			g150 = 1;
		}
	}

	pspSdkSetK1(k1);
	res = sceIoDopen(dirname);
	k1 = pspSdkSetK1(0);

	if (g150)
	{
		//"ms0:/PSP/GAME150"
		game150dfd = sceIoDopen( customdir_list[PSP_DIR_150][device_flag] );

		gamedfd = res;
		overiso = 0; 
	}

	pspSdkSetK1(k1);
	return res;
}

int iso_ctrl(SceIoDirent *dir, int dev)
{		
	char fullpath[128];
	int  res2 = -1;
	int  docache;
	
//	strcpy(fullpath, "ms0:/ISO/");
	strcpy(fullpath , customdir_list[PSP_DIR_ISO][ dev ] );
	strcat(fullpath , "/" );

	strcat(fullpath, dir->d_name);

	if (IsCached(fullpath, &dir->d_stat.st_mtime, &vpbp))
	{
		res2 = virtualpbp_fastadd(&vpbp);	
		docache = 0;
	}
	else	
	{
		res2 = virtualpbp_add(fullpath, &dir->d_stat.st_mtime, &vpbp);	
		docache = 1;
	}

	if (res2 >= 0)
	{				
		ApplyIsoNamePatch(dir);
	
		// Fake the entry from file to directory
		dir->d_stat.st_mode = 0x11FF;
		dir->d_stat.st_attr = 0x0010;
		dir->d_stat.st_size = 0;	
							
		// Change the modifcation time to creation time
		memcpy(&dir->d_stat.st_mtime, &dir->d_stat.st_ctime, sizeof(ScePspDateTime));

		if (docache)
		{
			Cache(&vpbp);
		}
	}

	return res2;
}

static SceFatMsDirentPrivateKernel d_private;

static int iso_subfolder(SceUID fd, SceIoDirent *dir)
{
	int res;

	SceFatMsDirentPrivateKernel *p_backup = dir->d_private;
	dir->d_private = &d_private;
	d_private.size = sizeof(SceFatMsDirentPrivateKernel);

	if(!vpbpinited)
	{
		virtualpbp_init();
		vpbpinited = 1;
	}

	while( (res = sceIoDread( fd , dir) ) > 0 )
	{
		if( ! FIO_S_ISDIR(dir->d_stat.st_mode))
		{
			break;
		}
	}

//	if( p_backup )
//		printf("backup found 0x%08X \n", *(u32 *)p_backup);

	dir->d_private = p_backup;
	
	if (res > 0)
	{
//		char buff[128];
//		strcpy( buff , dir->d_name );
//		sprintf(dir->d_name , "%s/%s", iso_subpath , buff );

		strcpy( d_private.LongName, ( strlen( dir->d_name ) > 0x20)? d_private.FileName : dir->d_name );

//		printf("d_name %s : private %s \n", dir->d_name, d_private.LongName );
		sprintf(dir->d_name , "%s/%s", iso_subpath , d_private.LongName );
		iso_ctrl( dir, sub_device_flag );
	}

	return res;
}

int sceIoDreadPatched(SceUID fd, SceIoDirent *dir)
{
	int res;
	u32 k1 = pspSdkSetK1(0);

	if (vpbpinited)
	{
		res = virtualpbp_dread(fd, dir); 
		if (res >= 0)
		{
			pspSdkSetK1(k1);
			return res;
		}
	}

	if (fd >= 0)
	{
		if(fd == iso_subfd)
		{
			res = iso_subfolder(fd , dir);
			pspSdkSetK1(k1);
			return res;
		}

		if (fd == gamedfd)
		{
			if (game150dfd >= 0)
			{
				res = sceIoDread(game150dfd, dir);

				if (res > 0)
				{
					ApplyNamePatch(dir, "__150", device_flag);
					pspSdkSetK1(k1);
					return res;
				}
				else
				{
					sceIoDclose(game150dfd);
					game150dfd = -1;
				}
			}

			if (game150dfd < 0  && isodfd < 0 && !overiso)
			{
				//"ms0:/ISO"
				isodfd = sceIoDopen( customdir_list[PSP_DIR_ISO][device_flag] );

				if (isodfd >= 0)
				{
					if (!vpbpinited)
					{
						virtualpbp_init();
						vpbpinited = 1;
					}
					else
					{
						virtualpbp_reset();
					}

					ReadCache( device_flag );
					isoindex = 0;
				}
				else
				{
					overiso = 1;
				}
			}

			if (isodfd >= 0)
			{
				SceFatMsDirentPrivateKernel *p_backup = dir->d_private;
				dir->d_private = &d_private;
				d_private.size = sizeof(SceFatMsDirentPrivateKernel);

				while( (res = sceIoDread(isodfd, dir) ) > 0)
				{	
					if( FIO_S_ISDIR(dir->d_stat.st_mode))
					{	
						if( dir->d_name[0] == '.' ||
							strcmp( dir->d_name , "VIDEO") == 0)
							continue;
				
						dir->d_private = p_backup;
						ApplyNamePatch(dir, "     ", device_flag);
						pspSdkSetK1(k1);
						return res;
					}
					else
					{
						if( strlen( dir->d_name ) > 0x20)
						{				
							strcpy( dir->d_name, d_private.FileName);
						}

//						printf("main d_name %s : private 0x%08X \n", dir->d_name, p_backup );
						break;
					}
				}

//				if( p_backup )
//					printf("backup found 0x%08X \n", *(u32 *)p_backup);


				dir->d_private = p_backup;

				if (res > 0)
				{
					iso_ctrl( dir, device_flag );
					pspSdkSetK1(k1);
					return res;
				}
				else
				{
					sceIoDclose(isodfd);
					isodfd = -1;
					overiso = 1;
				}
			}
		}
	}

	res = sceIoDread(fd, dir);

	if (res > 0 && (fd == gamedfd) )
	{
		if (config.hidecorrupt)
			CorruptIconPatch(dir->d_name, 0, 0 );
	}

	pspSdkSetK1(k1);
	return res;
}

int sceIoDclosePatched(SceUID fd)
{
	u32 k1 = pspSdkSetK1(0);
	int res;

	if (vpbpinited)
	{
		res = virtualpbp_close(fd); 
		if (res >= 0)
		{
			pspSdkSetK1(k1);
			return res;
		}
	}

	if (fd == gamedfd)
	{
		gamedfd = -1;
		overiso = 0;
		SaveCache( device_flag );
		device_flag = 0;
	}
	else if( fd == iso_subfd )
	{
		memset(iso_subpath , 0 , sizeof(iso_subpath));
		iso_subfd = -1;
		sub_device_flag = 0;
	}


	pspSdkSetK1(k1);
	return sceIoDclose(fd);
}

SceUID sceIoOpenPatched(const char *file, int flags, SceMode mode)
{
	u32 k1 = pspSdkSetK1(0);
	int index;

	Fix150Path(file);

	//printf("opening file; %s \n", file);
	//Kprintf("opening file; ra = %08X %s\n", sceKernelGetSyscallRA(), file);

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		if(iso_mount)
		{
			unmount_iso();
		}

		int res = virtualpbp_open(index); 
		
		pspSdkSetK1(k1);
		return res;
	}

#ifdef HEN
	else if (strcmp(file, "eflash0a:__hibernation") == 0 )
	{
		pspSdkSetK1(k1);
		paramsfo = sceIoOpen(file, flags, mode);
		return paramsfo;
	}
#endif

	pspSdkSetK1(k1);

	return sceIoOpen(file, flags, mode);
}


int sceIoClosePatched(SceUID fd)
{
	u32 k1 = pspSdkSetK1(0);
	int res = -1;

	if (vpbpinited)
	{
		res = virtualpbp_close(fd);
	}

#ifdef HEN
	if (fd == paramsfo)
	{
		paramsfo = -1;
	}
#endif

	pspSdkSetK1(k1);

	if (res < 0)
		return sceIoClose(fd);

	return res;
}

int sceIoReadPatched(SceUID fd, void *data, SceSize size)
{
	u32 k1 = pspSdkSetK1(0);	
	int res = -1;
	
	if (vpbpinited)
	{
		res = virtualpbp_read(fd, data, size);	
	}

#ifdef HEN
//#if 0
	if (fd == paramsfo)
	{
		pspSdkSetK1(k1);
		res = sceIoRead(fd, data, size);
		pspSdkSetK1(0);

		if( res == 0x200 )
		{
			if( ((u16 *)data)[0xEC/2] == 0x4745 )
			{
				((u16 *)data)[0xEC/2] = 0x474D;
			}
		}

		pspSdkSetK1(k1);
		return res;
	}
#endif

	pspSdkSetK1(k1);

	if (res < 0)
		return sceIoRead(fd, data, size);

	return res;
}

SceOff sceIoLseekPatched(SceUID fd, SceOff offset, int whence)
{
	u32 k1 = pspSdkSetK1(0);
	int res = -1;

	if (vpbpinited)
	{
		res = virtualpbp_lseek(fd, offset, whence);
	}

	pspSdkSetK1(k1);

	if (res < 0)
		return sceIoLseek(fd, offset, whence);

	return res;
}

int sceIoLseek32Patched(SceUID fd, int offset, int whence)
{
	u32 k1 = pspSdkSetK1(0);
	int res = -1;

	if (vpbpinited)
	{
		res = virtualpbp_lseek(fd, offset, whence);
	}

	pspSdkSetK1(k1);

	if (res < 0)
		return sceIoLseek32(fd, offset, whence);

	return res;
}

int sceIoGetstatPatched(const char *file, SceIoStat *stat)
{
	u32 k1 = pspSdkSetK1(0);
	int index;

//	printf("%s file = %s \n", __func__, file);

	Fix150Path(file);

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		int res = virtualpbp_getstat(index, stat); 

		pspSdkSetK1(k1);
		return res;
	}
/*
	if( memcmp("disc", file, 4) == 0)
	{
		pspSdkSetK1(k1);
		return 0;
	}
*/
	pspSdkSetK1(k1);

	return sceIoGetstat(file, stat);
}

int sceIoChstatPatched(const char *file, SceIoStat *stat, int bits)
{
	u32 k1 = pspSdkSetK1(0);
	int index;

	Fix150Path(file);
//	Fix3xxPath(file);

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		int res = virtualpbp_chstat(index, stat, bits); 

		pspSdkSetK1(k1);
		return res;
	}

	pspSdkSetK1(k1);

	return sceIoChstat(file, stat, bits);
}

int sceIoRemovePatched(const char *file)
{
	u32 k1 = pspSdkSetK1(0);
	int index;

	Fix150Path(file);

	index = GetIsoIndex(file);
	if (index >= 0)
	{
		int res = virtualpbp_remove(index);
		
		pspSdkSetK1(k1);
		return res;
	}

	pspSdkSetK1(k1);

	return sceIoRemove(file);
}

int sceIoRmdirPatched(const char *path)
{
	u32 k1 = pspSdkSetK1(0);
	int index;

	Fix150Path(path);

	index = GetIsoIndex(path);
	if (index >= 0)
	{
		int res = virtualpbp_rmdir(index);

		pspSdkSetK1(k1);
		return res;
	}

	pspSdkSetK1(k1);

	return sceIoRmdir(path);
}

int sceIoMkdirPatched(const char *dir, SceMode mode)
{
	int k1 = pspSdkSetK1(0);

	if (strcmp( dir + 2, "0:/PSP/GAME") == 0)
	{
		int i;
		int dev_type = CheckDevice( dir );
		if( dev_type >= 0 )
		{
			for(i=0;i<( sizeof(customdir_list)/ (sizeof(char*) * 2));i++)
			{
				sceIoMkdir( customdir_list[i][dev_type] , mode );
			}
		}
	}

/*
	if (strcmp(dir, "ms0:/PSP/GAME") == 0)
	{
		sceIoMkdir("ms0:/PSP/GAME150", mode);
		sceIoMkdir("ms0:/PSP/GAME"CFW_NAME, mode);
		sceIoMkdir("ms0:/ISO", mode);
		sceIoMkdir("ms0:/ISO/VIDEO", mode);
		sceIoMkdir("ms0:/seplugins", mode);
	}
*/
	pspSdkSetK1(k1);
	return sceIoMkdir(dir, mode);
}

/*
#if _PSP_FW_VERSION == 639
static const struct IoFileMgr_patch_list io_patch_list = {
	.Io_open_offset		= 0x00003FD4,
	.Io_close_offset	= 0x00003F94,
	.Io_read_offset		= 0x000040E4,
	.Io_lseek_offset	= 0x00004154,
	.Io_lseek32_offset	= 0x0000418C,
	.Io_getstat_offset	= 0x00004280,
	.Io_chstat_offset	= 0x000042A0,
	.Io_remove_offset	= 0x0000170C,
	.Io_dopen_offset	= 0x00001444,
	.Io_dread_offset	= 0x000015B8,
	.Io_dclose_offset	= 0x00001668,
	.Io_rmdir_offset	= 0x00004240,
	.Io_mkdir_offset	= 0x00004224,
};

#elif _PSP_FW_VERSION == 660
static const struct IoFileMgr_patch_list io_patch_list = {
	.Io_open_offset		= 0x00003FD0,//
	.Io_close_offset	= 0x00003F90,//
	.Io_read_offset		= 0x000040E0,//
	.Io_lseek_offset	= 0x00004150,//
	.Io_lseek32_offset	= 0x00004188,//
	.Io_getstat_offset	= 0x0000427C,//
	.Io_chstat_offset	= 0x0000429C,//
	.Io_remove_offset	= 0x0000170C,//
	.Io_dopen_offset	= 0x00001444,//
	.Io_dread_offset	= 0x000015B8,//
	.Io_dclose_offset	= 0x00001668,//
	.Io_rmdir_offset	= 0x0000423C,//
	.Io_mkdir_offset	= 0x00004220,//
};

#else
#error iofilemgr_patch.h
#endif
*/

typedef struct
{
	u32 offset;
	void *func;
} IoFileMgr_patch_list;


#if _PSP_FW_VERSION == 620
static const IoFileMgr_patch_list io_patch_list[] = {
	{ 0x00004010, sceIoOpenPatched		},//
	{ 0x00003FD0, sceIoClosePatched		},//
	{ 0x00004120, sceIoReadPatched		},//
	{ 0x00004190, sceIoLseekPatched		},//
	{ 0x000041C8, sceIoLseek32Patched	},//
	{ 0x000042BC, sceIoGetstatPatched	},//
	{ 0x000042DC, sceIoChstatPatched	},//
	{ 0x0000170C, sceIoRemovePatched	},//
	{ 0x00001444, sceIoDopenPatched		},//
	{ 0x000015B8, sceIoDreadPatched		},//
	{ 0x00001668, sceIoDclosePatched	},//
	{ 0x0000427C, sceIoRmdirPatched		},//
	{ 0x00004260, sceIoMkdirPatched		} //
};

#elif _PSP_FW_VERSION == 639
static const IoFileMgr_patch_list io_patch_list[] = {
	{ 0x00003FD4, sceIoOpenPatched		},
	{ 0x00003F94, sceIoClosePatched		},
	{ 0x000040E4, sceIoReadPatched		},
	{ 0x00004154, sceIoLseekPatched		},
	{ 0x0000418C, sceIoLseek32Patched	},
	{ 0x00004280, sceIoGetstatPatched	},
	{ 0x000042A0, sceIoChstatPatched	},
	{ 0x0000170C, sceIoRemovePatched	},
	{ 0x00001444, sceIoDopenPatched		},
	{ 0x000015B8, sceIoDreadPatched		},
	{ 0x00001668, sceIoDclosePatched	},
	{ 0x00004240, sceIoRmdirPatched		},
	{ 0x00004224, sceIoMkdirPatched		}
};

#elif _PSP_FW_VERSION == 660
static const IoFileMgr_patch_list io_patch_list[] = {
	{ 0x00003FD0, sceIoOpenPatched		},
	{ 0x00003F90, sceIoClosePatched		},
	{ 0x000040E0, sceIoReadPatched		},
	{ 0x00004150, sceIoLseekPatched		},
	{ 0x00004188, sceIoLseek32Patched	},
	{ 0x0000427C, sceIoGetstatPatched	},
	{ 0x0000429C, sceIoChstatPatched	},
	{ 0x0000170C, sceIoRemovePatched	},
	{ 0x00001444, sceIoDopenPatched		},
	{ 0x000015B8, sceIoDreadPatched		},
	{ 0x00001668, sceIoDclosePatched	},
	{ 0x0000423C, sceIoRmdirPatched		},
	{ 0x00004220, sceIoMkdirPatched		}
};

#elif _PSP_FW_VERSION == 661
static const IoFileMgr_patch_list io_patch_list[] = {
	{ 0x00003FD0, sceIoOpenPatched		},
	{ 0x00003F90, sceIoClosePatched		},
	{ 0x000040E0, sceIoReadPatched		},
	{ 0x00004150, sceIoLseekPatched		},
	{ 0x00004188, sceIoLseek32Patched	},
	{ 0x0000427C, sceIoGetstatPatched	},
	{ 0x0000429C, sceIoChstatPatched	},
	{ 0x0000170C, sceIoRemovePatched	},
	{ 0x00001444, sceIoDopenPatched		},
	{ 0x000015B8, sceIoDreadPatched		},
	{ 0x00001668, sceIoDclosePatched	},
	{ 0x0000423C, sceIoRmdirPatched		},
	{ 0x00004220, sceIoMkdirPatched		}
};

#else
#error iofilemgr_patch.h
#endif
void IoPatches(void)
{
	SceModule2 *mod = sceKernelFindModuleByName("sceIOFileManager");
	u32 text_addr = mod->text_addr;

	int i;
	for(i = 0; i < (sizeof(io_patch_list) / sizeof(IoFileMgr_patch_list)); i++)
	{
		PatchSyscall(text_addr + io_patch_list[i].offset, io_patch_list[i].func);
	}
/*
	PatchSyscall(text_addr + io_patch_list.Io_dopen_offset, sceIoDopenPatched);
	PatchSyscall(text_addr + io_patch_list.Io_dread_offset, sceIoDreadPatched);
	PatchSyscall(text_addr + io_patch_list.Io_dclose_offset, sceIoDclosePatched);

	PatchSyscall(text_addr + io_patch_list.Io_open_offset, sceIoOpenPatched);
	PatchSyscall(text_addr + io_patch_list.Io_close_offset, sceIoClosePatched);
	PatchSyscall(text_addr + io_patch_list.Io_read_offset, sceIoReadPatched);
	PatchSyscall(text_addr + io_patch_list.Io_lseek_offset, sceIoLseekPatched);
	PatchSyscall(text_addr + io_patch_list.Io_lseek32_offse, sceIoLseek32Patched);//
	PatchSyscall(text_addr + io_patch_list.Io_getstat_offset, sceIoGetstatPatched);
	PatchSyscall(text_addr + io_patch_list.Io_chstat_offset, sceIoChstatPatched);//

	PatchSyscall(text_addr + io_patch_list.Io_remove_offset, sceIoRemovePatched);
	PatchSyscall(text_addr + io_patch_list.Io_rmdir_offset, sceIoRmdirPatched);//
	PatchSyscall(text_addr + io_patch_list.Io_mkdir_offset, sceIoMkdirPatched);
*/
}

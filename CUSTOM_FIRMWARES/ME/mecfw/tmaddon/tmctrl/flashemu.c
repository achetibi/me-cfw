#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <string.h>
#include <stdio.h>
#include <pspsysevent.h>
#include <pspiofilemgr_kernel.h>

#include "systemctrl_m33.h"
#include "tmctrl.h"

int assign_thid = -1;//data4B7C
int wait_ms_flag = 1;//data4B68
SceUID flashfat_sema = 0;	//data69B0

OpenInfo open_info[32];//data4DB0

//sub_00000D68
void wait_ms()
{
	int fd;
	if( wait_ms_flag )//data4B68
	{
		while( ( fd = sceIoOpen( MS_PATH"/ipl.bin" , 1 , 0 )) < 0)
		{
			sceKernelDelayThreadCB( 20000 );
		}
		
		sceIoClose(fd);
		wait_ms_flag = 0;
	}
}

char longpath_buff[256];//data4CA0

void sub_000015DC(const char *path )
{
	strcpy( longpath_buff , MS_PATH );
	strcat( longpath_buff , path );
}


int sub_00000DD0( int index , char *path , int flags , int mode )
{
	wait_ms();//sub_00000D68();
	int uid;

TRY_REOPEN:
	if( flags == 0xD0D0 )//Dopen
		uid = sceIoDopen( path );
	else
		uid = sceIoOpen( path , flags , mode );

//	printk("uid = 0x%08X\n",(u32)uid);
	if( uid == 0x80010018 )
	{
		int i;
		for(i=0;i<32;i++)
		{
			if( open_info[ i ].index != 0
				&& open_info[ i ].resume == 0
				&& open_info[ i ].flags == 1)
			{
				int id = open_info[ i ].uid;
				open_info[ i ].offset = sceIoLseek( id , 0 , 1 );
				sceIoClose( id );
				open_info[ i ].resume = 1;
				printf("backup done %s\n", open_info[i].filename );
				goto TRY_REOPEN;
			}
		}
	}

	if( uid >= 0 )
	{
		open_info[ index ].index = 1;
		open_info[ index ].uid = uid;
		open_info[ index ].flags = flags;
		open_info[ index ].mode = mode;

		if( open_info[ index ].filename != path )
			strncpy( open_info[ index ].filename ,  path , 0xC0 );
	}

	return uid;
}

//get uid from index
int sub_00000FC4(int index )
{
	if( open_info[ index ].index == 0)
		return -1;

	if( open_info[ index ].resume == 0)
		return open_info[ index ].uid;

	int ret = sub_00000DD0(  index ,  open_info[ index ].filename , open_info[ index ].flags , open_info[ index ].mode );
	if( ret >= 0)
	{

		printf("reopen done %s\n", open_info[ index].filename );

		sceIoLseek( open_info[ index ].uid , open_info[ index ].offset , 0 );
		open_info[ index ].resume = 0;
		open_info[ index ].uid = ret;
	}

	return ret;
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);

	if (fd >= 0)
	{
		sceIoWrite(fd, buf, size);
		sceIoClose(fd);
		return 1;
	}

	return -1;
}

#include "overflash2.c"
#include "overflash.c"

static int SysEventHandler(int ev_id, char* ev_name, void* param, int* result)
{
	if( ev_id == 0x4000 )
	{
		int i;
		for(i=0;i<32;i++)
		{
			if( open_info[ i ].index != 0
				&& open_info[ i ].resume == 0
				&& open_info[ i ].flags != 0xD0D0 )
			{
				int uid = open_info[ i ].uid;
				open_info[ i ].offset = sceIoLseek( uid , 0 , 1 );
				sceIoClose( uid );
				open_info[ i ].resume = 1;
			}
		}
	}
	else if ( ev_id == 0x10009 )
	{
		wait_ms_flag = 1;//data4B68
	}

	return 0;
}

int LoadStartModule(char *module)
{
	SceUID mod = sceKernelLoadModule(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, strlen(module)+1, module, NULL, NULL);
}

//sub_000014A4
int flashemu_start()
{
	//wait for mount ms
	wait_ms();

	printf("start_thread\n");
#if 0	
	while( sceKernelFindModuleByName("sceUSB_Driver") == NULL )
	{
		sceKernelDelayThread( 20000 );
	}

	LoadStartModule("ms0:/seplugins/psplink.prx");
	sceKernelDelayThread( 5*1000*1000 );
#endif
	
	sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", IOASSIGN_RDONLY, NULL, 0);
	sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", IOASSIGN_RDWR, NULL, 0);
	printf("Assign done\n");

	int key = sceKernelInitKeyConfig();
	if( key == 0x100 || key == 0x200 || key == 0x400 )
	{	
		sceIoAssign("flash2:", "lflash0:0,2", "flashfat2:",  IOASSIGN_RDWR , NULL , 0 );
	}

	if( sceKernelGetModel() != 0)
	{
		if( (key == 0x100 ) || (key == 0x200) || (key == 0x400 && sceKernelBootFrom() == 128 ) )
		{
			sceIoAssign("flash3:", "lflash0:0,3", "flashfat3:",  IOASSIGN_RDWR , NULL , 0 );
		}
	}

	assign_thid = -1;
	sceKernelExitDeleteThread(0);
	return 0;
}


static PspSysEventHandler event_handler =
{
	sizeof(PspSysEventHandler),
	"",
	0x00FFFF00,
	SysEventHandler
};

void FlashEmu_D780E25C()
{
	sceIoDelDrv("lflash");
	sceIoAddDrv( &lflash_emu );//data4BC0

	sceIoDelDrv("flashfat");
	sceIoAddDrv( &flashfat_emu );//data4BD4

	sceKernelRegisterSysEventHandler( &event_handler );

	int thid = sceKernelCreateThread("SceLfatfsAssign", flashemu_start /*sub_000014A4 */, 100 , 4096 , 0x100000 , 0);
	assign_thid = thid;

	if( thid >= 0)
		 sceKernelStartThread( thid , 0 , 0);
}

int module_reboot_before(SceSize args, void *argp)
{
	u32 wait_sp = 500000;

	sceKernelWaitSema( flashfat_sema , 1, &wait_sp );
	sceKernelDeleteSema( flashfat_sema );

	sceIoUnassign("flash0:");
	sceIoUnassign("flash1:");
	sceIoUnassign("flash2:");
	sceIoUnassign("flash3:");

	sceKernelUnregisterSysEventHandler( &event_handler );//data4B80
	
	return 0;
}

int sceLfatfsWaitReady()
{

	int ret = 0;

	if( assign_thid >= 0)
	{
		ret = sceKernelWaitThreadEnd( assign_thid , NULL );
	}

	return ret;
}

int sceLfatfsStop()
{
	return 0;
}

int sceLFatFs_driver_F1FBA85F()
{
	return 0;
}

//660 sceLFatFs_driver_F28896C0
int sceLFatFs_driver_FFD16142()
{
	return DEVKIT_VER;
}


//660 0xBED8D616
int sceLFatFs_driver_250233F6()
{
	//nand check?
	return 0;
}

int sceLFatFs_driver_3D1BA01A = 0;

/*
 * vshMenu by neur0n 
 * based booster's vshex
 *
 */
#include <pspkernel.h>
#include <psppower.h>
#include <stdio.h>

#include <psploadexec_kernel.h>

#include "common.h"
#include "vshctrl.h"


int TSRThread(SceSize args, void *argp);


/* Define the module info section */
PSP_MODULE_INFO("VshCtrlSatelite", 0, 1, 2);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);


//extern ISO_cache cache[32];
extern int current_video_cnt;
extern char menu_title[];

int menu_mode  = 0;
u32 cur_buttons = 0xFFFFFFFF;
u32 button_on  = 0;
int stop_flag=0;
SceCtrlData ctrl_pad;

int psp_model = 0;

char full_buff[192];
char umd_path[72];

SEConfig cnf;
SEConfig cnf_old;


int scePowerRequestColdReset(int);
/////////////////////////////////////////////////////////////////////////////

int thread_id=0;

int module_start(int argc, char *argv[])
{
	int	thid;


	int len , i;
	char *path = (char *)argv;
	
	if( argc )
	{
		len  = scePaf_strlen( path );

		for(i=0 ;i < len-2;i++)
		{

			if(path[len - i]=='/')
			{
				scePaf_strcpy( umd_path , &path[len - i+1]);
				break;
			}
		}
	}
	
	u32 devkit_version = sceKernelDevkitVersion();
	menu_title[0] = ( devkit_version >> 24 ) + '0';	
	menu_title[2] = ((devkit_version >> 16 ) & 0xFF ) + '0';	
	menu_title[3] = ((devkit_version >> 8  ) & 0xFF ) + '0';

	psp_model = kuKernelGetModel();

	thid = sceKernelCreateThread("VshMenu_Thread", TSRThread, 16 , 0x1000 ,0 ,0);

	thread_id=thid;

	if (thid>=0)
	{
		sceKernelStartThread(thid, 0, 0);
	}
	
	return 0;
}


int module_stop(int argc, char *argv[])
{
	SceUInt time = 100*1000;

	stop_flag=1;

	int i = sceKernelWaitThreadEnd( thread_id , &time );

	if(i<0)
	{
		sceKernelTerminateDeleteThread(thread_id);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int EatKey(SceCtrlData *pad_data, int count)
{
	u32 buttons;
//	int eat_key;
	int i;


	//int result = sceCtrlReadBufferPositive(pad_data,count);

	// copy true value
	scePaf_6BD7452C_memcpy(&ctrl_pad , pad_data , sizeof(SceCtrlData));

	// buttons check
	buttons     = ctrl_pad.Buttons;
	button_on   = ~cur_buttons & buttons;
	cur_buttons = buttons;


	// mask buttons for LOCK VSH controll
	for(i=0;i < count;i++)
	{
		//pad_data[i].Buttons  &= 0xFF7E0C06;
		
		pad_data[i].Buttons &= ~(
		PSP_CTRL_SELECT|PSP_CTRL_START|
		PSP_CTRL_UP|PSP_CTRL_RIGHT|PSP_CTRL_DOWN|PSP_CTRL_LEFT|
		PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|
		PSP_CTRL_TRIANGLE|PSP_CTRL_CIRCLE|PSP_CTRL_CROSS|PSP_CTRL_SQUARE|
		PSP_CTRL_HOME|PSP_CTRL_NOTE);

	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int stop_stock=0;

static void button_func(void)
{
	int res;

	// menu controll
	switch(menu_mode)
	{
	case 0:	
		if( (cur_buttons & ALL_CTRL) == 0)
		{
			menu_mode = 1;
		}
		break;
	case 1:
		if( ( res = menu_ctrl(cur_buttons,button_on)) != 0)
		{
			if(res == 5)
			{
				menu_plugin( &cur_buttons ,&button_on);
			}
			else
			{
				stop_stock = res;		
				menu_mode = 2;
			}
		}
		break;
	case 2: // exit waiting 
		// exit menu
		if( (cur_buttons & ALL_CTRL) == 0)
		{
			stop_flag = stop_stock;
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
int TSRThread(SceSize args, void *argp)
{

	sceKernelChangeThreadPriority(0,8);
	vctrlVSHRegisterVshMenu(EatKey);

	sctrlSEGetConfig(&cnf);
	scePaf_6BD7452C_memcpy(&cnf_old , &cnf , sizeof(SEConfig));

	init_videoCache( umd_path );


	while( stop_flag == 0 )
	{

		//sceDisplayWaitVblankStart();
		if( sceDisplayWaitVblankStart() < 0)
			break; // end of VSH ?

		if(menu_mode > 0)
		{
			menu_draw();
			menu_setup();
		}

		button_func();
	}


	if(	scePaf_memcmp(&cnf_old , &cnf , sizeof(SEConfig)))
		sctrlSESetConfig( &cnf);

	
	char *send=NULL;
	int send_type=0;
	SEConfig *send_config = &cnf_old;

	if ( stop_flag ==2)
	{
		scePowerRequestColdReset(0);
	}
	else if (stop_flag ==3)
	{
		scePowerRequestStandby();
	}
	else if (stop_flag ==4 || stop_flag == 6 )
	{
		if( stop_flag == 6 )
		{
//			vctrlSetBootIndex(4);
			vctrlVSHSetRecovery();
		}
		sctrlKernelExitVSH( NULL );
	}
	else
	{
		send_config = &cnf;

		if (current_video_cnt == -1)
		{
			//vctrlVSHExitVSHMenu( &cnf, NULL , 0 );
		}
		else
		{
			//scePaf_15AFC8D3_snprintf( full_buff , 192 ,"ms0:/ISO/VIDEO/%s", GetVideoName() );	
			GetVideoPath( full_buff , GetVideoName() , current_video_cnt >> 4 );
			//scePaf_11EFC5FD_sprintf( full_buff ,"ms0:/ISO/VIDEO/%s", GetVideoName() );

			send=full_buff;
			send_type= GetVideoType() & ~0x10;
		}

	}

	vctrlVSHExitVSHMenu( send_config , send , send_type );

	return sceKernelExitDeleteThread(0);

}

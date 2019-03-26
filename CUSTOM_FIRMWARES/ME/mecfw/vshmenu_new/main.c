/*
 * vshMenu by neur0n 
 * based booster's vshex
 *
 */
#include <pspkernel.h>
#include <stdio.h>

#include <psploadexec_kernel.h>

#include "common.h"
#include "vshctrl.h"


int TSRThread(SceSize args, void *argp);

PSP_MODULE_INFO("VshCtrlSatelite", 0, 1, 2);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);


//extern ISO_cache cache[32];
extern int current_video_cnt;
extern char menu_title[];
extern int color_cnt;

int menu_mode  = 0;
u32 cur_buttons = 0xFFFFFFFF;
static u32 button_on  = 0;
int stop_flag=0;
//SceCtrlData ctrl_pad;

int psp_model = 0;

char full_buff[192];
char umd_path[72];

SEConfig cnf;
SEConfig cnf_old;

Menu_pack main_menu[] =
{
	{"CPU CLOCK XMB  "		, TMENU_FUNCTION_VALUE	, change_xmb_clock		, 0 },
	{"CPU CLOCK GAME "		, TMENU_FUNCTION_VALUE	, change_game_clock		, 0 },
	{"USB DEVICE     "		, TMENU_FUNCTION_VALUE	, change_usb			, 0 },
	{"UMD ISO MODE   "		, TMENU_FUNCTION_VALUE	, change_umd_mode		, 0 },
	{"ISO VIDEO MOUNT"		, TMENU_FUNCTION_VALUE	, change_umd_video		, 0 },
	{"BACK COLOR     "		, TMENU_FUNCTION_VALUE	, change_back_color		, 0 },
	{"XMB  PLUGINS   "		, TMENU_SWITCH			, &cnf.plugvsh			, 0 },
	{"GAME PLUGINS   "		, TMENU_SWITCH			, &cnf.pluggame			, 0 },
	{"POPS PLUGINS   "		, TMENU_SWITCH			, &cnf.plugpop			, 0 },
	{"PLUGINS MANAGER"		, TMENU_FUNCTION		, menu_plugin			, 2 },
	{"ENTER RECOVERY "		, TMENU_FUNCTION_EXIT	, request_enter_recovery, 2 },
	{"SHUTDOWN DEVICE"		, TMENU_FUNCTION_EXIT	, request_shutdown		, 1 },
	{"SUSPEND DEVICE"		, TMENU_FUNCTION_EXIT	, request_suspend		, 1 },
	{"RESET DEVICE"			, TMENU_FUNCTION_EXIT	, request_reset_device	, 1 },
	{"RESET VSH"			, TMENU_FUNCTION_EXIT	, request_reset_vsh		, 1 },
	{"EXIT"					, TMENU_EXIT			, NULL					, 1 },
	{NULL,0,"", 0}

};

VshConfig config_mini;
VshConfig config_mini_backup;

int *main_menu_switch = (void *)config_mini.menu_switch;

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

	sctrlSEGetMiniConfig(&config_mini);
	scePaf_memcpy( &config_mini_backup, &config_mini, sizeof(VshConfig));
	
	color_cnt = config_mini.vsh_color;
	change_back_color( FUNC_CHANGE_VALUE, 0 );

	init_font_table();
	
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

	release_font_table();

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int EatKey(SceCtrlData *pad_data, int count)
{
	u32 buttons;
	int i;

	// buttons check
	buttons		= pad_data->Buttons;
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

u32 get_buttons()
{
	u32 ret = button_on;
	button_on = 0;
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int display_menu(const Menu_pack *menu_list, int *switch_list, int exit_type);
void (* exit_func)() = NULL;

int TSRThread(SceSize args, void *argp)
{

	sceKernelChangeThreadPriority( thread_id,8);
	vctrlVSHRegisterVshMenu(EatKey);

	sctrlSEGetConfig(&cnf);
	scePaf_memcpy(&cnf_old , &cnf , sizeof(SEConfig));
	init_videoCache( umd_path );

	while( (cur_buttons & ALL_CTRL) != 0)
	{
		sceKernelDelayThread( 10*1000 );
	}

	stop_flag = display_menu( main_menu, main_menu_switch, 1 );


	if(	scePaf_memcmp(&cnf_old , &cnf , sizeof(SEConfig)))
		sctrlSESetConfig( &cnf);

	config_mini.vsh_color = color_cnt;
	if( scePaf_memcmp( &config_mini_backup, &config_mini, sizeof(VshConfig) ) != 0)
	{
		sctrlSESetMiniConfig(&config_mini);
	}

	char *send = NULL;
	int send_type = 0;
	SEConfig *send_config = &cnf_old;

	if ( stop_flag == 1)
	{
		send_config = &cnf;
		if (current_video_cnt != -1)
		{
			GetVideoPath( full_buff , GetVideoName() );
			send = full_buff;
			send_type = GetVideoType() & ~0x10;
		}
	}
	else if( stop_flag == 2 )
	{
		if( exit_func != NULL)
			exit_func();
	}

	vctrlVSHExitVSHMenu( send_config , send , send_type );

	return sceKernelExitDeleteThread(0);
}

void manager()
{

	Menu_pack main_menu_backup[sizeof(main_menu)/sizeof(Menu_pack)];
	scePaf_memcpy( main_menu_backup, main_menu, sizeof(main_menu) );
	const int menu_max_cnt = (sizeof(main_menu)/sizeof(Menu_pack))-1;

	int i;
	for(i=0;i< (menu_max_cnt);i++)
	{
		if( main_menu_backup[i].path != NULL && main_menu_backup[i].type != TMENU_EXIT )
		{
			main_menu_backup[i].type = TMENU_SWITCH;
			main_menu_backup[i].value = &main_menu_switch[i];
			main_menu_backup[i].draw_type = 0;
		}
	}	

	display_menu( main_menu_backup, NULL, 0 );
}
/*
	PSP VSH
*/
#include <pspkernel.h>
#include <psppower.h>

#include "common.h"
#include "vshctrl.h"

extern SEConfig cnf;
extern int psp_model;

//const char *str_default = "Default";
char str_buffer[256];

/////////////////////////////////////////////////////////////////////////////
// change CPU clock
/////////////////////////////////////////////////////////////////////////////

const u16 cpu_list[]={0, 20, 75, 100, 133, 166, 200, 222, 266, 300, 333};
//const u16 bus_list[]={0, 10, 37, 50, 66, 111, 133, 150, 166};

#define MAX_CLK_LIST (sizeof(cpu_list)/sizeof(u16))

static int cpu2no(int cpu)
{
	int i;

	for(i=0;i< MAX_CLK_LIST;i++)
	{
		if(cpu==cpu_list[i])
			return i;
	
	}
	return 0;
}
/*
int bus2no(int cpu)
{
	int i;

	for(i=0;i< MAX_CLK_LIST;i++)
	{
		if(cpu==bus_list[i])
			return i;
	
	}
	return 0;
}
*/

void *change_clock(int type, int dir , int flag)
{
	int sel;
	int *cpu[2];

	if(flag)
	{
		cpu[0]=&(cnf.umdisocpuspeed);
		cpu[1]=&(cnf.umdisobusspeed);
	}
	else
	{
		cpu[0]=&(cnf.vshcpuspeed);
		cpu[1]=&(cnf.vshbusspeed);
	}

	sel = cpu2no(*cpu[0]);

	if( type == FUNC_GET_STR )
	{
		if( sel )
		{
			scePaf_sprintf(str_buffer ,"%d/%d",  *cpu[0] ,  *cpu[1] );
		}
		else
		{
			scePaf_strcpy( str_buffer , vshmenu_str.Default /* str_default */ );
		}

		return str_buffer;
	}
	else
	{
		// select new
		sel = limit(sel+dir,0,MAX_CLK_LIST-1);

		*cpu[0] = cpu_list[sel];
		*cpu[1] = (cpu_list[sel])/2;//bus_list[sel];
	}

	return NULL;
}

void *change_xmb_clock(int type, int dir )
{
	return change_clock( type, dir, 0);
}

void *change_game_clock(int type, int dir )
{
	return change_clock( type, dir, 1);
}

void *change_usb(int type, int dir )
{
	int sel = cnf.usbdevice;
	char *bridge;


	if( type == FUNC_GET_STR )
	{
		if( sel > 0 && sel < 5)
		{	
			scePaf_sprintf( str_buffer ,"Flash %d", sel -1);
			bridge = str_buffer;
		}
		else 
		{
			bridge = ( sel == 5 )?
				((psp_model == 4)? vshmenu_str.HiddenStorage:vshmenu_str.UmdDisc)
				:
				vshmenu_str.MemoryStick;
		}

		return bridge;
	}
	else
	{
		// select new
		sel = limit(sel+dir,0, 5);
		cnf.usbdevice = sel;

	}

	return NULL;
}

static char **iso[] = {
	&vshmenu_str.umd_mode_list.Normal,
	&vshmenu_str.umd_mode_list.OE,
	&vshmenu_str.umd_mode_list.M33,
	&vshmenu_str.umd_mode_list.NP9660,
	&vshmenu_str.umd_mode_list.ME,
	&vshmenu_str.umd_mode_list.INFERNO
};

#define UMD_MODE_CNT (sizeof(iso)/sizeof(char*))

void *change_umd_mode(int type ,int dir )
{
	int sel = cnf.umdmode;

	if(type == FUNC_GET_STR)
	{	
		return *iso[ limit( sel , 0 , UMD_MODE_CNT - 1)];
	}
	else
	{
		// select new
		sel = limit(sel+dir, ( psp_model == 4 )?1:0, UMD_MODE_CNT - 1);
		cnf.umdmode=sel;
	}

	return NULL;
}

typedef struct {
	char** name;
	const u32 color;
} ColorList;

ColorList color_list[] = {
	{ &(vshmenu_str.color_list.red)		, 0xc00000FF },
	{ &(vshmenu_str.color_list.green)	, 0xc000FF00 },
	{ &(vshmenu_str.color_list.blue)	, 0xc0FF0000 },
	{ &(vshmenu_str.color_list.pink)	, 0x90FF80FF },
	{ &(vshmenu_str.color_list.purple)	, 0xa0FF0080 },
	{ &(vshmenu_str.color_list.orange)	, 0x804080FF },
	{ &(vshmenu_str.color_list.yellow)	, 0xb000FFFF },
	{ &(vshmenu_str.color_list.black)	, 0x80000000 },
	{ &(vshmenu_str.color_list.white)	, 0xc0FFFFFF },
};

#define COLOR_CNT	(sizeof(color_list)/sizeof(ColorList))

u32 back_color = 0xc00000ff;
int color_cnt = 0;

void *change_back_color(int type ,int dir )
{
	int sel = color_cnt;

	if(type == FUNC_GET_STR)
	{	
		return (void *)*color_list[ limit( sel , 0 , COLOR_CNT - 1)].name;
	}
	else
	{
		// select new
		sel = limit(sel+dir, 0,  COLOR_CNT - 1 );
		back_color = color_list[ sel ].color;	
		color_cnt = sel;
	}

	return NULL;
}

void request_shutdown()
{
	scePowerRequestStandby();
}

void request_suspend()
{
	scePowerRequestSuspend();
}

void request_reset_device()
{
	scePowerRequestColdReset(0);
}

void request_reset_vsh()
{
	sctrlKernelExitVSH( NULL );
}

void request_enter_recovery()
{
	vctrlVSHSetRecovery();
	sctrlKernelExitVSH( NULL );
}

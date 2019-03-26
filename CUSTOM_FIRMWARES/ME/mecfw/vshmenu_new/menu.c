/*
	PSP VSH MENU controll
	based Booster's vshex
*/

#include "common.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern int pwidth;
//extern int main_menu_switch[];
extern u32 cur_buttons ;
extern u32 back_color;
/////////////////////////////////////////////////////////////////////////////
// draw strings
/////////////////////////////////////////////////////////////////////////////
char menu_title[] = "X.XX VSH MENU";

/*
const char * enable_disable[] ={
	"Enable",
	"Disable"
};
*/
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int menu_sel = 0;//TMENU_XMB_CLOCK;

const int xyPoint[] = {0x98, 0x30, 0xC0, 0xA0, 0x70, 0x08, 0x0E};
const int xyPoint2[] ={0xB0, 0x30, 0xD8, 0xB8, 0x88, 0x08, 0x11};


/////////////////////////////////////////////////////////////////////////////
// draw menu 
/////////////////////////////////////////////////////////////////////////////
extern void (* exit_func)();

int menu_ctrl( const Menu_pack *menu_list, int *switch_list,int menu_max_cnt)
{
	u32 button_on = get_buttons();
	int direction;

	if( (button_on&PSP_CTRL_SELECT) ||
		(button_on&PSP_CTRL_HOME))
	{
		menu_sel = menu_max_cnt - 1;
//		menu_sel = TMENU_EXIT;
		return 1;
	}

	// change menu select
	direction = 0;
	if(button_on&PSP_CTRL_DOWN) direction++;
	if(button_on&PSP_CTRL_UP) direction--;
	if( direction )
	{
		do
		{
			menu_sel = limit(menu_sel+direction,0, menu_max_cnt -1 );
//		}while(  main_menu_switch[menu_sel] != 0);
		}while( switch_list && switch_list[menu_sel] != 0);
	}

	// LEFT & RIGHT
	direction = -2;
	if(button_on&PSP_CTRL_LEFT)   direction = -1;
	if(button_on&PSP_CTRL_CROSS) direction = 0;
	if(button_on&PSP_CTRL_RIGHT)  direction = 1;
	if(direction>-2)
	{
		void (* func)();
		switch( menu_list[menu_sel].type )
		{
		case TMENU_FUNCTION:
			func = (void *)(menu_list[menu_sel].value);
			func();
			break;
		case TMENU_FUNCTION_VALUE:
			func = (void *)(menu_list[menu_sel].value);
			func( FUNC_CHANGE_VALUE , direction );
			break;
		case TMENU_FUNCTION_EXIT:
//			func = (void *)menu_list[menu_sel].value;
			exit_func = (void *)menu_list[menu_sel].value;
//			func();
			return 2;
			break;	
		case TMENU_EXIT:
			return 1;
			break;
		case TMENU_SWITCH:
			*(int *)menu_list[menu_sel].value = ( *(int *)menu_list[menu_sel].value )? 0:1;
			break;
		}
	}
	return 0; // continue
}

void manager();
extern int stop_flag ;

int display_menu(const Menu_pack *menu_list, int *switch_list, int exit_type)
{
	int i;
	void *(* surrent_get)();
	int menu_mode = 0;

	int menu_max_cnt = 0;
	while( menu_list[ menu_max_cnt ].path != NULL)
	{
		menu_max_cnt++;
	}

	if( switch_list != NULL )
	{
		menu_sel = 0;
		while( switch_list[menu_sel] == 1 )
		{
			menu_sel++;
		}
	}

	while( stop_flag == 0 )
	{
		if( sceDisplayWaitVblankStart() < 0)
			break;

		if( blit_setup() >= 0)
		{
			const char *msg;
			int *pointer = (pwidth==720)?(int *)xyPoint:(int *)xyPoint2;

			blit_set_color(0x00ffffff,0x8000ff00);
			blit_string( pointer[0], pointer[1] , menu_title );

			int y = (pointer[5])*8;
			for(i=0;i < menu_max_cnt;i++)
			{
//				if( main_menu_switch[i] == 0 )
				if( !switch_list || switch_list[i] == 0 )
				{
					y += 8;
//					blit_set_color( 0xffffff , ( i== menu_sel)? 0x00ff8080 : 0xc00000ff );
					blit_set_color( 0xffffff , ( i== menu_sel)? 0x00ff8080 : back_color );

					int draw_type = menu_list[i].draw_type;
					if( draw_type == 0 )
						blit_string( pointer[4] , y , menu_list[i].path );
					else if( draw_type == 1 )
						blit_string_ctr( y , menu_list[i].path );
					else if( draw_type == 2 )
					{
						char write_buff[64];
						scePaf_sprintf(write_buff,"%s    ->", menu_list[i].path);
						blit_string( pointer[4] , y , write_buff );
					}

					switch( menu_list[i].type )
					{
					case TMENU_SWITCH:
//						msg = (char *)(enable_disable[*(int*)menu_list[i].value]);
						msg = (char *)( (*(int*)menu_list[i].value) == 0)? vshmenu_str.Enabled:vshmenu_str.Disabled ;
						blit_string( (pointer[6] * 8) + 128 , y ,msg);
						break;
					case TMENU_FUNCTION_VALUE:
						surrent_get = menu_list[i].value;
						msg = surrent_get(FUNC_GET_STR);
						blit_string( (pointer[6] * 8) + 128 , y ,msg);
						break;
					}
				}
			}

//			blit_set_color(0x00ffffff,0x00000000);
		}

		static int stop_stock = 0;
		int res;

		// menu controll
		switch(menu_mode)
		{
/*		case 0:	
			if( (cur_buttons & ALL_CTRL) == 0)
			{
				menu_mode = 1;
			}
			break;
*/		
		case 0:

			if(	exit_type &&
				cur_buttons&PSP_CTRL_RTRIGGER &&
				cur_buttons&PSP_CTRL_LTRIGGER &&
				cur_buttons&PSP_CTRL_START)
			{
				manager();
			}
			else if( ( res = menu_ctrl( menu_list, switch_list, menu_max_cnt )) != 0)
			{
				if( exit_type )
				{
					stop_stock = res;
					menu_mode = 1;
				}
				else
				{
					return res;
				}
			}
			break;
		case 1: // exit waiting 
			// exit menu
			if( (cur_buttons & ALL_CTRL) == 0)
			{
				stop_flag = stop_stock;
			}
			break;
		}
	}

	return stop_flag;
}
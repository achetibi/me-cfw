
#include <pspkernel.h>
//#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "text.h"

extern u32 PSP_CTRL_BACK;
extern u32 PSP_CTRL_ENTER;

char recovery_title[] = "X.XX Recovery Menu\n ";
/*
static const char *back_end[] =
{
	recovery_str.Back,
	recovery_str.Exit
};

static char *disable_enable[] =
{
	(recovery_str.Enabled),
	(recovery_str.Disabled)
};
*/

u32 select_color = 0x0000FF00;

char stat_line[70];

void draw_init()
{
	memset(stat_line,'*',sizeof(stat_line));
	stat_line[68]='\0';

	u32 version = sceKernelDevkitVersion();
	recovery_title[0] = ( version >> 24) + '0';
	recovery_title[2] = ((version >> 16)& 0xFF ) + '0';
	recovery_title[3] = ((version >> 8 )& 0xFF ) + '0';
}

static void draw_statbox()
{
	myDebugScreenSetXY(0, 26);
	myDebugScreenSetTextColor(select_color, 0);
	printf(stat_line);
}

static void draw_title(const char *sub_title)
{
//	if (no >= title_s)
//		no=0;

	myDebugScreenSetXY(1, 0);
	myDebugScreenSetTextColor(select_color, 0);
	printf( recovery_title );

	if( sub_title)
	{
		printf( sub_title );
	}
	printf( "\n\n\n\n" );

}

void send_msg(const char *msg )
{
	myDebugScreenSetXY(0, 27);
	myDebugScreenSetTextColor(normal_color , 0);
	printf("\n> %s",msg);
}

void set_select_color(u32 color )
{
	select_color = color;
}


int DrawMenu(Menu_pack *menu_desu ,u8 sub ,const char *sub_title)
{
	u8 i;
	short cpos=0;
	u32 pad_data;
	char* (*surrent_get)() ;
	int spuit;

	int menus = 0;

	while( menu_desu[menus].path )
	{
		menus ++;
	}

	if(sub != 0 )
		cpos = -1;

#define PAGE_MAX_ROW 18
	int page_max_cnt = (menus - 1)/PAGE_MAX_ROW;
	int page_current_cnt = 0;
	int display_page_row;

	myDebugScreenClear();

	display_page_row = limit2( menus - page_current_cnt * PAGE_MAX_ROW , 0 , PAGE_MAX_ROW );
//	display_page_row = (menus - page_current_cnt * PAGE_MAX_ROW) % PAGE_MAX_ROW;

	while(1)
	{
		int display_item_offset = page_current_cnt * PAGE_MAX_ROW;

		draw_title( sub_title );
		myDebugScreenSetXY(0, 4);

		if(sub != 0 )
		{
//			(cpos==-1) ? myDebugScreenSetTextColor(select_color) : myDebugScreenSetTextColor(normal_color);
//			myDebugScreenSetTextColor((cpos == -1) ? select_color : normal_color, (cpos == -1) ? 0x181818 : 0);
			myDebugScreenSetTextColor((cpos == -1) ? select_color : normal_color, 0);
			printf("%s %s\n\n", (cpos == -1) ? ">" : " ", recovery_str.Back );
		}

		cpos += display_item_offset;
		for(i = display_item_offset; i < display_item_offset + display_page_row; i++)
		{
//			(cpos==i) ? myDebugScreenSetTextColor(select_color):myDebugScreenSetTextColor(normal_color);

			if(cpos == i)
				myDebugScreenClearLineWithColor( 0x181818 );

				myDebugScreenSetTextColor((cpos == i) ? select_color : normal_color, (cpos == i) ? 0x181818 : 0);

				int type = menu_desu[i].type;
				switch( type )
				{
				case TMENU_SWITCH:
				case TMENU_SWITCH2:
					spuit = *(int *)menu_desu[i].value & 1;

					if( type == TMENU_SWITCH2){
						spuit = 1 - spuit;
					}

					printf("%s %s \t(%s: %s)\n",(cpos==i) ? ">" : " ", menu_desu[i].path, recovery_str.Currently 
						,( spuit != 0)? recovery_str.Enabled:recovery_str.Disabled );

					break;
/*				case TMENU_SWITCH2:
					printf("%s %s (currently:%s)\n",(cpos==i) ? ">" : " ",menu_desu[i].path, *disable_enable[*(int *)menu_desu[i].value & 1]);

					break;
*/				case TMENU_SUB_MENU:
					printf("%s %s ->\n",(cpos==i) ? ">" : " ", menu_desu[i].path);
					break;
				case TMENU_FUNCTION:
					printf("%s %s\n",(cpos==i) ? ">" : " ", menu_desu[i].path);
					break;
				case TMENU_FUNCTION_DISPLAY:
					surrent_get = (void *)(menu_desu[i].value);
					printf("%s %s\n",(cpos==i) ? ">" : " ", surrent_get(FUNC_GET_STR) );
					break;
				case TMENU_FUNCTION_VALUE_DISPLAY:
					surrent_get = (void *)(menu_desu[i].value);
					printf("%s %s \t(%s: %s)\n",(cpos==i) ? ">" : " ", surrent_get(FUNC_GET_STR), recovery_str.Currently, surrent_get(FUNC_GET_STR2) );
					break;
				case TMENU_FUNCTION_VALUE:

					surrent_get = (void *)(menu_desu[i].value);
					printf("%s %s \t(%s: %s)\n",(cpos==i) ? ">" : " ", menu_desu[i].path, recovery_str.Currently, surrent_get(FUNC_GET_STR) );
					break;
				default:
					break;
				}	
		}

		cpos -= display_item_offset;

		if(sub == 0)
		{
//			(cpos == (display_page_row) ) ? myDebugScreenSetTextColor(select_color):myDebugScreenSetTextColor(normal_color);
			myDebugScreenClearLineWithColor( (cpos == (display_page_row) ) ? 0x181818 : 0 );
			myDebugScreenSetTextColor((cpos == (display_page_row) ) ? select_color : normal_color, (cpos == (display_page_row) ) ? 0x181818 : 0);
//			myDebugScreenSetTextColor((cpos == (display_page_row) )? select_color:normal_color, 0 );
			printf("%s %s\n",( cpos == display_page_row ) ? ">" : " ", recovery_str.Exit );
		}
		else
		{
			i -= display_item_offset;	

			while(i < PAGE_MAX_ROW)
			{
				i++;
				printf("\n");
			}
		}

		if( page_max_cnt != 0 )
		{
			myDebugScreenSetTextColor(normal_color, 0);
			myDebugScreenSetXY(0, 25);	printf("<- L");
			myDebugScreenSetXY(64, 25);	printf("R ->");
		}

		draw_statbox();


//			printf("cpos%d",cpos);	
		while(1){
			sceKernelDelayThread(100);
			pad_data=ctrlRead();

			if (pad_data != 0)
				break;
		}

		if (pad_data & PSP_CTRL_UP)
			cpos -= 1;

		if (pad_data & PSP_CTRL_DOWN)
			cpos += 1;

		if( page_max_cnt != 0 )
		{
			if( pad_data & PSP_CTRL_LTRIGGER ) 
			{
				page_current_cnt = limit( page_current_cnt - 1 , 0 , page_max_cnt );
				display_page_row = limit2( menus - page_current_cnt * PAGE_MAX_ROW , 0 , PAGE_MAX_ROW );
//				display_page_row = (menus - page_current_cnt * PAGE_MAX_ROW)%PAGE_MAX_ROW;

			}
			else if( pad_data & PSP_CTRL_RTRIGGER )
			{
				page_current_cnt = limit( page_current_cnt + 1 , 0 , page_max_cnt );
				display_page_row = limit2( menus - page_current_cnt * PAGE_MAX_ROW , 0 , PAGE_MAX_ROW );
//				display_page_row = (menus - page_current_cnt * PAGE_MAX_ROW)%PAGE_MAX_ROW;

			}
		}

		if(sub != 0 )
		{
			cpos = limit( cpos , -1 , display_page_row -1 );
		}
		else
		{
			cpos = limit( cpos , 0 , display_page_row );
		}

			//if (pad_data & PSP_CTRL_LEFT)
			//if (pad_data & PSP_CTRL_RIGHT)
		
		if (pad_data & ( PSP_CTRL_ENTER | PSP_CTRL_LEFT | PSP_CTRL_RIGHT) )
		{
			int direction = (pad_data & PSP_CTRL_LEFT) ? -1 : 1;

			if(cpos == display_page_row)
			{
			//	exiting();
				break;
			}
			else if(cpos == -1)
			{
			//	backing();
				break;
			}

			cpos += display_item_offset;
			switch( menu_desu[ cpos ].type)
			{
			case TMENU_SWITCH:
			case TMENU_SWITCH2:
/*				spuit = ( *(int *)menu_desu[cpos].value )? 0:1;

				char *bridge;
				if( spuit )
					bridge = recovery_str.Enabled;
				else
					bridge = recovery_str.Disabled;
*/
				*(int *)menu_desu[cpos].value = ( *(int *)menu_desu[cpos].value )? 0:1;
				send_msg( recovery_str.Setting);
				printf(" ...");
				sceKernelDelayThread(1*1000*1000);
				myDebugScreenClear();

				break;
			case TMENU_SUB_MENU:
				DrawMenu((Menu_pack *)(menu_desu[cpos].value) , 1 , menu_desu[cpos].path );

				send_msg( recovery_str.Back);
				sceKernelDelayThread(1*1000*1000);
				myDebugScreenClear();
				break;
			case TMENU_FUNCTION:
				if( direction > 0)
				{
					surrent_get = (void *)(menu_desu[cpos].value);
					surrent_get();
				}
				break;
		
			case TMENU_FUNCTION_DISPLAY:
				if( direction < 0)
					break;

			case TMENU_FUNCTION_VALUE_DISPLAY:
			case TMENU_FUNCTION_VALUE:

				surrent_get = (void *)(menu_desu[cpos].value);
				surrent_get( FUNC_CHANGE_VALUE , direction );
				sceKernelDelayThread(1*1000*1000);
				myDebugScreenClear();
				break;
			}

			cpos -= display_item_offset;
		}
		else if( pad_data & (PSP_CTRL_BACK | PSP_CTRL_SELECT) )
		{
			break;
		}
	}

	return 0;
}
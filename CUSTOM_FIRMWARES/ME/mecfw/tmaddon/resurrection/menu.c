
#include <pspkernel.h>
//#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"

char recovery_title[] = "X.XX Unbricker Menu\n ";

u32 select_color = 0x0000FF00;

char stat_line[70];

u32 ctrlRead()
{
	static u32 cur_buttons = 0xFFFFFFFF;
	SceCtrlData ctl;
	u32 buttons;
	u32 button_on;

	sceCtrlPeekBufferPositive(&ctl, 1);

	buttons     = ctl.Buttons;
	button_on   = ~cur_buttons & buttons;
	cur_buttons = buttons;

	return button_on;
}

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

static void draw_title(char *sub_title)
{
	myDebugScreenSetXY(1, 0);
	myDebugScreenSetTextColor(select_color, 0);
	printf( recovery_title );

	if( sub_title)
	{
		printf( sub_title );
	}
	printf( "\n\n\n\n" );
}

int ofw;

int DrawMenu(Menu_pack *menu_desu ,u8 sub ,char *sub_title)
{
	u8 i;
	short cpos = 0;
	u32 pad_data;
	char* (*surrent_get)() ;
	int menus = 0;

	while( menu_desu[menus].path )
	{
		menus ++;
	}

#define PAGE_MAX_ROW 18
	int page_current_cnt = 0;
	int display_page_row;

	myDebugScreenClear();

	display_page_row = limit2( menus - page_current_cnt * PAGE_MAX_ROW , 0 , PAGE_MAX_ROW );

	while(1)
	{
		int display_item_offset = page_current_cnt * PAGE_MAX_ROW;

		draw_title( sub_title );
		myDebugScreenSetXY(0, 4);

		cpos += display_item_offset;
		for(i = display_item_offset; i < display_item_offset + display_page_row; i++)
		{
			if(cpos == i)
				myDebugScreenClearLineWithColor(0x181818);

			myDebugScreenSetTextColor((cpos == i) ? select_color : normal_color, (cpos == i) ? 0x181818 : 0);

				int type = menu_desu[i].type;
				switch( type )
				{
					case TMENU_SUB_MENU:
					case TMENU_FUNCTION:
					case TMENU_FUNCTION_RET:
						printf("%s %s\n",(cpos == i) ? ">" : " ", menu_desu[i].path);
					break;
				}
		}

		cpos -= display_item_offset;
		
		i -= display_item_offset;	

		while( i < PAGE_MAX_ROW)
		{
			i++;
			printf("\n");
		}

		draw_statbox();

		while(1)
		{
			sceKernelDelayThread(100);
			pad_data = ctrlRead();

			if (pad_data != 0)
				break;
		}

		if (pad_data & PSP_CTRL_UP)
			cpos -= 1;

		if (pad_data & PSP_CTRL_DOWN)
			cpos += 1;

		cpos = limit( cpos , 0 , display_page_row -1 );

		if (pad_data & PSP_CTRL_CROSS)
		{
			int ret = 0;

			if (menu_desu[ cpos ].value == NULL)
			{
				break;
			}

			cpos += display_item_offset;

			switch( menu_desu[ cpos ].type)
			{
				case TMENU_SUB_MENU:
					ofw = cpos;
					DrawMenu((Menu_pack *)(menu_desu[cpos].value) , 1 , menu_desu[cpos].path );

					sceKernelDelayThread(100);
					myDebugScreenClear();
				break;

				case TMENU_FUNCTION:
						surrent_get = (void *)(menu_desu[cpos].value);
						surrent_get();
				break;

				case TMENU_FUNCTION_RET:
						surrent_get = (void *)(menu_desu[cpos].value);
						ret = (int)surrent_get();
				break;
			}

			cpos -= display_item_offset;
			
			if (ret < 0) break;
		}
	}

	return 0;
}
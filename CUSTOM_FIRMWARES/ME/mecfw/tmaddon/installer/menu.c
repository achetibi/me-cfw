#include <pspkernel.h>
#include <pspctrl.h>

#include "main.h"

char title[] = "Time Machine Addon Installer by Rahim-US\n";

u32 select_color = 0x00FFFF00;

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

static void draw_title()
{
	pspDebugScreenSetXY(1, 0);
	pspDebugScreenSetTextColor(0x000000FF);
	pspDebugScreenPrintf( title );

	pspDebugScreenPrintf( "\n\n" );
}

int DrawMenu(Menu_pack *menu_desu ,u8 sub)
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

	pspDebugScreenClear();

	display_page_row = limit2( menus - page_current_cnt * PAGE_MAX_ROW , 0 , PAGE_MAX_ROW );

	while(1)
	{
		int display_item_offset = page_current_cnt * PAGE_MAX_ROW;

		draw_title();
		pspDebugScreenSetXY(0, 3);

		cpos += display_item_offset;
		for(i = display_item_offset; i < display_item_offset + display_page_row; i++)
		{
			pspDebugScreenSetTextColor((cpos == i) ? select_color : normal_color);

				int type = menu_desu[i].type;
				switch( type )
				{
					case TMENU_FUNCTION:
						pspDebugScreenPrintf("%s %s\n",(cpos == i) ? ">" : " ", menu_desu[i].path);
					break;
				}
		}

		cpos -= display_item_offset;
		
		i -= display_item_offset;	

		while( i < PAGE_MAX_ROW)
		{
			i++;
			pspDebugScreenPrintf("\n");
		}

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
			cpos += display_item_offset;

			switch( menu_desu[ cpos ].type)
			{
				case TMENU_FUNCTION:
						surrent_get = (void *)(menu_desu[cpos].value);
						surrent_get();
				break;
			}

			cpos -= display_item_offset;
		}
	}

	return 0;
}
#include <pspkernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Define the module info section */
PSP_MODULE_INFO("unbricker", 0x800 , 1, 0);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR( PSP_THREAD_ATTR_VSH );//| PSP_THREAD_ATTR_VFPU 

#include "menu_struct.h"

int main()
{
	char sub_title[] = "Unbricker (base fw -- X.XX)";

	myDebugScreenInit();

	draw_init();

	if (mallocate_buffer() < 0)
	{
		printf("buffer init error\n");
		TestME();
	}

	u32 version = sceKernelDevkitVersion();
	sub_title[22] = ( version >> 24) + '0';
	sub_title[24] = ((version >> 16)& 0xFF ) + '0';
	sub_title[25] = ((version >> 8 )& 0xFF ) + '0';

	DrawMenu( menu_main , 0 , sub_title );

	free_buffer();

	return 0;
}

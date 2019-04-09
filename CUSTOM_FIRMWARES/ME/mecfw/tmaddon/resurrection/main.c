#include <pspkernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <psppower.h>

/* Define the module info section */
PSP_MODULE_INFO("unbricker", 0x800 , 1, 0);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR( PSP_THREAD_ATTR_VSH );//| PSP_THREAD_ATTR_VFPU 

#include "menu_struct.h"

u32 wakeup_timer()
{
    scePowerTick(0);
    return 10*1000*1000;
}

int main()
{
    SceUID id;
    //char sub_title[] = "Unbricker (base fw -- X.XX)";

    id = sceKernelCreateVTimer("", NULL);
    if (id >= 0) {
        sceKernelStartVTimer(id);
        sceKernelSetVTimerHandlerWide(id, 2*1000*1000, wakeup_timer, NULL);
    }

    myDebugScreenInit();

    draw_init();

    if (mallocate_buffer() < 0)
    {
        myDebugScreenSetTextColor(0x00FFFFFF, 0x00000000);
        printf("buffer init error\n");
        sceKernelDelayThread(5 * 1000 * 1000);
        sctrlKernelExitVSH(NULL);
        //TestME();
    }

    //u32 version = sceKernelDevkitVersion();
    //sub_title[22] = ( version >> 24) + '0';
    //sub_title[24] = ((version >> 16)& 0xFF ) + '0';
    //sub_title[25] = ((version >> 8 )& 0xFF ) + '0';
#if _PSP_FW_VERSION == 660
    DrawMenu( menu_main , 0 , "Unbricker (base fw -- 6.60)" );
#elif _PSP_FW_VERSION == 661
    DrawMenu( menu_main , 0 , "Unbricker (base fw -- 6.61)" );
#else
#error main.c
#endif

    free_buffer();

	return 0;
}

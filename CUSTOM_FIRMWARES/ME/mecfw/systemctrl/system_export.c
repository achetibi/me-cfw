#include <pspsdk.h>
#include <pspkernel.h>
#include <pspmodulemgr_kernel.h>

#include "main.h"

char *on_reboot_after = NULL;
void *on_reboot_buf = NULL;
int on_reboot_size = 0;
int on_reboot_flag = 0;

void sctrlHENLoadModuleOnReboot(char *module_after, void *buf, int size, int flags)
{
	on_reboot_after =module_after;
	on_reboot_buf   =buf;
	on_reboot_size  =size;
	on_reboot_flag  =flags;
}

//hook sceKernelLoadModuleMs2
void SystemCtrlForKernel_07232EA5(void *func)
{
	SceModule2 *mod = sceKernelFindModuleByName("sceInit");
	if( mod != NULL)
	{
		u32 data = mod->text_addr;
		mod = sceKernelFindModuleByName("sctrlHoroscope");
		if( mod != NULL )
		{	
			void ( *sctrlHoroscopeHookLoadModuleMs2)(void *, u32 ) = search_module_export( mod, "horoscope_driver", 0x0FB6B018 );
			sctrlHoroscopeHookLoadModuleMs2( func, data );
		}
	}
}

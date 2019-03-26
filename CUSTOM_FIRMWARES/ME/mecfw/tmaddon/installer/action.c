#include <pspkernel.h>
#include <pspctrl.h>

#include "main.h"
#include "dcman.h"
#include "kubridge.h"

void sub_01544 ( void )
{
	pspDebugScreenPrintf("\nPress any key to return.\n");

	while (1)
	{
		sceKernelDelayThread(10000);
		u32 pad_data = ctrlRead();

		if (pad_data != 0)
			break;
	}
}

void Exit()
{
	sctrlKernelExitVSH(NULL);
}

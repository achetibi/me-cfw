#include <pspkernel.h>

#include "action.h"
#include "data/tm_ipl_bin.h"

#define STR_OK "OK\n"

char buffer[0x20000];

int sub_0033C ()
{
	int ret = 0;

	SceUID fd = sceIoOpen("msstor:", PSP_O_RDONLY, 0777);
	if (fd < 0)
	{
		ret = -1;
	}
	else
	{
		int size = sceIoRead(fd, buffer, 512);
		sceIoClose(fd);

		if (size != 512)
		{
			ret = -1;
		}
		else
		{
			ret = ((((((buffer[457] << 0x00000008) + buffer[456]) << 0x00000008) + buffer[455]) << 0x00000008) + buffer[454]) + 0xFFFFFFF0;
		}
	}

	return ret;
}

void install_iplloader()
{
	pspDebugScreenClear();
	pspDebugScreenSetTextColor(0x0000FF00);

	pspDebugScreenPrintf("Checking space... ");
	
	int ret = sub_0033C();
	if (ret < 0)
	{
		pspDebugScreenPrintf("\nError: Unable to get ipl space.\n");
		return;
	}
	else
	{


		if (ret < 0x00000020)
		{
			pspDebugScreenPrintf("\nError: Too few free sectors (%d). 32 at least required.\n", ret);
			return;
		}
		else
		{
			pspDebugScreenPrintf(STR_OK);
			pspDebugScreenPrintf("Free sector %d(size: %d)\n", ret, (ret << 0x00000009));
			pspDebugScreenPrintf("Writting ipl... \n");

			SceUID fd = sceIoOpen("msstor:", PSP_O_WRONLY, 0777);
			if (fd < 0)
			{
				pspDebugScreenPrintf("\nError: Unable to open Output Device.\n");
				return;
			}
			else
			{
				sceIoLseek(fd, 0x2000, 0);
				sceIoWrite(fd, ipl, sizeof(ipl));
				sceIoClose(fd);

				pspDebugScreenPrintf(STR_OK);
			}
		}
	}

	sub_01544();
	pspDebugScreenClear();

	return;
}
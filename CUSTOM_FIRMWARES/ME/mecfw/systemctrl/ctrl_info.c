#include <pspsdk.h>
#include <pspkernel.h>

int sctrlSEGetVersion()
{
	int v = 0x00010023;
#if _PSP_FW_VERSION == 639
	v -= 0xDF80;
#endif

	return v;
}

int	sctrlHENIsSE()
{
	return 1;
}

int	sctrlHENIsDevhook()
{
	return 0;
}

int sctrlHENGetVersion()
{
	return 0x00001000;
}

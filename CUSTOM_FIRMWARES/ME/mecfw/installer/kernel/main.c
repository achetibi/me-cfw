#include <pspsdk.h>
#include <pspkernel.h>

PSP_MODULE_INFO("bridge", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

u32 sceSysreg_driver_E2A5D1EE(void);
u32 sceSysregGetTachyonVersion(void)
{
	int k1 = pspSdkSetK1(0);
	int ret = sceSysreg_driver_E2A5D1EE();
	pspSdkSetK1(k1);

	return ret;
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}

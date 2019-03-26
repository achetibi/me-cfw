#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <pspdebug.h>
#include <pspinit.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <systemctrl_me.h>

static int (*_scePowerSetClockFrequency)(int pllfreq, int cpufreq, int busfreq);
static int (*_KDebugForKernel_93F5D2A6)(int unk);

int SysMemUserForUser_1B4217BC_Patched(u32 fw_version)
{
	int (*sceKernelSetCompiledSdkVersion)(u32 fw_version);
	sceKernelSetCompiledSdkVersion = (void*)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x7591C7DB);

	return (*sceKernelSetCompiledSdkVersion)(fw_version);
}

int scePower_469989AD_Patched(int pllfreq, int cpufreq, int busfreq)
{
	int ret = (*_scePowerSetClockFrequency)(pllfreq, cpufreq, busfreq);
	return ret;
}

int scePower_A85880D0_Patched(void)
{
	int ret = sceKernelGetModel();

	if (ret == 0)
	{
		ret = (*_KDebugForKernel_93F5D2A6)(11);
		if (ret == 1)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
	}
	else
	{
		ret = 1;
	}

	return ret;
}

void patch_for_old_fw(SceModule *mod)
{
#if _PSP_FW_VERSION != 660
	sctrlHookImportByNid(mod, "SysMemUserForUser", 0x358CA1BB, &SysMemUserForUser_1B4217BC_Patched, 1);
#endif

#if _PSP_FW_VERSION == 620
	sctrlHookImportByNid(mod, "SysMemUserForUser", 0x1B4217BC, &SysMemUserForUser_1B4217BC_Patched, 1);
	sctrlHookImportByNid(mod, "scePower", 0x469989AD, &scePower_469989AD_Patched, 1);
	sctrlHookImportByNid(mod, "scePower", 0xA85880D0, &scePower_A85880D0_Patched, 1);
#endif
}

void get_functions_for_old_fw(void)
{
	_scePowerSetClockFrequency = (void *)sctrlHENFindFunction("scePower_Service", "scePower", 0x737486F2);
	_KDebugForKernel_93F5D2A6 = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "KDebugForKernel", 0x93F5D2A6);
}

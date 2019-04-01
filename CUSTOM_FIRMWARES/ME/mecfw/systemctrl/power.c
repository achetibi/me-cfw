#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <psppower.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

static int g_fake_pll = 222;
static int g_fake_cpu = 222;
static int g_fake_bus = 111;

typedef struct _PowerFuncRedir {
	u32 nid;
	void *fp;
} PowerFuncRedir;

int (* scePowerSetClockFrequency_k)(int cpufreq, int ramfreq, int busfreq);

__attribute__((noinline)) u32 FindPowerFunction(u32 nid)
{
	return FindProc("scePower_Service", "scePower", nid);
}

 __attribute__((noinline)) u32 FindPowerDriverFunction(u32 nid)
{
	return FindProc("scePower_Service", "scePower_driver", nid);
}

int	sctrlHENSetSpeed(int cpu,int bus)
{	
	scePowerSetClockFrequency_k = (void *)FindPowerFunction(0x545A7F3C);
	return scePowerSetClockFrequency_k( cpu, cpu ,bus);
}

static int scePowerGetPllClockFrequencyIntFaked(void)
{

	return g_fake_pll;
}

static float scePowerGetPllClockFrequencyFloatFaked(void)
{

	return g_fake_pll;
}

static int scePowerGetCpuClockFrequencyFaked(void)
{

	return g_fake_cpu;
}

static float scePowerGetCpuClockFrequencyFloatFaked(void)
{

	return g_fake_cpu;
}

static int scePowerGetBusClockFrequencyFaked(void)
{

	return g_fake_bus;
}

static float scePowerGetBusClockFrequencyFloatFaked(void)
{

	return g_fake_bus;
}

static int scePowerSetClockFrequencyFaked(int pllfreq, int cpufreq, int busfreq)
{
	g_fake_pll = pllfreq;
	g_fake_cpu = cpufreq;
	g_fake_bus = busfreq;

	return 0;
}

static int scePowerSetCpuClockFrequencyFaked(int cpufreq)
{
	g_fake_cpu = cpufreq;

	return 0;
}

static int scePowerSetBusClockFrequencyFaked(int busfreq)
{
	g_fake_bus = busfreq;
	
	return 0;
}

static PowerFuncRedir g_power_func_redir[] = {
	{ 0x737486F2, scePowerSetClockFrequencyFaked },
	{ 0x545A7F3C, scePowerSetClockFrequencyFaked },
	{ 0xEBD177D6, scePowerSetClockFrequencyFaked },
	{ 0xA4E93389, scePowerSetClockFrequencyFaked },
	{ 0x469989AD, scePowerSetClockFrequencyFaked },
	{ 0x843FBF43, scePowerSetCpuClockFrequencyFaked },
	{ 0xB8D7B3FB, scePowerSetBusClockFrequencyFaked },
	{ 0x34F9C463, scePowerGetPllClockFrequencyIntFaked },
	{ 0xEA382A27, scePowerGetPllClockFrequencyFloatFaked },
	{ 0xFEE03A2F, scePowerGetCpuClockFrequencyFaked },
	{ 0xB1A52C83, scePowerGetCpuClockFrequencyFloatFaked },
	{ 0x478FE6F5, scePowerGetBusClockFrequencyFaked },
	{ 0x9BADB3EB, scePowerGetBusClockFrequencyFloatFaked },
};

void SetSpeed(int cpu, int bus)
{
	if(cpu==20 || cpu==75 || cpu==100 || cpu==133 || cpu==166 || cpu==200 || cpu==222 || cpu==266 || cpu==300 || cpu==333)
	{
		scePowerSetClockFrequency_k = (void *)FindPowerFunction(0x737486F2);//scePowerSetClockFrequency
		scePowerSetClockFrequency_k(cpu, cpu, bus);

		int intr = sceKernelInitKeyConfig();
		if(intr != PSP_INIT_KEYCONFIG_VSH)
		{
			MAKE_DUMMY_FUNCTION0((u32)scePowerSetClockFrequency_k);

			int i;
			u32 fp;

//			for(i=0;i<sizeof(cpu_nid_list)/sizeof(u32);i++)
			for(i=0;i<sizeof(g_power_func_redir)/sizeof(u32);i++)
			{
//				u32 patch_addr = (u32)FindPowerFunction(cpu_nid_list[i]);
//				MAKE_DUMMY_FUNCTION0( patch_addr );
				fp = FindPowerFunction(g_power_func_redir[i].nid);
				if(fp != 0) {
					REDIRECT_FUNCTION(fp, g_power_func_redir[i].fp);
				}
			}

			ClearCaches();
		}
	}

}


#if PSP_MODEL != 0
int (* scePowerIsBatteryCharging_k)() = NULL;
int (* scePowerBatteryDisableUsbCharging_k)() = NULL;
int (* scePowerBatteryEnableUsbCharging_k)() = NULL;

u32 UsbChargePatch()
{
	static int connected = 0;
	static int usb_swap = 0;

	int r = scePowerIsBatteryCharging_k();

	//if already charging
	if(r != 0) return 2*1000*1000;

	if(connected == 1)//connected
	{
		if(usb_swap)
		{
			scePowerBatteryDisableUsbCharging_k(0);
		}

		connected = 0;

		usb_swap= (usb_swap > 0)? 0: 1 ;

		return 5*1000*1000;
	}


	if( scePowerBatteryEnableUsbCharging_k(1) >= 0)
		connected = 1;

	return 15*1000*1000;
}

static void initUsbPowerFunc(void)
{
	scePowerIsBatteryCharging_k			= (void *)FindPowerFunction(0x1E490401);
	scePowerBatteryDisableUsbCharging_k	= (void *)FindPowerDriverFunction(0x90285886);
	scePowerBatteryEnableUsbCharging_k	= (void *)FindPowerDriverFunction(0x733F973B);
}

void UsbChargeStart()
{
	SceUID id =sceKernelCreateVTimer("",NULL);
	if(id >= 0) {
		initUsbPowerFunc();

		sceKernelStartVTimer(id);
		sceKernelSetVTimerHandlerWide(id ,5*1000*1000, UsbChargePatch ,NULL);

		SceModule2 *mod =sceKernelFindModuleByName("sceUSB_Driver");
		if(mod)
		{
			u32 text_addr =mod->text_addr;

#if _PSP_FW_VERSION == 620
#define UsbChargePatchAddr1	0x00008FF0//OK
#define UsbChargePatchAddr2	0x00008FE8//OK

#elif _PSP_FW_VERSION == 639
#define UsbChargePatchAddr1	0x00009048
#define UsbChargePatchAddr2	0x00009050

#elif _PSP_FW_VERSION == 660
#define UsbChargePatchAddr1	0x00008FE8
#define UsbChargePatchAddr2	0x00008FF0

#elif _PSP_FW_VERSION == 661
#define UsbChargePatchAddr1	0x00008FE8
#define UsbChargePatchAddr2	0x00008FF0

#endif

			MAKE_DUMMY_FUNCTION0(text_addr + UsbChargePatchAddr1 );//scePowerBatteryEnableUsbCharging
			MAKE_DUMMY_FUNCTION0(text_addr + UsbChargePatchAddr2 );//scePowerBatteryDisableUsbCharging
		}
	}
}
#endif
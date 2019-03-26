#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspreg.h>
#include <pspthreadman_kernel.h>
#include <psploadcore.h>
#include <pspctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "systemctrl_me.h"

extern int (*vshmenu_kernel)();
extern SEConfig config;

int vctrlVSHRegisterVshMenu(int (* ctrl)())
{
	int k1 = pspSdkSetK1(0);
	vshmenu_kernel=(void *)(0x80000000 | (u32)ctrl);
	pspSdkSetK1(k1);
	return 0;
}

void vctrlVSHSetSpeed(int cpu, int bus )
{
	int k1 = pspSdkSetK1(0);
	config.vshcpuspeed = cpu;
	config.vshbusspeed = bus;
	sctrlSESetConfig(&config);
	pspSdkSetK1(k1);
}

void vctrlVSHGetSpeed(int *cpu, int *bus )
{
	int k1 = pspSdkSetK1(0);
	*cpu = config.vshcpuspeed;
	*bus = config.vshbusspeed;
	pspSdkSetK1(k1);
}

void vctrlVSHSetRecovery()
{
//	int k1 = pspSdkSetK1(0);
	sctrlSESetBootConfFileIndex( 4 );
//	pspSdkSetK1(k1);
}
/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pspkernel.h>
#include <pspreg.h>
#include <stdio.h>
#include <string.h>
#include <systemctrl_me.h>
#include <pspsysmem_kernel.h>
#include <pspsysevent.h>
#include <pspumd.h>
#include <psprtc.h>

#include "inferno.h"

PSP_MODULE_INFO("Inferno_driver", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

u32 module_sdk_version =  0x03060010;

u32 psp_model;
u32 psp_fw_version;

extern int sceKernelApplicationType(void);
extern int sceKernelSetQTGP3(void *unk0);

// 00002790
const char *g_iso_fn = NULL;

// 0x00002248
u8 g_umddata[16] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

const u16 cache_sels[] = { 64, 128, 160, 196, 256, 320, 384, 512, 640, 768 };

extern int power_event_handler(int ev_id, char *ev_name, void *param, int *result);

PspSysEventHandler g_power_event = {
	.size = sizeof(g_power_event),
	.name = "infernoSysEvent",
	.type_mask = 0x00FFFF00, // both suspend / resume
	.handler = &power_event_handler,
};

// 00000090
int setup_umd_device(void)
{
	g_iso_fn = sctrlSEGetUmdFile();
	infernoSetDiscType(sctrlSEGetDiscType());
	int ret = sceIoAddDrv(&g_iodrv);

	if(ret < 0) {
		return ret;
	}

	sceKernelSetQTGP3(g_umddata);

	return 0;
}

// 00001514
int init_inferno(void)
{
	g_drive_status = PSP_UMD_INITING;
	g_umd_cbid = -1;
	g_umd_error_status = 0;
	g_drive_status_evf = sceKernelCreateEventFlag("SceMediaManUser", 0x201, 0, NULL);
	sceKernelRegisterSysEventHandler(&g_power_event);

	return MIN(g_drive_status_evf, 0);
}

// 0x00000000
int module_start(SceSize args, void* argp)
{
	int ret, key_config;
	SEConfig config;

	psp_model = sceKernelGetModel();
	psp_fw_version = sceKernelDevkitVersion();

	key_config = sceKernelApplicationType();
	sctrlSEGetConfig(&config);

	if(config.iso_cache && psp_model != 0 && key_config == PSP_INIT_KEYCONFIG_GAME) {
		int bufsize;

		bufsize = config.iso_cache_total_size * 1024 * 1024 / cache_sels[config.iso_cache_num];
		
		if((bufsize % 512) != 0) {
			bufsize &= ~(512-1);
		}

		if(bufsize == 0) {
			bufsize = 512;
		}

		infernoCacheSetPolicy(config.iso_cache_policy);
		infernoCacheInit(bufsize, cache_sels[config.iso_cache_num]);
	}

	ret = setup_umd_device();

	if(ret < 0) {
		return ret;
	}

	ret = init_inferno();

	return MIN(ret, 0);
}

// 0x0000006C
int module_stop(SceSize args, void *argp)
{
	sceIoDelDrv("umd");
	sceKernelDeleteEventFlag(g_drive_status_evf);
	sceKernelUnregisterSysEventHandler(&g_power_event);

	return 0;
}

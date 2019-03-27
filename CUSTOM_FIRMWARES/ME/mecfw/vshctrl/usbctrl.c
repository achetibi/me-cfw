#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman.h>
#include <psploadcore.h>
#include <pspctrl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <pspusb.h>
#include <pspusbdevice.h>

#include "systemctrl_me.h"

#include "main.h"

static u8 usb_umd=0;
static SceUID usbdfd = -1;

extern u8 iso_mount;
extern SEConfig config;


int PatchUsbStart(const char* driverName, int size, void *args)
{
	int k1 = pspSdkSetK1(0);

	if (strcmp(driverName, "USBStor_Driver") == 0)
	{
		int usb_status = config.usbdevice;

		if(usb_status > 0 && usb_status < 6)
		{
			if(iso_mount && config.usbdevice == 5 )
			{
				unmount_iso();
				usb_umd=1;
			}
			
			usbdfd = sceKernelLoadModule("flash0:/kd/usbdev.prx",0,0);
			if( usbdfd <0 )
			{
				//usbdfd = sceKernelLoadModule("ms0:/cfw/usbdev.prx",0,0);
				usbdfd = sceKernelLoadModule("ms0:/seplugins/usbdev.prx",0,0);
			}

			if(usbdfd >= 0)
			{
				if( sceKernelStartModule(usbdfd,0,0,0,0) >=0)
				{
					if(pspUsbDeviceSetDevice( usb_status - 1, ( config.usbprotect == 0 )? 1: 0, 0) < 0)
					{
						pspUsbDeviceFinishDevice();
					}
				}
			}
		}
		
	}

	pspSdkSetK1(k1);
	return sceUsbStart(driverName, size,args);
}

int PatchUsbStop(const char* driverName, int size, void *args)
{
	int r = sceUsbStop(driverName,size,args);

	int k1 = pspSdkSetK1(0);

	if (strcmp(driverName, "USBStor_Driver") == 0)
	{
		if(usbdfd >= 0 )
		{
			pspUsbDeviceFinishDevice();

			if(sceKernelStopModule(usbdfd,0,0,0,0) >= 0)
			{
				sceKernelUnloadModule(usbdfd);
				usbdfd=-1;
			}

			if(usb_umd)
			{
				//remount iso
				sctrlSESetDiscOut(1);
				char *umd = sctrlSEGetUmdFile();

				sctrlSEMountUmdFromFile(umd, 0 , 0 );

				iso_mount = 1;	
				usb_umd = 0;
			}
		}
	}

	pspSdkSetK1(k1);
	return r;
}

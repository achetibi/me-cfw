// thanks to nem for sharing the battery stuff on ps2dev
// thanks to Chilly Willy for the source this is based on
//
// overwrite serial number to 0xffffffff to make service mode battery
// overwrite serial number to 0x00000000 to make autoboot mode battery
// serial number is stored at address 0x07 and address 0x09

#include <pspsdk.h>
#include <pspkernel.h>
#include <string.h>
#include <pspsysmem.h>

#define sceSysregGetTachyonVersion sceSysreg_driver_E2A5D1EE
#define sceSysconGetBaryonVersion sceSyscon_driver_7EC5A957

PSP_MODULE_INFO("bridge", 0x1006, 1, 3);
PSP_MAIN_THREAD_ATTR(0);

// syscon function for all versions used by silversprings' code
u32 sceSysconCmdExec(void* param, int unk);

int sceSysregGetTachyonVersion(void);
int sceSysconGetBaryonVersion(u32* val);

int read_only = 0;

// thanks silverspring for the reversed functions
u32 write_eeprom(u8 addr, u16 data)
{
	int res;
	u8 param[0x60];

	if (addr > 0x7F)
		return(0x80000102);

	param[0x0C] = 0x73; // write battery eeprom command
	param[0x0D] = 5;	// tx packet length

	// tx data
	param[0x0E] = addr;
	param[0x0F] = data;
	param[0x10] = data>>8;
	
	res = sceSysconCmdExec(param, 0);
	
	if (res < 0)
		return(res);

	return 0;
}

u32 read_eeprom(u8 addr)
{
	int res;
	u8 param[0x60];

	if (addr > 0x7F)
		return(0x80000102);
		
	param[0x0C] = 0x74; // read battery eeprom command
	param[0x0D] = 3;	// tx packet length

	// tx data
	param[0x0E] = addr;
		
	res = sceSysconCmdExec(param, 0);

	if (res < 0)
		return(res);

	// rx data
	return((param[0x21]<<8) | param[0x20]);
}
// end functions from silverspring

int CheckModel() 
{
	u32 k1, tmp;
	k1 = pspSdkSetK1(0);
	sceSysconGetBaryonVersion(&tmp);
	if(( sceSysregGetTachyonVersion() >= 0x00500000) && ( tmp > 0x0022B200))
	{
		read_only = 1;
	}

	pspSdkSetK1(k1);
	return read_only;
}	
/*
// 0x8025000x are module errors
// anything above 0x80250080 are hw errors
// 0x802500b8 - read error on a fake battery
int errCheck(u32 data)
{
	if ((data & 0x80250000) == 0x80250000) // old way (data & 0x80000000) <- checking only for -1 wrather than specifically a syscon error
		return -1;
	else if (data & 0xffff0000)
		return ((data & 0xffff0000)>>16);
	return 0;
}
*/

// read the serial data locations
int ReadBatterySerial(u16* pdata)
{
	int err = 0;
	u32 k1, data;

	k1 = pspSdkSetK1(0);

	// serial number is stored at address 0x07 and address 0x09
	err = data = read_eeprom(0x07); // lower 16bit
	pdata[0] = (data &0xffff);	
	err |= data = read_eeprom(0x09); // upper 16bit
	pdata[1] =  (data &0xffff);

	if ((err & 0x80250000) == 0x80250000)
	{
		err = 0x80250000;
	}
	else
	{
		err = 0;
	}

	pspSdkSetK1(k1);
	return err;
}

// write the serial data locations
int WriteBatterySerial(u16* pdata)
{
	int err = 0;
	if( read_only )
	{
		err = -1;
	}
	else
	{
		u32 k1 = pspSdkSetK1(0);
		err = write_eeprom(0x07, pdata[0]); // lower 16bit
		err = write_eeprom(0x09, pdata[1]); // lower 16bit
		pspSdkSetK1(k1);
	}
	return err;
}


int module_start(SceSize args, void *argp)
{
	CheckModel();
	return 0;
}

int module_stop()
{
	return 0;
}

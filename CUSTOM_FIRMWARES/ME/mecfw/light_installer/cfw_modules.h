
#include "../recovery/recovery_bin.h"
#include "../galaxy/pulsar_bin.h"
#include "../inferno/inferno_bin.h"
#include "../march33/isotope_bin.h"
#include "../horoscope/horoscope_bin.h"
#include "../vshctrl/vshctrl_lite_bin.h"
//#include "../vshmenu_src/satellite_bin.h"
#include "../vshmenu_new/satellite_bin.h"
#include "../idcanager/idmanager_bin.h"

//#include "../installer/modules/usbdev.h"
#include "../usbdevice/usbdevice_bin.h"

#include "../rebooter/reboot_01g_bin.h"
#include "../rebooter/reboot_02g_bin.h"
//#include "../rebooter/reboot_03g_bin.h"
#include "../rebooter/reboot_05g_bin.h"


#include "../dax9660/dax9660.h"

#include "../pspbtjnf/pspbtjnf_01g.h"
#include "../systemctrl/systemctrl_01g_bin.h"

#include "../pspbtjnf/pspbtjnf_02g.h"
#include "../systemctrl/systemctrl_02g_bin.h"

#include "../pspbtjnf/pspbtjnf_03g.h"
#include "../systemctrl/systemctrl_03g_bin.h"

#include "../pspbtjnf/pspbtjnf_04g.h"

#include "../pspbtjnf/pspbtjnf_05g.h"
#include "../systemctrl/systemctrl_05g_bin.h"

#if _PSP_FW_VERSION == 639 || _PSP_FW_VERSION == 660
#include "../pspbtjnf/pspbtjnf_07g.h"
#include "../pspbtjnf/pspbtjnf_09g.h"
#endif

#if _PSP_FW_VERSION == 660
#include "../pspbtjnf/pspbtjnf_11g.h"
#endif

typedef struct Module
{
	char *dst;
	void* buffer;
	u32 size;
} Module;

/*
#if _PSP_FW_VERSION == 639
#define REBOOTER_PATH "reboot63x.prx"
#elif _PSP_FW_VERSION == 660
#define REBOOTER_PATH "rebooter.prx"
#endif
*/
#define MODULES_COUNT 3
#define COMMON_MODULES_COUNT 10


Module common_modules[COMMON_MODULES_COUNT] = 
{
	{ "flash0:/kd/usbdev.prx", usbdevice, sizeof(usbdevice) },
	{ "flash0:/vsh/module/satellite.prx", satellite, sizeof(satellite) },
	{ "flash0:/vsh/module/recovery.prx", recovery, sizeof(recovery) },
//	{ "flash0:/kd/popcorn.prx", popcorn, sizeof(popcorn) },
	{ "flash0:/kd/dax9660.prx", dax9660, sizeof(dax9660) },
	{ "flash0:/kd/inferno.prx", inferno, sizeof(inferno) },
	{ "flash0:/kd/isotope.prx", isotope, sizeof(isotope) },
	{ "flash0:/kd/idmanager.prx", idmanager, sizeof(idmanager) },
	{ "flash0:/kd/pulsar.prx", pulsar, sizeof(pulsar) },
	{ "flash0:/kd/horoscope.prx", horoscope, sizeof(horoscope) },
//	{ "flash0:/vsh/module/velf.prx", velf, sizeof(velf) },
//	{ "flash1:/prometheus_key.txt", prometheus_key, sizeof(prometheus_key) }
	{ "flash0:/kd/vshctrl_02g.prx", vshctrl, sizeof(vshctrl) }
};

Module modules_01g[MODULES_COUNT] =
{
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_01g , sizeof(reboot_01g) },
	{ "flash0:/kd/rebooter.prx", reboot_01g , sizeof(reboot_01g) },
	{ "flash0:/kd/pspbtjnf.bin", pspbtjnf_01g , sizeof(pspbtjnf_01g) },
	{ "flash0:/kd/systemctrl_01g.prx", systemctrl_01g, sizeof(systemctrl_01g) }
};

Module modules_02g[MODULES_COUNT] =
{
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_02g , sizeof(reboot_02g) },
	{ "flash0:/kd/rebooter.prx", reboot_02g , sizeof(reboot_02g) },
	{ "flash0:/kd/pspbtjnf_02g.bin", pspbtjnf_02g, sizeof(pspbtjnf_02g) },
	{ "flash0:/kd/systemctrl_02g.prx", systemctrl_02g, sizeof(systemctrl_02g) }
};

Module modules_03g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_03g , sizeof(reboot_03g) },
	{ "flash0:/kd/pspbtjnf_03g.bin", pspbtjnf_03g, sizeof(pspbtjnf_03g) },
	{ "flash0:/kd/systemctrl_03g.prx", systemctrl_03g, sizeof(systemctrl_03g) }
};

Module modules_04g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_03g , sizeof(reboot_03g) },
	{ "flash0:/kd/pspbtjnf_04g.bin", pspbtjnf_04g, sizeof(pspbtjnf_04g) },
	{ "flash0:/kd/systemctrl_03g.prx", systemctrl_03g, sizeof(systemctrl_03g) }
};

Module modules_05g[MODULES_COUNT] =
{
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_03g , sizeof(reboot_03g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_05g , sizeof(reboot_05g) },
	{ "flash0:/kd/rebooter.prx", reboot_05g , sizeof(reboot_05g) },
	{ "flash0:/kd/pspbtjnf_05g.bin", pspbtjnf_05g, sizeof(pspbtjnf_05g) },
	{ "flash0:/kd/systemctrl_05g.prx", systemctrl_05g, sizeof(systemctrl_05g) }
};

#if _PSP_FW_VERSION == 639 || _PSP_FW_VERSION == 660
Module modules_07g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_03g , sizeof(reboot_03g) },
	{ "flash0:/kd/pspbtjnf_07g.bin", pspbtjnf_07g, sizeof(pspbtjnf_07g) },
	{ "flash0:/kd/systemctrl_03g.prx", systemctrl_03g, sizeof(systemctrl_03g) }
};

Module modules_09g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_03g , sizeof(reboot_03g) },
	{ "flash0:/kd/pspbtjnf_09g.bin", pspbtjnf_09g, sizeof(pspbtjnf_09g) },
	{ "flash0:/kd/systemctrl_03g.prx", systemctrl_03g, sizeof(systemctrl_03g) }
};
#endif

#if _PSP_FW_VERSION == 660
Module modules_11g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_02g , sizeof(reboot_02g) },
//	{ "flash0:/kd/"REBOOTER_PATH, reboot_03g , sizeof(reboot_03g) },
	{ "flash0:/kd/pspbtjnf_11g.bin", pspbtjnf_11g, sizeof(pspbtjnf_11g) },
	{ "flash0:/kd/systemctrl_03g.prx", systemctrl_03g, sizeof(systemctrl_03g) }
};
#endif

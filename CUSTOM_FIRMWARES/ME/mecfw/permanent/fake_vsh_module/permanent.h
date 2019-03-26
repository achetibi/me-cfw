
typedef struct Module
{
	char *file_dest;
} Module;

#define MODULES_COUNT 3
#define COMMON_MODULES_COUNT 9

Module common_modules[COMMON_MODULES_COUNT] = 
{
	{ "flash0:/kd/usbdev.prx" },
	{ "flash0:/vsh/module/satellite.prx" },
	{ "flash0:/vsh/module/recovery.prx" },
	{ "flash0:/kd/dax9660.prx" },
	{ "flash0:/kd/isotope.prx" },
	{ "flash0:/kd/idmanager.prx" },
	{ "flash0:/kd/pulsar.prx" },
	{ "flash0:/kd/horoscope.prx" },
	{ "flash0:/kd/vshctrl_02g.prx" }
};

Module modules_01g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx" },
	{ "flash0:/kd/pspbtjnf.bin" },
	{ "flash0:/kd/systemctrl_01g.prx" }
};

Module modules_02g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx" },
	{ "flash0:/kd/pspbtjnf_02g.bin" },
	{ "flash0:/kd/systemctrl_02g.prx" }
};

Module modules_03g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx" },
	{ "flash0:/kd/pspbtjnf_03g.bin" },
	{ "flash0:/kd/systemctrl_03g.prx" }
};

Module modules_04g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx" },
	{ "flash0:/kd/pspbtjnf_04g.bin" },
	{ "flash0:/kd/systemctrl_03g.prx" }
};

Module modules_05g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx" },
	{ "flash0:/kd/pspbtjnf_05g.bin" },
	{ "flash0:/kd/systemctrl_05g.prx" }
};
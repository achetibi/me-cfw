
// 01g
#include "../../rebooter/reboot_01g_bin.h"
#include "../../rebooter/reboot_01g_p_bin.h"
#include "../../pspbtjnf/pspbtjnf_01g.h"
#include "../../pspbtjnf/pspbtjnf_01g_p.h"
#include "../../systemctrl/systemctrl_01g_bin.h"
#include "../../systemctrl/systemctrl_01g_p_bin.h"
//02g
#include "../../rebooter/reboot_02g_bin.h"
#include "../../rebooter/reboot_02g_p_bin.h"
#include "../../pspbtjnf/pspbtjnf_02g.h"
#include "../../pspbtjnf/pspbtjnf_02g_p.h"
#include "../../systemctrl/systemctrl_02g_bin.h"
#include "../../systemctrl/systemctrl_02g_p_bin.h"

//03g
//#include "../../rebooter/reboot_02g_bin.h"
//#include "../../rebooter/reboot_02g_p_bin.h"
#include "../../pspbtjnf/pspbtjnf_03g.h"
#include "../../pspbtjnf/pspbtjnf_03g_p.h"
#include "../../systemctrl/systemctrl_03g_bin.h"
#include "../../systemctrl/systemctrl_03g_p_bin.h"

//04g
//#include "../../rebooter/reboot_02g_bin.h"
//#include "../../rebooter/reboot_02g_p_bin.h"
#include "../../pspbtjnf/pspbtjnf_04g.h"
#include "../../pspbtjnf/pspbtjnf_04g_p.h"
//#include "../../systemctrl/systemctrl_03g_bin.h"
//#include "../../systemctrl/systemctrl_03g_p_bin.h"

//05g
#include "../../rebooter/reboot_05g_bin.h"
#include "../../rebooter/reboot_05g_p_bin.h"
#include "../../pspbtjnf/pspbtjnf_05g.h"
#include "../../pspbtjnf/pspbtjnf_05g_p.h"
#include "../../systemctrl/systemctrl_05g_bin.h"
#include "../../systemctrl/systemctrl_05g_p_bin.h"

typedef struct Module
{
	char *file_dest;		// destination file name
	void *file_perm;		// permanent patch pspbtjnf buffer
	void *file_orig;		// original 6.20 lme pspbtjnf buffer
	u32   file_perm_size;	// permanent patch pspbtjnf buffer size
	u32   file_orig_size;	// original 6.20 lme pspbtjnf buffer size
} Module;

#define MODULES_COUNT 3

Module modules_01g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_01g_p, reboot_01g, sizeof(reboot_01g_p), sizeof(reboot_01g) },
	{ "flash0:/kd/pspbtjnf.bin", pspbtjnf_01g_p, pspbtjnf_01g, sizeof(pspbtjnf_01g_p), sizeof(pspbtjnf_01g) },
	{ "flash0:/kd/systemctrl_01g.prx", systemctrl_01g_p, systemctrl_01g, sizeof(systemctrl_01g_p), sizeof(systemctrl_01g) },
};

Module modules_02g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_02g_p, reboot_02g, sizeof(reboot_02g_p), sizeof(reboot_02g) },
	{ "flash0:/kd/pspbtjnf_02g.bin", pspbtjnf_02g_p, pspbtjnf_02g, sizeof(pspbtjnf_02g_p), sizeof(pspbtjnf_02g) },
	{ "flash0:/kd/systemctrl_02g.prx", systemctrl_02g_p, systemctrl_02g, sizeof(systemctrl_02g_p), sizeof(systemctrl_02g) },
};

Module modules_03g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_02g_p, reboot_02g, sizeof(reboot_02g_p), sizeof(reboot_02g) },
	{ "flash0:/kd/pspbtjnf_03g.bin", pspbtjnf_03g_p, pspbtjnf_03g, sizeof(pspbtjnf_03g_p), sizeof(pspbtjnf_03g) },
	{ "flash0:/kd/systemctrl_03g.prx", systemctrl_03g_p, systemctrl_03g, sizeof(systemctrl_03g_p), sizeof(systemctrl_03g) },
};

Module modules_04g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_02g_p, reboot_02g, sizeof(reboot_02g_p), sizeof(reboot_02g) },
	{ "flash0:/kd/pspbtjnf_04g.bin", pspbtjnf_04g_p, pspbtjnf_04g, sizeof(pspbtjnf_04g_p), sizeof(pspbtjnf_04g) },
	{ "flash0:/kd/systemctrl_03g.prx", systemctrl_03g_p, systemctrl_03g, sizeof(systemctrl_03g_p), sizeof(systemctrl_03g) },
};

Module modules_05g[MODULES_COUNT] =
{
	{ "flash0:/kd/rebooter.prx", reboot_05g_p, reboot_05g, sizeof(reboot_05g_p), sizeof(reboot_05g) },
	{ "flash0:/kd/pspbtjnf_05g.bin", pspbtjnf_05g_p, pspbtjnf_05g, sizeof(pspbtjnf_05g_p), sizeof(pspbtjnf_05g) },
	{ "flash0:/kd/systemctrl_05g.prx", systemctrl_05g_p, systemctrl_05g, sizeof(systemctrl_05g_p), sizeof(systemctrl_05g) },
};

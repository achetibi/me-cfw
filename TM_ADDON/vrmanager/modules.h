#ifndef __MODULES_H__
#define __MODULES_H__

#include "../includes/modules/DC500/iop.h"
#include "../includes/modules/DC500/vlf.h"
#include "../includes/modules/DC500/intraFont.h"
#include "../includes/modules/DC500/vunbricker_en.h"
#include "../includes/kpspident/kpspident.h"
/* DC8 */
#include "../includes/modules/DC500/config.h"
#include "../includes/modules/DC500/dcman.h"
#include "../includes/modules/DC500/galaxy.h"
#include "../includes/modules/DC500/idcanager.h"
#include "../includes/modules/DC500/idsregeneration.h"
#include "../includes/modules/DC500/ipl.h"
#include "../includes/modules/DC500/ipl_01g.h"
#include "../includes/modules/DC500/ipl_02g.h"
#include "../includes/modules/DC500/ipl_update.h"
#include "../includes/modules/DC500/lflash_fdisk.h"
#include "../includes/modules/DC500/march33.h"
#include "../includes/modules/DC500/popcorn.h"
#include "../includes/modules/DC500/pspbtdnf.h"
#include "../includes/modules/DC500/pspbtdnf_02g.h"
#include "../includes/modules/DC500/pspbtjnf.h"
#include "../includes/modules/DC500/pspbtjnf_02g.h"
#include "../includes/modules/DC500/pspbtknf.h"
#include "../includes/modules/DC500/pspbtknf_02g.h"
#include "../includes/modules/DC500/pspbtlnf.h"
#include "../includes/modules/DC500/pspbtlnf_02g.h"
#include "../includes/modules/DC500/recovery.h"
#include "../includes/modules/DC500/resurrection.h"
#include "../includes/modules/DC500/satelite.h"
#include "../includes/modules/DC500/systemctrl.h"
#include "../includes/modules/DC500/systemctrl_02g.h"
#include "../includes/modules/DC500/tmctrl500.h"
#include "../includes/modules/DC500/tm_ipl.h"
#include "../includes/modules/DC500/usbdevice.h"
#include "../includes/modules/DC500/vshctrl.h"
/* DC9 */
#include "../includes/modules/DC660/config.h"
#include "../includes/modules/DC660/dax9660.h"
#include "../includes/modules/DC660/dcman.h"
#include "../includes/modules/DC660/horoscope.h"
#include "../includes/modules/DC660/iop.h"
#include "../includes/modules/DC660/ipl.h"
#include "../includes/modules/DC660/ipl_01g.h"
#include "../includes/modules/DC660/ipl_02g.h"
#include "../includes/modules/DC660/isotope.h"
#include "../includes/modules/DC660/libpsardumper.h"
#include "../includes/modules/DC660/pspbtdnf.h"
#include "../includes/modules/DC660/pspbtdnf_02g.h"
#include "../includes/modules/DC660/pspbtjnf.h"
#include "../includes/modules/DC660/pspbtjnf_02g.h"
#include "../includes/modules/DC660/pspdecrypt.h"
#include "../includes/modules/DC660/pulsar.h"
#include "../includes/modules/DC660/recovery.h"
#include "../includes/modules/DC660/resurrection.h"
#include "../includes/modules/DC660/satellite.h"
#include "../includes/modules/DC660/systemctrl_01g.h"
#include "../includes/modules/DC660/systemctrl_02g.h"
#include "../includes/modules/DC660/tmctrl660.h"
#include "../includes/modules/DC660/usbdev.h"
#include "../includes/modules/DC660/vshctrl_02g.h"

struct eboots {
	int version;
	int size;
	u8 md5[16];
} eboots[] = {
	{ 0x01F4, 0x019D9E95, {0x60, 0xAA, 0x03, 0x56, 0xD4, 0xC6, 0x6F, 0x58, 0x9B, 0x71, 0xCF, 0xC9, 0xAB, 0xC0, 0x87, 0x13} },//500
	{ 0x0294, 0x01F19005, {0x2C, 0xA6, 0x4D, 0x59, 0xDC, 0xF4, 0x8F, 0x45, 0xFB, 0x99, 0xB4, 0x00, 0xA5, 0x86, 0xB3, 0x95} } //660
};

struct DC8_dirs {
	char *dir_name;
} DC8_dirs[] = {
	{ "ms0:/TM" },
	{ "ms0:/TM/XXX" },
	{ "ms0:/TM/XXX/data" },
	{ "ms0:/TM/XXX/data/cert" },
	{ "ms0:/TM/XXX/dic" },
	{ "ms0:/TM/XXX/font" },
	{ "ms0:/TM/XXX/gps" },
	{ "ms0:/TM/XXX/kd" },
	{ "ms0:/TM/XXX/kd/resource" },
	{ "ms0:/TM/XXX/net" },
	{ "ms0:/TM/XXX/net/http" },
	{ "ms0:/TM/XXX/registry" },
	{ "ms0:/TM/XXX/vsh" },
	{ "ms0:/TM/XXX/vsh/etc" },
	{ "ms0:/TM/XXX/vsh/module" },
	{ "ms0:/TM/XXX/vsh/resource" },
	{ "ms0:/TM/XXX/vsh/theme" },
	{ "ms0:/TM/XXX/codepage" }
};

struct DC8_Files {
	int version;
	char *filepath;
	u8 *filebuffer;
	unsigned int *filesize;
} DC8_Files[] = {
	{ 0, "ms0:/TM/500/config.se",					config_DC500,				&size_config_DC500 },
	{ 0, "ms0:/TM/500/ipl.bin",						ipl_DC500,					&size_ipl_DC500 },
	{ 0, "ms0:/TM/500/ipl_01g.bin",					ipl_01g_DC500,				&size_ipl_01g_DC500 },
	{ 0, "ms0:/TM/500/ipl_02g.bin",					ipl_02g_DC500,				&size_ipl_02g_DC500 },
	{ 0, "ms0:/TM/500/tmctrl500.prx",				tmctrl500_DC500,			&size_tmctrl500_DC500 },
	{ 0, "ms0:/TM/500/kd/dcman.prx",				dcman_DC500,				&size_dcman_DC500 },
	{ 0, "ms0:/TM/500/kd/galaxy.prx",				galaxy_DC500,				&size_galaxy_DC500 },
	{ 0, "ms0:/TM/500/kd/idcanager.prx",			idcanager_DC500,			&size_idcanager_DC500 },
	{ 0, "ms0:/TM/500/kd/idsregeneration.prx",		idsregeneration_DC500,		&size_idsregeneration_DC500 },
	{ 0, "ms0:/TM/500/kd/iop.prx",					iop_DC500,					&size_iop_DC500 },
	{ 0, "ms0:/TM/500/kd/ipl_update.prx",			ipl_update_DC500,			&size_ipl_update_DC500 },
	{ 0, "ms0:/TM/500/kd/kpspident.prx",			kpspident,					&size_kpspident },
	{ 0, "ms0:/TM/500/kd/lflash_fdisk.prx",			lflash_fdisk_DC500,			&size_lflash_fdisk_DC500 },
	{ 0, "ms0:/TM/500/kd/march33.prx",				march33_DC500,				&size_march33_DC500 },
	{ 0, "ms0:/TM/500/kd/popcorn.prx",				popcorn_DC500,				&size_popcorn_DC500 },
	{ 0, "ms0:/TM/500/kd/pspbtdnf.bin",				pspbtdnf_DC500,				&size_pspbtdnf_DC500 },
	{ 0, "ms0:/TM/500/kd/pspbtdnf_02g.bin",			pspbtdnf_02g_DC500,			&size_pspbtdnf_02g_DC500 },
	{ 0, "ms0:/TM/500/kd/pspbtjnf.bin",				pspbtjnf_DC500,				&size_pspbtjnf_DC500 },
	{ 0, "ms0:/TM/500/kd/pspbtjnf_02g.bin",			pspbtjnf_02g_DC500,			&size_pspbtjnf_02g_DC500 },
	{ 0, "ms0:/TM/500/kd/pspbtknf.bin",				pspbtknf_DC500,				&size_pspbtknf_DC500 },
	{ 0, "ms0:/TM/500/kd/pspbtknf_02g.bin",			pspbtknf_02g_DC500,			&size_pspbtknf_02g_DC500 },
	{ 0, "ms0:/TM/500/kd/pspbtlnf.bin",				pspbtlnf_DC500,				&size_pspbtlnf_DC500 },
	{ 0, "ms0:/TM/500/kd/pspbtlnf_02g.bin",			pspbtlnf_02g_DC500,			&size_pspbtlnf_02g_DC500 },
	{ 0, "ms0:/TM/500/kd/resurrection.prx",			resurrection_DC500,			&size_resurrection_DC500 },
	{ 0, "ms0:/TM/500/kd/systemctrl.prx",			systemctrl_DC500,			&size_systemctrl_DC500 },
	{ 0, "ms0:/TM/500/kd/systemctrl_02g.prx",		systemctrl_02g_DC500,		&size_systemctrl_02g_DC500 },
	{ 0, "ms0:/TM/500/kd/usbdevice.prx",			usbdevice_DC500,			&size_usbdevice_DC500 },
	{ 0, "ms0:/TM/500/kd/vshctrl.prx",				vshctrl_DC500,				&size_vshctrl_DC500 },
	{ 0, "ms0:/TM/500/vsh/module/intraFont.prx",	intraFont_DC500,			&size_intraFont_DC500 },
	{ 0, "ms0:/TM/500/vsh/module/recovery.prx",		recovery_DC500,				&size_recovery_DC500 },
	{ 0, "ms0:/TM/500/vsh/module/satelite.prx",		satelite_DC500,				&size_satelite_DC500 },
	{ 0, "ms0:/TM/500/vsh/module/vlf.prx",			vlf_DC500,					&size_vlf_DC500 },

	{ 1, "ms0:/TM/660/config.me",					config_DC660,				&size_config_DC660 },
	{ 1, "ms0:/TM/660/ipl.bin",						ipl_DC660,					&size_ipl_DC660 },
	{ 1, "ms0:/TM/660/ipl_01g.bin",					ipl_01g_DC660,				&size_ipl_01g_DC660 },
	{ 1, "ms0:/TM/660/ipl_02g.bin",					ipl_02g_DC660,				&size_ipl_02g_DC660 },
	{ 1, "ms0:/TM/660/tmctrl660.prx",				tmctrl660_DC660,			&size_tmctrl660_DC660 },
	{ 1, "ms0:/TM/660/kd/dax9660.prx",				dax9660_DC660,				&size_dax9660_DC660 },
	{ 1, "ms0:/TM/660/kd/dcman.prx",				dcman_DC660,				&size_dcman_DC660 },
	{ 1, "ms0:/TM/660/kd/horoscope.prx",			horoscope_DC660,			&size_horoscope_DC660 },
	{ 1, "ms0:/TM/660/kd/iop.prx",					iop_DC660,					&size_iop_DC660 },
	{ 1, "ms0:/TM/660/kd/ipl_update.prx",			ipl_update_DC500,			&size_ipl_update_DC500 },
	{ 1, "ms0:/TM/660/kd/isotope.prx",				isotope_DC660,				&size_isotope_DC660 },
	{ 1, "ms0:/TM/660/kd/libpsardumper.prx",		libpsardumper_DC660,		&size_libpsardumper_DC660 },
	{ 1, "ms0:/TM/660/kd/pspbtdnf.bin",				pspbtdnf_DC660,				&size_pspbtdnf_DC660 },
	{ 1, "ms0:/TM/660/kd/pspbtdnf_02g.bin",			pspbtdnf_02g_DC660,			&size_pspbtdnf_02g_DC660 },
	{ 1, "ms0:/TM/660/kd/pspbtjnf.bin",				pspbtjnf_DC660,				&size_pspbtjnf_DC660 },
	{ 1, "ms0:/TM/660/kd/pspbtjnf_02g.bin",			pspbtjnf_02g_DC660,			&size_pspbtjnf_02g_DC660 },
	{ 1, "ms0:/TM/660/kd/pspdecrypt.prx",			pspdecrypt_DC660,			&size_pspdecrypt_DC660 },
	{ 1, "ms0:/TM/660/kd/pulsar.prx",				pulsar_DC660,				&size_pulsar_DC660 },
	{ 1, "ms0:/TM/660/kd/systemctrl_01g.prx",		systemctrl_01g_DC660,		&size_systemctrl_01g_DC660 },
	{ 1, "ms0:/TM/660/kd/systemctrl_02g.prx",		systemctrl_02g_DC660,		&size_systemctrl_02g_DC660 },
	{ 1, "ms0:/TM/660/kd/usbdev.prx",				usbdev_DC660,				&size_usbdev_DC660 },
	{ 1, "ms0:/TM/660/kd/vshctrl_02g.prx",			vshctrl_02g_DC660,			&size_vshctrl_02g_DC660 },
	{ 1, "ms0:/TM/660/vsh/module/recovery.prx",		recovery_DC660,				&size_recovery_DC660 },
	{ 1, "ms0:/TM/660/vsh/module/resurrection.prx",	resurrection_DC660,			&size_resurrection_DC660 },
	{ 1, "ms0:/TM/660/vsh/module/satellite.prx",	satellite_DC660,			&size_satellite_DC660 }
};

#endif


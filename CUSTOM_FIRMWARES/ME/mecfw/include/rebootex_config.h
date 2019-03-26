#ifndef __REBOOTEX_CONFIG_H__
#define __REBOOTEX_CONFIG_H__

#define REBOOTEX_FILELEN_MAX	0x00000050
#define REBOOTEX_PARAM_OFFSET	0x88FB0000

typedef struct RebootexParam {
    char	file[REBOOTEX_FILELEN_MAX];	// 0x00
	u32		config[0x70/4];				// 0x50
	int		reboot_index;				// 0xC0
	int		mem2;
	int		mem9;
	int		k150_flag;
	void*	on_reboot_after;
	void*	on_reboot_buf;
	int		on_reboot_size;
	int		on_reboot_flag;
} RebootexParam;

//	*(u32 *)0x88FB00F0 = size_systemctrl;//

#endif


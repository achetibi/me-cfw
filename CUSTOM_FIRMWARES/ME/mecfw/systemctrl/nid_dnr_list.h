
#if _PSP_FW_VERSION == 620
static Nid_list DNR_sysclib[] = {
	{ 0x909C228B , (u32)my_setjmp },//setjmp
	{ 0x18FE80DB , (u32)my_longjmp },//longjmp
	{ 0x87F8D2DA , (u32)my_strtok	},//strtok
	{ 0x1AB53A58 , (u32)my_strtok_r },//strtok_r
	{ 0x89B79CB1 , (u32)my_strcspn	},//strcspn
	{ 0x62AE052F , (u32)my_strspn	},//strspn
};

static Nid_list DNR_loadcore[] = {
	{ 0x2952F5AC , 0x00007F2C },//sceKernelDcacheWBinvAll
	{ 0xD8779AC6 , 0x00007F6C },//sceKernelIcacheClearAll
};

static Nid_list DNR_syscon[] = {
	{ 0xC8439C57 , 0x00002C64 },//sceSysconPowerStandby
};

#elif _PSP_FW_VERSION == 639
static Nid_list DNR_sysclib[] = {
	{ 0x909C228B , 0x88002E88 },//setjmp
	{ 0x18FE80DB , 0x88002EC4 },//longjmp
	{ 0x87F8D2DA , (u32)my_strtok	},//strtok
	{ 0x1AB53A58 , (u32)my_strtok_r },//strtok_r
	{ 0x89B79CB1 , (u32)my_strcspn	},//strcspn
	{ 0x62AE052F , (u32)my_strspn	},//strspn
};

static Nid_list DNR_loadcore[] = {
	{ 0x2952F5AC , 0x0000778C },//sceKernelDcacheWBinvAll
	{ 0xD8779AC6 , 0x000077CC },//sceKernelIcacheClearAll
};

static Nid_list DNR_syscon[] = {
	{ 0xC8439C57 , 0x00002C6C },//sceSysconPowerStandby
};

#elif _PSP_FW_VERSION == 660
static Nid_list DNR_sysclib[] = {
	{ 0x909C228B , (u32)my_setjmp },//setjmp
	{ 0x18FE80DB , (u32)my_longjmp },//longjmp
	{ 0x87F8D2DA , (u32)my_strtok	},//strtok
	{ 0x1AB53A58 , (u32)my_strtok_r },//strtok_r
	{ 0x89B79CB1 , (u32)my_strcspn	},//strcspn
	{ 0x62AE052F , (u32)my_strspn	},//strspn
};

static Nid_list DNR_loadcore[] = {
	{ 0x2952F5AC , 0x0000744C },//sceKernelDcacheWBinvAll
	{ 0xD8779AC6 , 0x0000748C },//sceKernelIcacheClearAll
};

static Nid_list DNR_syscon[] = {
	{ 0xC8439C57 , 0x00002D08 },//sceSysconPowerStandby
};

#elif _PSP_FW_VERSION == 661
static Nid_list DNR_sysclib[] = {
	{ 0x909C228B , (u32)my_setjmp },//setjmp
	{ 0x18FE80DB , (u32)my_longjmp },//longjmp
	{ 0x87F8D2DA , (u32)my_strtok	},//strtok
	{ 0x1AB53A58 , (u32)my_strtok_r },//strtok_r
	{ 0x89B79CB1 , (u32)my_strcspn	},//strcspn
	{ 0x62AE052F , (u32)my_strspn	},//strspn
};

static Nid_list DNR_loadcore[] = {
	{ 0x2952F5AC , 0x0000744C },//sceKernelDcacheWBinvAll
	{ 0xD8779AC6 , 0x0000748C },//sceKernelIcacheClearAll
};

static Nid_list DNR_syscon[] = {
	{ 0xC8439C57 , 0x00002D08 },//sceSysconPowerStandby
};

#else
#error nid resolver.c
#endif


#define DNR_FILES		(3)

Module_list DNR_table[DNR_FILES] =
{
	{ "SysclibForKernel"	, DNR_sysclib,	sizeof(DNR_sysclib)/sizeof(Nid_list) /*DNR_sysclib_no*/	},
	{ "LoadCoreForKernel"	, DNR_loadcore,	sizeof(DNR_loadcore)/sizeof(Nid_list) /*DNR_loadcore_no*/},
	{ "sceSyscon_driver"	, DNR_syscon,	sizeof(DNR_syscon)/sizeof(Nid_list) /*DNR_syscon_no*/	}
};

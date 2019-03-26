#if _PSP_FW_VERSION == 620
u32 loadexec_patch_list[] = {
	0x00002CD8,//
	0x00002D24,//
	0x00002350,//
	0x00002394,//
	0x00001D54,// LoadExecForKernel_783EA19F
};
u32 loadexec_patch_list_05g[] = {	
	0x00002F28,//
	0x00002F74,//
	0x000025A4,//
	0x000025E8,//
	0x00001F88,// LoadExecForKernel_783EA19F
	0x00002078,// LoadExecForKernel_41D9398E
};

#elif _PSP_FW_VERSION == 639
u32 loadexec_patch_list[] = {
	0x00002D5C,
	0x00002DA8,
	0x000023D0,
	0x00002414,
	0x00001D84,// LoadExecForKernel_7286CF0B
};
u32 loadexec_patch_list_05g[] = {	
	0x00002FA8,
	0x00002FF4,
	0x00002624,
	0x00002668,
	0x00001FB8, // LoadExecForKernel_7286CF0B
	0x000020A8, // LoadExecForKernel_CEFE1100
};

#elif _PSP_FW_VERSION == 660
u32 loadexec_patch_list[] = {
	0x00002D5C,
	0x00002DA8,
	0x000023D0,
	0x00002414,
	0x00001D84,// LoadExecForKernel_7286CF0B
};
u32 loadexec_patch_list_05g[] = {	
	0x00002FA8,
	0x00002FF4,
	0x00002624,
	0x00002668,
	0x00001FB8, // LoadExecForKernel_7286CF0B
	0x000020A8, // LoadExecForKernel_CEFE1100
};

#else
#error loadexec_offs.h
#endif
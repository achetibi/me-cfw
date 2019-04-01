
#if _PSP_FW_VERSION == 620
Nid_list Nid_Paf[] =
{
	{ 0x2BE8DDBB, 0xE9411837, },//
	{ 0xE8CCC611, 0x85D1CB6B, },//
	{ 0xCDDCFFB3, 0x4B2F9A4F, },//
	{ 0x48BB05D5, 0xF0D98BD1, },//
	{ 0x22FB4177, 0xC9B72C40, },//
	{ 0xBC8DC92B, 0x5F73A09B, },//
	{ 0xE3D530AE, 0x4900119B, },//
};

#elif _PSP_FW_VERSION == 639
Nid_list Nid_Paf[] =
{
	{ 0x2BE8DDBB , 0x4DCF9203 },//
	{ 0xE8CCC611 , 0xB70C91FD },//
	{ 0xCDDCFFB3 , 0xAEF47B29 },//snprintf for wchar
	{ 0x48BB05D5 , 0x9E9FFBFB },//
	{ 0x22FB4177 , 0x1F538758 },//
	{ 0xBC8DC92B , 0xCDE9615E },//
	{ 0xE3D530AE , 0x1B952318 },//
};

#elif _PSP_FW_VERSION == 660
Nid_list Nid_Paf[] =
{
	{ 0x2BE8DDBB , 0xEC805E95 },//sce_paf_private_wcscpy
	{ 0xE8CCC611 , 0x95692855 },//sce_paf_private_wcsncmp
	{ 0xCDDCFFB3 , 0xC5C17E46 },//sce_paf_private_swprintf
	{ 0x48BB05D5 , 0x5E909060 },//malloc_1_2
	{ 0x22FB4177 , 0x412B2F09 },//sce_paf_private_free
	{ 0xBC8DC92B , 0xD590412B },//scePafGetCurrentClockLocalTime
	{ 0xE3D530AE , 0x4CF09BA2 },//sce_paf_private_strcmp
};

#elif _PSP_FW_VERSION == 661
Nid_list Nid_Paf[] =
{
	{ 0x2BE8DDBB , 0xEC805E95 },//sce_paf_private_wcscpy
	{ 0xE8CCC611 , 0x95692855 },//sce_paf_private_wcsncmp
	{ 0xCDDCFFB3 , 0xC5C17E46 },//sce_paf_private_swprintf
	{ 0x48BB05D5 , 0x5E909060 },//malloc_1_2
	{ 0x22FB4177 , 0x412B2F09 },//sce_paf_private_free
	{ 0xBC8DC92B , 0xD590412B },//scePafGetCurrentClockLocalTime
	{ 0xE3D530AE , 0x4CF09BA2 },//sce_paf_private_strcmp
};

#else
#error paf_list
#endif

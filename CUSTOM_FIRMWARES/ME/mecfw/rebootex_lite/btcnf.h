
typedef struct ModuleList
{
	char * before_path;
	char * add_path;
	u8 flag;
	u16 loadmode;
} ModuleList;

#if PERMANENT == 1
#if PSP_MODEL != 4
ModuleList module_path[] = {
	{"/module/vshorig.prx",		"/vsh/module/recovery.prx"	,   1  , 4 },
	{"libatrac3plus.prx",	"/kd/usbstorms.prx"		,   1  , 0x8001 },
	{"/module/paf.prx",		"/kd/usbstorboot.prx"	,   1  , 0x8001 },
	{"/module/common_gui.prx", "/kd/usbdev.prx"		,   1  , 0x8001 },
	{"/module/common_util.prx", "/kd/lflash_fatfmt.prx"		,   1  , 0x8001 },
	{"/module/libpspvmc.prx",	"/kd/usersystemlib.prx"		, 0xEF , 0x8002 },
	{"usersystemlib.prx",	"/kd/usbstormgr.prx"	,   1  , 0x8001 },
	{"vshctrl_02g.prx",		"/kd/usbstor.prx"		,   1  , 0x8001 },
	{"mediasync.prx"		,"a" , 0 , 0x8001 },
	{ NULL			, NULL					, 0  , 0}
};
#else
ModuleList module_path[] = {
	/*
	{"/module/vshorig.prx",		"/vsh/module/recovery.prx"	,   1  , 4 },
	{"libatrac3plus.prx",	"/kd/usbstorms.prx"		,   1  , 0x8001 },
	{"/module/paf.prx",		"/kd/usbstorboot.prx"	,   1  , 0x8001 },
	{"/module/common_gui.prx", "/kd/usbdev.prx"		,   1  , 0x8001 },
	{"/module/common_util.prx", "/kd/lflash_fatfmt.prx"		,   1  , 0x8001 },
	{"/module/libpspvmc.prx",	"/kd/usersystemlib.prx"		, 0xEF , 0x8002 },
	{"usersystemlib.prx",	"/kd/usbstormgr.prx"	,   1  , 0x8001 },
	{"vshctrl_02g.prx",		"/kd/usbstor.prx"		,   1  , 0x8001 },
	{"mediasync.prx"		,"a" , 0 , 0x8001 },
	*/
	{ "mediasync.prx"			,"/kd/usbstor.prx"		,   1  , 0x8001 },
	{ "vshctrl_02g.prx"			,"/kd/usbstormgr.prx"	,   1  , 0x8001 },
	{ "usersystemlib.prx"		,"/kd/usbstorms.prx"		,   1  , 0x8001 },
	{ "/module/mcore.prx"		,"/kd/usbstoreflash.prx"	,   1  , 0x8001 },
	{ "libatrac3plus.prx"		,"/kd/usbstorboot.prx"	,   1  , 0x8001 },
	{ "/module/paf.prx"			,"/kd/usbdev.prx"		,   1  , 0x8001 },
	{ "/module/common_gui.prx"	,"/kd/lflash_fatfmt.prx"	,   1  , 0x8001 },
	{ "/module/common_util.prx"	,"/kd/usersystemlib.prx"	, 0xEF , 0x8002 },
	{ "/module/vshorig.prx"		,"/vsh/module/recovery.prx"	,   1  , 4 },
	{  NULL			, NULL					, 0  , 0}
};
#endif
#else
#if PSP_MODEL != 4
ModuleList module_path[] = {
	{"/module/vshmain.prx",		"/vsh/module/recovery.prx"	,   1  , 4 },
	{"libatrac3plus.prx",	"/kd/usbstorms.prx"		,   1  , 0x8001 },
	{"/module/paf.prx",		"/kd/usbstorboot.prx"	,   1  , 0x8001 },
	{"/module/common_gui.prx", "/kd/usbdev.prx"		,   1  , 0x8001 },
	{"/module/common_util.prx", "/kd/lflash_fatfmt.prx"		,   1  , 0x8001 },
	{"/module/libpspvmc.prx",	"/kd/usersystemlib.prx"		, 0xEF , 0x8002 },
	{"usersystemlib.prx",	"/kd/usbstormgr.prx"	,   1  , 0x8001 },
	{"vshctrl_02g.prx",		"/kd/usbstor.prx"		,   1  , 0x8001 },
	{"mediasync.prx"		,"a" , 0 , 0x8001 },
	{ NULL			, NULL					, 0  , 0}
};
#else
ModuleList module_path[] = {
	/*
	{"/module/vshmain.prx",		"/vsh/module/recovery.prx"	,   1  , 4 },
	{"libatrac3plus.prx",	"/kd/usbstorms.prx"		,   1  , 0x8001 },
	{"/module/paf.prx",		"/kd/usbstorboot.prx"	,   1  , 0x8001 },
	{"/module/common_gui.prx", "/kd/usbdev.prx"		,   1  , 0x8001 },
	{"/module/common_util.prx", "/kd/lflash_fatfmt.prx"		,   1  , 0x8001 },
	{"/module/libpspvmc.prx",	"/kd/usersystemlib.prx"		, 0xEF , 0x8002 },
	{"usersystemlib.prx",	"/kd/usbstormgr.prx"	,   1  , 0x8001 },
	{"vshctrl_02g.prx",		"/kd/usbstor.prx"		,   1  , 0x8001 },
	{"mediasync.prx"		,"a" , 0 , 0x8001 },
	*/
	{ "mediasync.prx"			,"/kd/usbstor.prx"		,   1  , 0x8001 },
	{ "vshctrl_02g.prx"			,"/kd/usbstormgr.prx"	,   1  , 0x8001 },
	{ "usersystemlib.prx"		,"/kd/usbstorms.prx"		,   1  , 0x8001 },
	{ "/module/mcore.prx"		,"/kd/usbstoreflash.prx"	,   1  , 0x8001 },
	{ "libatrac3plus.prx"		,"/kd/usbstorboot.prx"	,   1  , 0x8001 },
	{ "/module/paf.prx"			,"/kd/usbdev.prx"		,   1  , 0x8001 },
	{ "/module/common_gui.prx"	,"/kd/lflash_fatfmt.prx"	,   1  , 0x8001 },
	{ "/module/common_util.prx"	,"/kd/usersystemlib.prx"	, 0xEF , 0x8002 },
	{ "/module/vshmain.prx"		,"/vsh/module/recovery.prx"	,   1  , 4 },
	{  NULL			, NULL					, 0  , 0}
};
#endif
#endif

ModuleList *Get_list(const char * path)
{
	int i = 0;

	while( module_path[i].before_path )
	{
//		printf("-- %s\n", module_path[0].add_path );
		if( strcmp( path , module_path[i].before_path ) == 0)
		{
//			module_path[i].before_path = "d";
			return module_path + i ;
		}
		i++;
	}

	return NULL;
}

ModuleList *list_stock;

int sceKernelCheckPspConfigPatched(BtcnfHeader *a0 , int size , int flag)//decrypt patch
{

	int ret = size;//
	int i , j;
	int module_cnt;

	BtcnfHeader *header = a0;

	if( header->signature == 0x0F803001)// 0x0F803001if btcnf
	{
		module_cnt = header->nmodules;
		if( module_cnt > 0)
		{
			ModuleEntry *module_offset = (ModuleEntry *)((u32)header + (u32)(header->modulestart));
			char* modname_start = (char *)((u32)header+ header->modnamestart);
		
			for(i=0; i< module_cnt;i++)
			{	
//					printf("- %s\n", modname_start + module_offset[i].stroffset );
				if( (list_stock = Get_list( modname_start + module_offset[i].stroffset + 4) ) != NULL)
				{
//					printf("Add %s\n", list_stock->add_path );

					ModuleEntry *sp = &(module_offset[i]);

					sp->flags= list_stock->flag;//flag
					sp->loadmode=  list_stock->loadmode;
					memcpy( modname_start + module_offset[i].stroffset , list_stock->add_path , strlen( list_stock->add_path )+1 );

				}
			}
		}	
	}

	return ret;
}

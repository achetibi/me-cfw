
enum {
	PATCH_ADD,
	PATCH_OVERWRITE,
	PATCH_OVERRIDE
};

typedef struct ModuleList
{
	int patch_type;
	char * before_path;
	char * add_path;
	u16 flag;
	u16 loadmode;
} ModuleList;

ModuleList module_inferno[] = {
    { PATCH_OVERRIDE	,"/kd/np9660.prx", "/kd/inferno.prx",  0x42  , 0x8001 },
    { -1		,NULL			, NULL					, 0  , 0}
};
ModuleList module_isotope[] = {
	{ PATCH_OVERRIDE	,"/kd/np9660.prx", "/kd/isotope.prx",  0x42  , 0x8001 },
	{ -1		,NULL			, NULL					, 0  , 0}
};
ModuleList module_np9660[] = {
	{ PATCH_ADD	,"/kd/np9660.prx", "/kd/pulsar.prx"		, 0x42  , 0x8001 },
	{ -1		,NULL			, NULL					, 0  , 0}
};

/*
ModuleList module_boot[] = {
	{ PATCH_ADD ,"/kd/init.prx"	, "/kd/systemctrl_02g.prx"	, 0xEF , 0x8001 },
	{ PATCH_ADD	,"/kd/usersystemlib.prx", "/kd/vshctrl_02g.prx"		,   1  , 0x8001 },
	{ PATCH_ADD	,"/kd/popsman.prx", "/kd/horoscope.prx"		,   2 | 0x40 , 0x8001 },
	{ PATCH_ADD	,"/kd/popsman.prx", "/kd/idmanager.prx"		,   8  , 0x8001 },
	{ -1		,NULL			, NULL					, 0  , 0}
};
*/

ModuleList module_recovery[] = {
	{ PATCH_OVERRIDE	,"/vsh/module/vshmain.prx","/vsh/module/recovery.prx"	,   1  , 4 },
	{ PATCH_OVERWRITE	,"/kd/usersystemlib.prx","/kd/usbstorms.prx"		,   1  , 0x8001 },
	{ PATCH_OVERWRITE	,"/kd/libatrac3plus.prx","/kd/usbstorboot.prx"	,   1  , 0x8001 },
	{ PATCH_OVERWRITE	,"/vsh/module/paf.prx"	,"/kd/usbdev.prx"		,   1  , 0x8001 },
	{ PATCH_OVERWRITE	,"/vsh/module/common_gui.prx", "/kd/lflash_fatfmt.prx"		,   1  , 0x8001 },
	{ PATCH_OVERWRITE	,"/vsh/module/common_util.prx",	"/kd/usersystemlib.prx"		, 0xEF , 0x8002 },
	{ PATCH_OVERWRITE	,"/kd/vshctrl_02g.prx"	,	"/kd/usbstormgr.prx"	,   1  , 0x8001 },
	{ PATCH_OVERWRITE	,"/kd/mediasync.prx"	,"/kd/usbstor.prx"		,   1  , 0x8001 },
	{ -1				, NULL			, NULL					, 0  , 0}
};

static ModuleList *Get_list(ModuleList module_path[], const char * path)
{
	int i = 0;

	while( module_path[i].before_path )
	{
		if( strcmp( path , module_path[i].before_path ) == 0)
		{
			module_path[i].before_path = "";
			return module_path + i ;
		}
		i++;
	}

	return NULL;
}

int btcnf_patch(void *a0 , int size , ModuleList patch_list[] , int before , int after)
{
	ModuleList *list_stock;
	int ret = size;//
	int i , j;
	int module_cnt;

	BtcnfHeader *header = a0;

	if( header->signature == 0x0F803001)
	{
		module_cnt = header->nmodules;
//		printf("module_cnt:0x%08X\n",module_cnt);
		if( module_cnt > 0)
		{

			ModuleEntry *module_offset = (ModuleEntry *)((u32)header + (u32)(header->modulestart));
			char* modname_start = (char *)((u32)header+ header->modnamestart);
		
			for(i=0; i< module_cnt;i++)
			{				
				ModuleEntry *sp = &(module_offset[i]);

				if( before ){
					if( sp->flags & before){
						sp->flags |= after;
					}else{
						sp->flags &= ~(after);
					}
				}

				if( (list_stock = Get_list( patch_list, modname_start + sp->stroffset ) ) != NULL)
				{
//					printf("Add %s\n", list_stock->add_path );

					if( list_stock->patch_type == PATCH_OVERWRITE )
					{
						memcpy( modname_start + sp->stroffset , list_stock->add_path , strlen( list_stock->add_path )+1 );
					}
					else// if( list_stock->patch_type == PATCH_OVERRIDE || list_stock->patch_type == PATCH_ADD )
					{	

						if( list_stock->patch_type == PATCH_ADD ){	
							memcpy_b( &(module_offset[i+1]) , sp , ret - header->modulestart - 32*i);	
							ret += 32;
							header->nmodules ++;
							header->modnamestart += 0x20;
							header->modnameend += 0x20;

							module_cnt ++;

							int mode_cnt = header->nmodes;
							ModeEntry * mode_entyr = (ModeEntry *)( (u32)header + (u32)(header->modestart));
							for(j=0;j< mode_cnt;j++)
							{
								mode_entyr[j].maxsearch ++;		
								mode_entyr[j].searchstart = 0;
							}		
						}
						ret += strlen( list_stock->add_path ) + 1;
						sp->stroffset = header->modnameend - header->modnamestart;			

						memcpy( (char *)((u32)header + (u32)(header->modnameend)) , list_stock->add_path , strlen( list_stock->add_path )+1 );
						header->modnameend += strlen( list_stock->add_path ) +1;
		
					}

					sp->flags= list_stock->flag;//flag
					sp->loadmode=  list_stock->loadmode;

					modname_start =(char *)((u32)header+ header->modnamestart);
				}
			}
		}	
	}

	return ret;
}
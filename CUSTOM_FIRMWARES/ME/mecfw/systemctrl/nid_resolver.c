#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <pspsysmem_kernel.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "nid_resolver.h"

char *my_strtok(char *s1, const char *s2 );
char *my_strtok_r(char *s1, const char *s2 , char **s3 );
int my_strcspn(const char *s1, const char *s2);
int my_strspn(const char *s1, const char *s2);

int my_setjmp(int a0);
void my_longjmp(int a0, int a1);

static Module_list *get_nid_list(const char *name);
static u32 get_Nid(Module_list *list , u32 nid);

extern int (* LinkLibraryEntries)();
u32 FindPowerFunction(u32 nid);

#if _PSP_FW_VERSION == 620
#include "nid_resolver62x.h"

#elif _PSP_FW_VERSION == 639
#include "nid_resolver63x.h"

#elif _PSP_FW_VERSION == 660 || _PSP_FW_VERSION == 661
#include "nid_resolver66x.h"

#else
#error nid_resolver.c
#endif

#include "nid_dnr_list.h"

void create_DNR_list(const char *libname , u32 text_addr )
{
	int i , j;
	for(i=0;i<DNR_FILES;i++)
	{
		if(strcmp( libname , DNR_table[i].name ) == 0)
		{
			Nid_list *buff = DNR_table[i].buf;
			for(j=0;j<DNR_table[i].count ; j++)
			{
				if( !(buff[j].send & 0xFF000000) )
					buff[j].send += text_addr;
			}

			break;
		}
	}
}

static void search_DNR_list(struct SceLibraryStubTable *current , u32 *list)
{
	int i;
	const char *libname = current->libname;

	for(i=0;i<DNR_FILES;i++)
	{
		if(strcmp( libname , DNR_table[i].name ) == 0)
		{
			list[i] = (u32)current;
			break;
		}
	}
}

static int resolve_import_table(u32 *list)
{
	int i , j, changed = 0;

	for(j=0;j<DNR_FILES;j++)
	{
		if( list[j])
		{
			struct SceLibraryStubTable *current = (struct SceLibraryStubTable *)list[j];

			for(i=0;i< current->stubcount ;i++)
			{
				u32 ret = get_Nid( &(DNR_table[j]) , current->nidtable[i] );

				if( ret & 0xFF000000 )
				{
					u32 patch_addr = (u32)(current->stubtable) + (i*8);
					REDIRECT_FUNCTION( patch_addr , (void *)ret );

					changed = 1;
				}
			}
		}
	}

	return changed;
}


#include "scePaf_nid.h"
Module_list paf_table = { NULL , Nid_Paf , sizeof(Nid_Paf)/sizeof(Nid_list) };
int nid_paf_enable_flag = 0;

Module_list Mod_table[] =
{
	{ "SysMemForKernel"				, Nid_sysmem,		 sizeof(Nid_sysmem)/sizeof(Nid_list)	},
	{ "KDebugForKernel"				, Nid_kdebug,		 sizeof(Nid_kdebug)/sizeof(Nid_list)	},
	{ "LoadCoreForKernel"			, Nid_loadcore ,	 sizeof(Nid_loadcore)/sizeof(Nid_list)	},
	{ "ExceptionManagerForKernel"	, Nid_exception ,	 sizeof(Nid_exception)/sizeof(Nid_list)	},
	{ "InterruptManagerForKernel"	, Nid_interrupt ,	 sizeof(Nid_interrupt)/sizeof(Nid_list)	},
	{ "IoFileMgrForKernel"			, Nid_Iofile ,		 sizeof(Nid_Iofile)/sizeof(Nid_list)	},
	{ "ModuleMgrForKernel"			, Nid_ModuleMgr ,	 sizeof(Nid_ModuleMgr)/sizeof(Nid_list)	},
	{ "LoadExecForKernel"			, Nid_LoadExec ,	 sizeof(Nid_LoadExec)/sizeof(Nid_list)	},
	{ "sceDdr_driver"				, Nid_Ddr ,			 sizeof(Nid_Ddr)/sizeof(Nid_list)		},
	{ "sceDmacplus_driver"			, Nid_Dmacplus ,	 sizeof(Nid_Dmacplus)/sizeof(Nid_list)	},
	{ "sceGpio_driver"			,Nid_Gpio	, sizeof(Nid_Gpio)/sizeof(Nid_list)		},
	{ "sceSysreg_driver"		,Nid_Sysreg , sizeof(Nid_Sysreg)/sizeof(Nid_list)	},
	{ "sceSyscon_driver"		,Nid_Syscon , sizeof(Nid_Syscon)/sizeof(Nid_list)	},
	{ "sceDisplay_driver"		,Nid_Display, sizeof(Nid_Display)/sizeof(Nid_list)	},
	{ "sceDve_driver"			,Nid_Dve	, sizeof(Nid_Dve)/sizeof(Nid_list)		},
	{ "sceGe_driver"			,Nid_Ge		, sizeof(Nid_Ge)/sizeof(Nid_list)		},
	{ "sceCtrl_driver"			,Nid_Ctrl	, sizeof(Nid_Ctrl)/sizeof(Nid_list)		},
	{ "sceUmd"					,Nid_Umd	, sizeof(Nid_Umd)/sizeof(Nid_list)		},
	{ "sceHprm_driver"			,Nid_Hprm	, sizeof(Nid_Hprm)/sizeof(Nid_list)		},
	{ "scePower_driver"		,Nid_Power		, sizeof(Nid_Power)/sizeof(Nid_list)	},
	{ "sceImpose_driver"	,Nid_Impose		, sizeof(Nid_Impose)/sizeof(Nid_list)	},
	{ "sceRtc_driver"		,Nid_Rtc		, sizeof(Nid_Rtc)/sizeof(Nid_list)		},
	{ "sceReg_driver"		,Nid_Reg		, sizeof(Nid_Reg)/sizeof(Nid_list)		},
	{ "memlmd"				,Nid_memlmd		, sizeof(Nid_memlmd)/sizeof(Nid_list)	},
	{ "SysTimerForKernel"	,Nid_Systimer	, sizeof(Nid_Systimer)/sizeof(Nid_list)	},
	{ "sceAudio_driver"		,Nid_Audio		, sizeof(Nid_Audio)/sizeof(Nid_list)	},
	{ "sceMesgLed_driver"	,Nid_MesgLed	, sizeof(Nid_MesgLed)/sizeof(Nid_list)	},
	{ "sceClockgen_driver"	,Nid_Clockgen	, sizeof(Nid_Clockgen)/sizeof(Nid_list)	},
	{ "sceCodec_driver"		,Nid_Codec		, sizeof(Nid_Codec)/sizeof(Nid_list)	},
	{ "sceMeCore_driver"	,Nid_MeCore		, sizeof(Nid_MeCore)/sizeof(Nid_list)	},
};

#define M_FILES		(sizeof(Mod_table)/sizeof(Module_list))

int nid_enable_flag[M_FILES] = { 0 };

static int (* NidResolverEx)();
void SystemCtrlForKernel_6A5F76B5(int (* a0)())
{
	NidResolverEx = a0;
}

static Module_list *get_nid_list(const char *name)
{
	int i;
	for(i = 0; i < M_FILES; i++)
	{
		if(strcmp(name, Mod_table[i].name) == 0)
		{
			if(nid_enable_flag[i] == 0)
			{
				return &(Mod_table[i]);
			}

			break;
		}
	}

	return NULL;
}

//get fix nid
static u32 get_Nid(Module_list *list, u32 nid)
{
	int i;
	Nid_list *buff = list->buf;

	for(i = 0; i < list->count; i++)
	{
		if(buff[i].receive == nid)
		{
			return buff[i].send;
		}
	}

	return 0;
}

u32 sctrlKernelResolveNid(const char *szLib, u32 nid)
{
	u32 ret = 0;
	Module_list *add= get_nid_list(szLib);
	if(add)
	{
		u32 get = get_Nid(add, nid);
		if(get)
		{
			ret = get;
		}
	}

	return ret;
}

static int check_module_version(void *buff)
{
	u16 *func = (u16 *)sctrlHENFindFunction(buff, NULL, 0x11B97506);//module_sdk_version

	if(func)
	{
		if(func[1] == CFW_VER_MAJOR)//0x0603
		{
			return 1;
		}
	}

	return 0;
}

//nid resolve user
int LinkLibraryEntriesUserPatch(char* szMod, void *szLib, int a2, int a3)
{
	if( check_module_version( szMod ) == 0)
	{
		Module_list *list;
		u32 receive;
		void *entTab;
		int entLen;

		struct SceLibraryStubTable *current;
		int i = 0, j = 0;
		entTab = szMod;
		entLen = (int)szLib;

		while(i < entLen)
		{
			current = (struct SceLibraryStubTable *)(entTab + i);

			list = NULL;

			if(strcmp(current->libname, "scePaf") == 0 && nid_paf_enable_flag == 0)
			{
				list = &paf_table;//Get_nid_list(current->libname);
			}

			if(list)
			{
				for(j = 0; j < current->stubcount; j++)
				{
					receive = get_Nid(list, current->nidtable[j]);

					if(receive)
					{
						current->nidtable[j] = receive;
					}
				}
			}

			i += (current->len * 4);
		}

//		ClearCaches();
	}
	
	return LinkLibraryEntries(szMod, szLib, 1, a3);
}

//nid resolver
int LinkLibraryEntriesPatch(char* szMod, void *szLib)
{

	if(check_module_version(szMod))
	{
		return LinkLibraryEntries(szMod, szLib, 0, 0);
	}

	Module_list *list;
	u32 receive;
	void *entTab;
	int entLen;

	struct SceLibraryStubTable *current;

	SceLibraryStubTable *power_stub = NULL;

	u32 dnr_buff[DNR_FILES];
	memset(dnr_buff, 0, sizeof(u32) * DNR_FILES);

	int i = 0, j = 0;
	entTab = szMod;
	entLen = (int)szLib;

	while(i < entLen)
	{
		current = (struct SceLibraryStubTable *)(entTab + i);

		list = get_nid_list(current->libname);

		search_DNR_list(current, dnr_buff);

		if(strcmp(current->libname, "scePower_driver") == 0)
		{
			power_stub = current;
		}

		if(NidResolverEx)
		{
			NidResolverEx(current);
		}

		if(list)
		{
			int cnt = current->stubcount;
			for(j = 0; j < cnt; j++)
			{
				receive = get_Nid(list, current->nidtable[j]);

				if(receive)
				{
					current->nidtable[j] = receive;
				}
			}
		}

		i += (current->len * 4);
	}

//	ClearCaches();

	int r = LinkLibraryEntries(szMod, szLib, 0, 0);
	int changed = 0;

	if(power_stub)
	{
		for(i = 0; i < power_stub->stubcount; i++)
		{
			if(power_stub->nidtable[i] == 0x737486F2)//scePowerSetClockFrequency
			{
				u32 patch_addr = (u32)(power_stub->stubtable) + (i*8);
				REDIRECT_FUNCTION( patch_addr , (void *)FindPowerFunction(0x737486F2));
				changed = 1;
			}
		}
	}

	changed |= resolve_import_table(dnr_buff);

	if(changed != 0)
	{
		ClearCaches();
	}

	return r;
}

int sctrlKernelSetNidResolver(char *libname, u32 enabled)
{
	u32 i;
	u32 old;

	if (strcmp(libname, "scePaf") == 0)
	{
		old = (nid_paf_enable_flag) ? 0 : 1;
		nid_paf_enable_flag = (enabled) ? 0 : 1;

		return old;
	}

	for(i = 0; i < M_FILES; ++i)
	{
		if (strcmp(libname, Mod_table[i].name) == 0)
		{
			old = (nid_enable_flag[i]) ? 0 : 1;
			nid_enable_flag[i] = (enabled) ? 0 : 1;

			return old;
		}
	}

	return -1;
}

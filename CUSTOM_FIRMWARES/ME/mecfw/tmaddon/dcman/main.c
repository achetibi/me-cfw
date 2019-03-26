#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspiofilemgr_kernel.h>
#include <pspsysevent.h>
#include <pspnand_driver.h>
#include <stdio.h>
#include <string.h>

#include "systemctrl_me.h"
#include "dcman.h"


//#define NOP			0x00000000
//#define JAL_OPCODE	0x0C000000
//#define J_OPCODE		0x08000000
//#define SC_OPCODE		0x0000000C
//#define JR_RA			0x03e00008

//#define MAKE_JUMP(a, f) _sw(J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a);
//#define MAKE_CALL(a, f) _sw(JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);
//#define MAKE_SYSCALL(a, n) _sw(SC_OPCODE | (n << 6), a);
//#define JUMP_TARGET(x) (0x80000000 | ((x & 0x03FFFFFF) << 2))

//#define REDIRECT_FUNCTION(a, f) _sw(J_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a);  _sw(NOP, a+4);
//#define MAKE_DUMMY_FUNCTION0(a) _sw(0x03e00008, a); _sw(0x00001021, a+4);
//#define MAKE_DUMMY_FUNCTION1(a) _sw(0x03e00008, a); _sw(0x24020001, a+4);


PSP_MODULE_INFO("DesCemManager", 0x1007, 1, 0);

u64 sceSysregGetFuseId();
u32 sceSysregGetTachyonVersion();
int sceSysconGetBaryonVersion(u32 *);
int sceSysconGetPommelVersion(u32 *);
u32 sceSysregGetFuseConfig();

int sceNandReadExtraOnly(u32 ppn, void *buf, u32 count); // not in the sdk
//int sceNandReadAccess(u32 ppn, void *buf, void *buf2, u32 count, u32 unk); // not in the sdk
int sceNandWriteAccess(u32 ppn, void *buf, void *buf2, u32 count, u32 unk); // not in the sdk
int sceNandReadPagesRawAll(u32 page, u8 *block, u32 count, u32 unk); // not in the sdk
int sceNandEraseBlock(u32 page); // not in the sdk

int sceLflashFatfmtStartFatfmt(int argc, char *argv[]); // not in the sdk

//	u32 k1 = pspSdkSetK1(0);
//	pspSdkSetK1(k1);

u32 data1BB8,data1BC0,data1BD4,data1BEC;

u64 data1BF8;//fuseid
u32 data1BBC;//fuse cnfig
u32 data1BF0;//nand size
u32 data1BD0;//total block

int data1BD8 = 0;

int dcGetCancelMode()
{
	return data1BD8;
}

int dcSetCancelMode(int mode)
{
	data1BD8 = mode;
	return 0;
}

u32  LW(u32 address)
{
	u32 k1 = pspSdkSetK1(0);
	u32 ret = *(u32 *)address;
	pspSdkSetK1(k1);

	return ret;
}

void SW(u32 word, u32 address)
{
	u32 k1 = pspSdkSetK1(0);
	_sw( word, address);
	pspSdkSetK1(k1);
}

int dcGetNandInfo(u32 *pagesize, u32 *ppb, u32 *totalblocks)
{
	u32 k1 = pspSdkSetK1(0);

	if( pagesize != NULL)
	{
		*pagesize = sceNandGetPageSize();
	}

	if( ppb != NULL)
	{
		*ppb = sceNandGetPagesPerBlock();
	}

	if( totalblocks != NULL)
	{
		*totalblocks = sceNandGetTotalBlocks();
	}
	
	pspSdkSetK1(k1);
	return 0;
}

int dcGetHardwareInfo(u32 *ptachyon, u32 *pbaryon, u32 *ppommel, u32 *pmb, u64 *pfuseid, u32 *pfuseconfig, u32 *pnandsize)
{
	u32 k1 = pspSdkSetK1(0);

	if( ptachyon != NULL)
	{
		*ptachyon = data1BB8;
	}

	if( pbaryon != NULL)
	{
		*pbaryon = data1BD4;
	}

	if( ppommel != NULL)
	{
		*ppommel = data1BEC;
	}

	if( pmb != NULL)
	{
		*pmb = data1BC0;
	}

	if( pfuseid != NULL)
	{
		*pfuseid = data1BF8;
	}

	if( pfuseconfig != NULL)
	{
		*pfuseconfig = data1BBC;
	}

	if( pnandsize != NULL)
	{
		*pnandsize = data1BF0;
	}
	
	pspSdkSetK1(k1);
	return 0;
}

int dcLflashStartFatfmt(int argc, char *argv[])
{
	u32 k1 = pspSdkSetK1(0);
	int level = sctrlKernelSetUserLevel(4);
	int ret = sceLflashFatfmtStartFatfmt( argc, argv);
	sctrlKernelSetUserLevel(level);
	pspSdkSetK1(k1);

	return ret;
}

int dcEraseNandBlock(u32 page)
{
	u32 k1 = pspSdkSetK1(0);
	int ret = sceNandEraseBlock( page );
	pspSdkSetK1(k1);

	return ret;
}

//0x00000C10 
int dcWriteNandBlock(u32 page, u8 *user, u8 *spare)
{
	int i;
	u32 k1 = pspSdkSetK1(0);

	//always 32
	int ppb = sceNandGetPagesPerBlock();

	for(i=0;i<ppb;i++)
	{
		sceNandWriteAccess( page, user, spare, 1, 49 );

		page ++;
		user += 0x200;
		spare += 0x10;
	}

	pspSdkSetK1(k1);
	return 0;
}

//0x00000CBC: 
int dcReadNandBlock(u32 page, u8 *block)
{
	int i, j;
	u32 k1 = pspSdkSetK1(0);

	if( sceNandIsBadBlock( page ) != 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}
	
	//always 32 page
	int ppb = sceNandGetPagesPerBlock();

	for(i=0;i<ppb;i++)
	{
		for(j=0;j<4;j++)
		{
			sceNandReadPagesRawAll( page, block, 0, 1);
			sceNandReadExtraOnly( page, block + 0x200, 1);
		}

		page ++;
		block += 0x210;
	}
	
	pspSdkSetK1(k1);
	return 0;
}

int dcUnlockNand()
{
	u32 k1 = pspSdkSetK1(0);
	sceNandUnlock();
	pspSdkSetK1(k1);

	return 0;
}

int dcLockNand(int flag)
{
	u32 k1 = pspSdkSetK1(0);
	int ret = sceNandLock( flag );
	pspSdkSetK1(k1);

	return ret;
}

void *search_module_stub(SceModule2 *pMod, const char *szLib, u32 nid)
{
	void *entTab = pMod ->stub_top;
	int entLen = pMod->stub_size;
	struct SceLibraryStubTable *current;
	int i = 0 ,j;

	while( i < entLen )
	{
		current = (struct SceLibraryStubTable *)(entTab + i);
		if(strcmp(current->libname, szLib ) == 0)
		{
			for(j=0;j< current->stubcount ;j++)
			{
				if( current->nidtable[j] == nid )
				{
					return (void *)((u32)(current->stubtable) + 8*j );
				}
			}

			break;
		}

		i += (current->len * 4);
	}

	return NULL;
}

u32 dcFindModuleStub(char *modname, char *libname, u32 nid)
{
	u32 ret = 0;
	u32 k1 = pspSdkSetK1(0);

	SceModule2 *mod = sceKernelFindModuleByName(modname);
	if( mod != NULL)
	{
		ret = (u32)search_module_stub(mod, libname, nid );
	}

	pspSdkSetK1(k1);
	return ret;
}

int dcPatchModuleString(char *modname, char *string, char *replace)
{
	int ret = -1;
	u32 k1 = pspSdkSetK1(0);

	SceModule2 *mod = sceKernelFindModuleByName(modname);
	if( mod != NULL)
	{
		u32 len = strlen(string);
		u32 text_addr = mod->text_addr;
		u32 text_size = mod->text_size + mod->data_size + mod->bss_size;
		u32 text_end = text_addr + text_size - len;

		int flag = 0;
		while( text_addr < text_end )
		{
			if( memcmp( (void *)text_addr, string, len ) == 0 )
			{
				memcpy( (void *)text_addr, replace, len );
				flag += 1;
			}

			text_addr ++;
		}

		if(flag)
		{
			dcClearCache();
		}

		ret = flag;
	}

	pspSdkSetK1(k1);
	return ret;
}


int dcPatchModule(char *modname, int type, u32 addr, u32 word)
{
	int ret = -1;
	u32 k1 = pspSdkSetK1(0);
	
	SceModule2 *mod = sceKernelFindModuleByName(modname);
	if( mod != NULL)
	{
		if( type == 0 )
		{
			_sw( word, mod->text_addr + addr );
			ret = 0;
		}
		else if( type == 1)
		{
			_sh( (u16)word, mod->text_addr + addr );
			ret = 0;
		}
		else if( type == 2)
		{
			_sb( (u8)word, mod->text_addr + addr );
			ret = 0;
		}

		dcClearCache();

	}
	
	pspSdkSetK1(k1);
	return ret;
}

static int SysEventHandler(int ev_id, char* ev_name, void* param, int* result)
{
	if( ev_id == 0x100 )
	{
		if( data1BD8 )
		{
			return -1;
		}
	}

	return 0;
}

static PspSysEventHandler event_handler =
{
	sizeof(PspSysEventHandler),
	"",
	0x00FFFF00,
	SysEventHandler
};


void dcClearCache()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

STMOD_HANDLER previous = NULL;

int sub_000010A0(SceModule2 *mod)
{
//	u32 text_addr = mod->text_addr;
	char *modinfo = mod->modname;

	if (strcmp(modinfo, "sceLflashFatfmt") == 0) 
	{
		return 0;
	}
/*
	if (strcmp(modinfo, "sceLFatFs_Updater_Driver") == 0)
	{
		MAKE_CALL( text_addr + 0x88F8 , sub_00000294 );
		data1BC4 = text_addr + 0x8E40;
		ClearCache();
	}
	else if(strcmp(modinfo, "sceWlan_Driver") == 0)
	{
		MAKE_CALL( text_addr + 0x4DF8 , sub_00000230 );
		data1BE0 = text_addr + 0xD47C;
		ClearCache();
	}
	else if (strcmp(modinfo, "sceNAND_Updater_Driver") == 0)
	{
	}
*/
	if( previous )  
		return previous( mod );

	return 0;

}

int module_start(SceSize args, void *argp)
{

	data1BB8 = 0;
	data1BC0 = 9;//TAUN
	data1BD4 = 0;
	data1BEC = 0;

	data1BB8 = sceSysregGetTachyonVersion();

	if( sceSysconGetBaryonVersion( &data1BD4 ) < 0 )
	{
		data1BD4 = 0xDADADADA;
	}

	if( sceSysconGetPommelVersion( &data1BEC ) < 0 )
	{
		data1BEC = 0xDADADADA;
	}

	switch( data1BB8)
	{
		case 0x00100000:
			data1BC0 = DEVKIT;
			break;

		case 0x00140000:
			//TA-079 v1
			data1BC0 = TA79v1;
			break;

		case 0x00200000:
			if( data1BD4 == 0x00030600 )
			{
				//TA-079 v2
				data1BC0 = TA79v2;
			}
			else if( data1BD4 == 0x00040600)
			{
				//TA-079 v3
				data1BC0 = TA79v3;
			}
			
			break;
	
		case 0x00300000:
			//TA-081
			data1BC0 = TA81;
			break;

		case 0x00400000:
			if( data1BD4 == 0x00114000 )
			{
				//TA-082
				data1BC0 = TA82;
			}
			else if( data1BD4 == 0x00121000)
			{
				//TA-086
				data1BC0 = TA86;
			}
			break;

		case 0x00500000:
			if( data1BD4 == 0x0022B200 )
			{
				//TA-085
				data1BC0 = TA85;
			}
			else if( data1BD4 == 0x00234000 )
			{
				//TA-085 v2
				data1BC0 = TA85v2;
			}
			else if( data1BD4 == 0x00243000)
			{
				//
				if( data1BEC == 123 )
				{
					//TA-088
					data1BC0 = TA88;
				}
				else if( data1BEC == 132)
				{
					//TA-090
				}
			}
			break;
	}

	data1BF8 = (u64)sceSysregGetFuseId();

	data1BBC = sceSysregGetFuseConfig();

	data1BD0 = sceNandGetTotalBlocks();

	data1BF0 = sceNandGetPagesPerBlock() * sceNandGetPageSize() * data1BD0;

	sceKernelRegisterSysEventHandler( &event_handler );

	previous = sctrlHENSetStartModuleHandler( sub_000010A0 );

	return 0;
}



#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <psputilsforkernel.h>

#include <stdio.h>
#include <string.h>

#include <systemctrl_me.h>
#include "main.h"


PSP_MODULE_INFO("Idchange_Driver", 0x5006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

STMOD_HANDLER prev_handler = NULL;

u32 sceKernelQuerySystemCall( void* address );

static int icon_patch = 1;
static int unofficial_game = 0;
static int key_enabled = 0;


void ClearCaches()
{
	sceKernelDcacheWritebackAll(); 
	sceKernelIcacheClearAll();
}

int ReadFile(char *file, void *buf, int size, int mode) {
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, mode);
	if (fd < 0) return fd;
	int read = sceIoRead(fd, buf, size);
	sceIoClose(fd);
	return read;
}

int WriteFile(char *file, void *buf, int size) {
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 511);
	if (fd < 0)	return fd;
	int written = sceIoWrite(fd, buf, size);
	sceIoClose(fd);
	return written;
}

void *search_module_stub(SceModule2 *pMod, const char *szLib, u32 nid)
{
	void *entTab = pMod ->stub_top;
	int entLen = pMod->stub_size;
	struct SceLibraryStubTable *current;
	int i = 0 ,j;

	while( i < entLen ) {
		current = (struct SceLibraryStubTable *)(entTab + i);
		if(strcmp(current->libname, szLib ) == 0) {
			for(j=0;j< current->stubcount ;j++) {
				if( current->nidtable[j] == nid ) {
					return (void *)((u32)(current->stubtable) + 8*j );
				}
			}

			break;
		}
		i += (current->len * 4);
	}

	return NULL;
}

int (* sub_0000266C)() = NULL;
int (* sub_00000124)() = NULL;
int (* sub_00000000)() = NULL;
char dummy_id[48];

int sub_0000266C_patch(char *filename, void *thekeys , void *name)
{
	int res;
	char path[256];
	char *p; 
	char dummy_key[16];


	strcpy(path, filename);
	p = strrchr(path, '/');
	if (!p)
	{
		return 0xCA000000;
	}

	strcpy(p+1, "KEYS.BIN");
	res = ReadFile(path, dummy_key , 0x10, 0); 
	if (res == 0x10)
	{
		key_enabled = 1;
		memcpy( thekeys , dummy_key , 0x10);
		goto COPY_KEYS;
	}

	SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 511 );
	if (fd >= 0)
	{
		HEADER header; 
		sceIoRead(fd, &header,sizeof(header));
		sceIoLseek(fd, header.offset[6], PSP_SEEK_SET);
		sceIoRead(fd, &header, 4);
		sceIoClose(fd);

		if (header.signature == 0x464C457F) /* ELF */
		{
			//write_debug( "is elf\n", NULL , 0 );
			memset( dummy_key , 0xDE , 0x10 );
			memcpy( thekeys , dummy_key , 16 );
			goto COPY_KEYS;
		}
	}

	int ret = sub_0000266C(filename , thekeys, name );

	if( ret >= 0)
	{
		WriteFile( path , thekeys , 0x10);
	}

	return ret;

COPY_KEYS:
	
	sub_00000000( filename );
						
	memset(dummy_id , 0 , 48 );		
//	strcpy(dummy_id, "XX0000-XXXX00000_00-XXXXXXXXXX000XXX");
	memset(dummy_id, '-', 36 );
		
	if(name)		
	{	
		memcpy( name , dummy_id , 48);		
	}

	sub_00000124( filename , dummy_key , dummy_id );

	return 0;
}

SceUID sceIoOpenPatched(const char *file, int flags, SceMode mode)
{
	if( unofficial_game )
	{

		if( strstr( file , "EBOOT.PBP") || strstr( file , "DOCUMENT.DAT") )
			flags &= 0xBFFFFFFF;
	}

	return sceIoOpen(file, flags, mode);
}

int sceIoIoctlPatch(SceUID fd, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
//	write_debug( "ioctl call\n" , NULL , 0 );

	if( unofficial_game)
	{
		if( cmd == 0x04100001 )
		{
			return 0;
		}

		if( cmd == 0x04100002 )
		{
			sceIoLseek32(fd , *(u32 *)indata , PSP_SEEK_SET );
			return 0;
		}

	}

	return sceIoIoctl( fd , cmd , indata , inlen , outdata , outlen );
}

#include "pop_icon.h"

SceUID *document_fd = NULL;

int sceIoReadPatch(SceUID fd, void *data, SceSize size)
{

//	write_debug( "readPatch:\n" , NULL , 0 );
//	log_u32( "size", size );

	SceSize read_size = sceIoRead(fd , data, size );

	if( read_size != size )
	{
		if( fd == document_fd[0] )
		{
			int k1 = pspSdkSetK1(0); 
			read_size = sceIoRead(fd , data, size );
			pspSdkSetK1(k1); 
		}

		return read_size;
	}


	if( read_size == 4)	
	{	
//		if( *(u32 *)data == 0x464C457F )
		if( _lwl(data) == 0x464C457F )
		{
//			*(u32 *)data = 0x5053507E;
			_swl( 0x5053507E, data);
		}

		return 4;
	}
		

	if(read_size == DUMMY_ICON_SIZE )//0x3730
	{
		if( icon_patch )
		{
			if( icon_patch == 2 || 
//				( icon_patch == 1 && *(u32 *)data == 0x474E5089 ))
				( icon_patch == 1 && _lwl(data) == 0x474E5089 ))
			{
				memcpy( data , png_icon , DUMMY_ICON_SIZE );//	0x3730	
				return DUMMY_ICON_SIZE;
			}
		}
	}


	if( read_size >= 1056 )
	{
		u8 *buff_c = (u8 *)data;

		if( buff_c[1051] == 39 &&
			buff_c[1052] == 25 &&
			buff_c[1053] == 34 &&
			buff_c[1054] == 65 &&
			buff_c[1050] == buff_c[1055])
		{
			buff_c[1051] = 85;
		}
	}
	return read_size;

}

int ( *scePopsManExitVSHKernel)(u32 error_code) = NULL;

int popsDeflateDecompress(u32 destSize, u8 *dest , u8 *src, u32 unknown)
{ 
	int k1 = pspSdkSetK1(0); 
	int ret = 0;

//	printf("destsize = 0x%08X , src = 0x%08X, dest = 0x%08X \n", destSize, src, dest );
	if( destSize != 0x9300 )
	{
		scePopsManExitVSHKernel( (u32)destSize);
		pspSdkSetK1(k1);
		return 0;
	}

	ret = sceKernelDeflateDecompress(src, destSize, dest , 0 );
//	printf("ret = 0x%08X \n", ret );

	pspSdkSetK1(k1); 
	return (ret == 0x9300) ? 0x92FF : ret;
}

/*
int dummy()
{
	return 0;
}
*/

static void HookPopsManExitKernel()
{
	scePopsManExitVSHKernel = (void *)sctrlHENFindFunction("scePops_Manager","scePopsMan", 0x0090B2C8 );
}

int OnPspRelSectionEvent(SceModule2 *mod)
{
	u32 text_addr =mod->text_addr; 
	char *modinfo=mod->modname;

//	printf("mod: %s \n", modinfo );
	if( strcmp( modinfo, "scePops_Manager") == 0 )
	{
		if(unofficial_game)
		{
			int (* sceKernelSetCompiledSdkVersion_k)() = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x315AD3A0 );
			if(sceKernelSetCompiledSdkVersion_k)		
				sceKernelSetCompiledSdkVersion_k( sceKernelDevkitVersion() );

			//addiu $v0, $zr, 1 
			_sw( 0x24020001 ,		text_addr + pops_patch_list.popsman_list.DecryptPatchAddr1 );
			MAKE_DUMMY_FUNCTION0(	text_addr + pops_patch_list.popsman_list.DecryptPatchAddr2 );
			MAKE_DUMMY_FUNCTION1(	text_addr + pops_patch_list.popsman_list.DecryptPatchAddr3 );
			_sw( 0 ,				text_addr + pops_patch_list.popsman_list.DecryptPatchAddr4 );

			HookPopsManExitKernel();
		}
		
		document_fd = (SceUID *)(text_addr + pops_patch_list.popsman_list.Document_fd_Addr );

		_sw( 0x00002021 , text_addr + pops_patch_list.popsman_list.RifCheckPatchAddr1 );
		_sw( 0x00001021 , text_addr + pops_patch_list.popsman_list.RifCheckPatchAddr2 );

		REDIRECT_FUNCTION( text_addr + pops_patch_list.popsman_list.IoOpenPatchAddr	, sceIoOpenPatched );
		REDIRECT_FUNCTION( text_addr + pops_patch_list.popsman_list.IoctlPatchAddr	, sceIoIoctlPatch );
		REDIRECT_FUNCTION( text_addr + pops_patch_list.popsman_list.IoReadPatchAddr	, sceIoReadPatch );
	
		MAKE_CALL( text_addr + pops_patch_list.popsman_list.GetKeyPatchAddr1 , sub_0000266C_patch );
		MAKE_CALL( text_addr + pops_patch_list.popsman_list.GetKeyPatchAddr2 , sub_0000266C_patch );

		sub_00000000 = (void *)(text_addr);
		sub_0000266C = (void *)(text_addr + pops_patch_list.popsman_list.GetKeyAddr );
		sub_00000124 = (void *)(text_addr + 0x00000124 );

		_sw( 0, text_addr + pops_patch_list.popsman_list.DevkitVerPatchAddr );

		ClearCaches();

	}
	else if( strcmp( modinfo, "pops") == 0 )
	{
		if(unofficial_game)
		{
			const PopsList *patch_list = NULL;
			int model = sceKernelGetModel();
			switch( model )
			{
				case 0:
					patch_list = &(pops_patch_list.pops_list_01g);
					break;
				case 1:
				case 2:
					patch_list = &(pops_patch_list.pops_list_02g);
					break;
				case 3://04g
					patch_list = &(pops_patch_list.pops_list_04g);
					break;
				case 4:
					patch_list = &(pops_patch_list.pops_list_05g);
					break;
				case 6://07g
				case 8://09g
					patch_list = &(pops_patch_list.pops_list_07g);
					break;
				case 10:
					patch_list = &(pops_patch_list.pops_list_11g);
					break;
			}

			if( icon_patch )
				_sw( 0x24050000 | DUMMY_ICON_SIZE  , text_addr + patch_list->IconPatchAddr );//addiu      $a1, $zr, 

			u32 x = (u32)search_module_stub( mod,"scePopsMan", 0x0090B2C8 );
			MAKE_CALL( text_addr + patch_list->DecryptPatchAddr, x );
			MAKE_SYSCALL( x + 4  , sceKernelQuerySystemCall(popsDeflateDecompress));

			_sw( 0 , text_addr + patch_list->GameIdPatchAddr );

			ClearCaches();
		}

	}
 
	if(prev_handler)
	{
		return prev_handler(mod);
	}

	return 0;
}

int module_start(SceSize args, void *argp)
{
//	init_debug();

	SceUID fd;
	if((fd = sceIoOpen(sceKernelInitFileName(), PSP_O_RDONLY, 511 )) < 0)
	{
		return 1;
	}

	u32 header[0x28 / 4];
	sceIoRead(fd, header, 0x28);

	u32 header_offset = header[0x24 / 4];
	u32 icon0_offset = header[0xC / 4];

	sceIoLseek(fd, header_offset , PSP_SEEK_SET);
	sceIoRead(fd, header, 0x28);

	if(memcmp( header , "PSTITLE" , 7 ) == 0)
	{
		sceIoLseek(fd, header_offset + 0x200, PSP_SEEK_SET);
	}
	else
	{
		sceIoLseek(fd, header_offset + 0x400, PSP_SEEK_SET);
	}

	sceIoRead(fd, header, 0x28);
	if(header[0] != 0x44475000)	//official
	{	
		//write_debug( "unofficial game\n" , NULL , 0 );
		unofficial_game = 1;
	}
	else
	{
		unofficial_game = 0;
	}

	sceIoLseek(fd, icon0_offset, PSP_SEEK_SET);
	sceIoRead(fd, header, 0x28);

	if( header[0] == 0x474E5089 )//png?
	{
		if( header[ 4/4] == 0x0A1A0A0D &&
			header[12/4] == 0x52444849 &&
			header[16/4] == 0x50000000 &&
			header[20/4] == 0x50000000 )
		{
			icon_patch = 0;
		}
	}
	else
	{
		icon_patch = 2;
	}

	sceIoClose(fd);

	prev_handler = sctrlHENSetStartModuleHandler(OnPspRelSectionEvent); 

	return 0;
}
int module_stop(SceSize args, void *argp)
{
	return 0;
}

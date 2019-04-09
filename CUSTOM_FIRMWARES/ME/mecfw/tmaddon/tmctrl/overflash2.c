
//sub_00001A7C
int flashfat_open2( OpenParams *open_params )
{
	PspIoDrvFileArg *arg = open_params->arg;
	char *file = open_params->file;
	int flags = open_params->flags;
	SceMode mode = open_params->mode;

	sceKernelWaitSema( flashfat_sema , 1, NULL);
	sub_000015DC( file );
	
	int i;
	int ret = 0x80010018;
	for(i=0;i<32;i++)
	{
		if( open_info[i].index ==0 )
		{
			u32 value = sub_00000DD0( i , longpath_buff , flags , mode );
			if( value < 0)
			{
				ret = value;
			}
			else
			{
				arg->arg = (void *)i;
				ret = 0;
			}

			break;
		}
	}
	
	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_0000142C
int flashfat_close2(PspIoDrvFileArg *arg)
{
	sceKernelWaitSema( flashfat_sema , 1, NULL);

	int i = (int)( arg->arg );
	int ret = 0;
	int uid = sub_00000FC4( i );
	if( uid < 0)
	{
		ret = uid;
	}
	else
	{
		ret = sceIoClose( uid );
		if( ret < 0)
		{
			//ret = ret;
		}
		else
		{
			open_info[i].index = 0;
			ret = 0;
		}

	}

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_00001374
int flashfat_read2( ReadParams *read_params )
{
	PspIoDrvFileArg *arg = read_params->arg;
	char *data = read_params->data;
	int len = read_params->len;

	sceKernelWaitSema( flashfat_sema , 1, NULL);

	int i = (int)( arg->arg );
	int uid = sub_00000FC4( i );
	int ret;
	if( uid < 0)
	{
		ret = uid;
	}
	else
	{
		ret = sceIoRead( uid , data , len );
	}
	
	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_000012A8
int flashfat_write2( ReadParams *read_params )
{
	PspIoDrvFileArg *arg = read_params->arg;
	char *data = read_params->data;
	int len = read_params->len;

	sceKernelWaitSema( flashfat_sema , 1, NULL);

	int i = (int)( arg->arg );
	int uid = sub_00000FC4( i );
	int ret;
	if( uid < 0)
	{
		ret = uid;
	}
	else
	{
		if( !data && (len == 0) )
			ret = 0;
		else
			ret = sceIoWrite( uid , data , len );	
	}

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_00001218
u32 flashfat_lseek2( LseekParams *lseek_params )
{
	PspIoDrvFileArg *arg = lseek_params->arg;
	SceOff ofs = (SceOff)(lseek_params->ofs);
	int whence = lseek_params->whence;

	sceKernelWaitSema( flashfat_sema , 1, NULL);

	int i = (int)( arg->arg );
	int uid = sub_00000FC4( i );
	if( uid >= 0)
	{
		int ret = sceIoLseek( uid , ofs , whence);
		uid = ret;
	}

	sceKernelSignalSema( flashfat_sema , 1);
	return uid;
}

//sub_00001A10
int flashfat_remove2(RemoveParams *remove_params)
{
	const char *filename = remove_params->filename;

	wait_ms();
	
	sceKernelWaitSema( flashfat_sema , 1, NULL);
	sub_000015DC( filename );

	int ret = sceIoRemove( longpath_buff );

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_00001990
int flashfat_mkdir2( MkdirParams *mkdir_params )
{
	const char *dirname = mkdir_params->dirname;
	SceMode mode = mkdir_params->mode;

	wait_ms();
	sceKernelWaitSema( flashfat_sema , 1, NULL);
	sub_000015DC( dirname );

	int ret = sceIoMkdir( longpath_buff , mode );

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_00001924
int flashfat_rmdir2(RemoveParams *remove_params)
{
//	PspIoDrvFileArg *arg = remove_params->arg;
	const char *filename = remove_params->filename;

	wait_ms();
	sceKernelWaitSema( flashfat_sema , 1, NULL);

	sub_000015DC( filename );
	int ret = sceIoRmdir( longpath_buff );

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_00001808
int flashfat_dopen2(DopenParams *dopen_params)
{
	PspIoDrvFileArg *arg = dopen_params->arg;
	const char *dirname = dopen_params->dirname;

	sceKernelWaitSema( flashfat_sema , 1, NULL);
	sub_000015DC( dirname );

	int i;
	int ret = 0x80010018;

	for(i=0;i<32;i++)
	{
		if( open_info[i].index == 0 )
		{
			u32 value;
			if( ( value = sub_00000DD0( i , longpath_buff ,  0xD0D0  , 0  )) < 0)
				ret = value;
			else
			{
				arg->arg = (void *)i;
				ret = 0;
			}

			break;
		}
	}

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_00001160
int flashfat_dclose2(PspIoDrvFileArg *arg)
{
	sceKernelWaitSema( flashfat_sema , 1, NULL);

	int i = (int)( arg->arg );
	int ret = 0;
	int uid = sub_00000FC4( i );
	if( uid < 0)
	{
		ret = uid;
	}
	else
	{
		int res = sceIoDclose( uid );
		if( res < 0)
			ret = res;
		else	
			open_info[i].index = 0;
	}
	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_000010BC
int flashfat_dread2(DreadParams *dread_params)
{
	PspIoDrvFileArg *arg =  dread_params->arg;
	SceIoDirent *dirent = dread_params->dirent;

	sceKernelWaitSema( flashfat_sema , 1, NULL);

	int i = (int)( arg->arg );
	int ret;
	int uid = sub_00000FC4( i );
	if( uid < 0)
		ret = uid;
	else	
		ret = sceIoDread( uid , dirent );	

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_00001788
int flashfat_getstat2(GetStatParams *getstat_params )
{
//	PspIoDrvFileArg *arg = getstat_params->arg;
	const char *file = getstat_params->file;
	SceIoStat *stat = getstat_params->stat;

	wait_ms();
	sceKernelWaitSema( flashfat_sema , 1, NULL);

	sub_000015DC( file );
	int ret = sceIoGetstat( longpath_buff , stat );

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_000016F8
int flashfat_chstat2(ChStatParams *chstat_params)
{
//	PspIoDrvFileArg *arg = chstat_params->arg;
	const char *file = chstat_params->file;
	SceIoStat *stat = chstat_params->stat;
	int bits = chstat_params->bits;

	wait_ms();
	sceKernelWaitSema( flashfat_sema , 1, NULL);

	sub_000015DC( file );
	int ret = sceIoChstat( longpath_buff , stat , bits );

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_0000167C
int flashfat_rename2(RenameParams *rename_params)
{
//	PspIoDrvFileArg *arg = rename_params->arg;
	const char *oldname = rename_params->oldname;
	const char *newname = rename_params->newname;

	wait_ms();
	sceKernelWaitSema( flashfat_sema , 1, NULL);

	sub_000015DC( oldname );
	int ret = sceIoRename( oldname , newname );

	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

//sub_00001610
int flashfat_chdir2( DopenParams *chdir_params )
{
//	PspIoDrvFileArg *arg = chdir_params->arg;
	const char *dirname = chdir_params ->dirname;

	wait_ms();
	sceKernelWaitSema( flashfat_sema , 1, NULL);
	sub_000015DC( dirname );
	int ret = sceIoChdir( longpath_buff );
	sceKernelSignalSema( flashfat_sema , 1);
	return ret;
}

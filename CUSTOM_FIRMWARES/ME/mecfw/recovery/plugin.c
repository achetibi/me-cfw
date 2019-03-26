

#include <pspkernel.h>
//#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "text.h"

extern char str_buffer[];

static int ReadLine(char *read_buff , int max_size , char** real_path ,char* display_str , int api_type , int *flag);

static const char * menu_plugins[] =
{
	"VSH",
	"GAME",
	"POPS"
};

#define TEXT_CNT 6
static const char * plugin_path[TEXT_CNT] =
{
	"ms0:/seplugins/vsh.txt",
	"ms0:/seplugins/game.txt",
	"ms0:/seplugins/pops.txt",
	"ef0:/seplugins/vsh.txt",
	"ef0:/seplugins/game.txt",
	"ef0:/seplugins/pops.txt",
};

static void *p_malloc(u32 size)
{
  u32 *p;
  SceUID h_block;

  if(size == 0)
    return NULL;

  h_block = sceKernelAllocPartitionMemory(2, "block", 0, size + sizeof(h_block), NULL);

  if(h_block <= 0)
    return NULL;

  p = (u32 *)sceKernelGetBlockHeadAddr(h_block);
  *p = h_block;

  return (void *)(p + 1);
}

static s32 p_mfree(void *ptr)
{
  return sceKernelFreePartitionMemory((SceUID)*((u32 *)ptr - 1));
}


#define PLUGIN_MAX_CNT 128
void* plugin_manager(int type)
{
	if(type == FUNC_GET_STR)
	{	
		my_sprintf(str_buffer ,"%s ->", recovery_str.Plugins );
		return str_buffer;
	}

	SceUID fd;

	u8 plug_cnt[TEXT_CNT];
	int i,j,k;
	int total_cnt;
	int ret;
	int buff_offset;
	int dispstr_offset;

	char buffer[128];

	char *pugin_buff[TEXT_CNT];

	char *real_path[ PLUGIN_MAX_CNT ];
	char *display_str[ PLUGIN_MAX_CNT ];

	int plugin_stat[ PLUGIN_MAX_CNT ];
	int plugin_stat_backup[ PLUGIN_MAX_CNT ];

	Menu_pack plugin_pack[ PLUGIN_MAX_CNT + 1];

	memset( plug_cnt , 0 , sizeof(plug_cnt) );
	memset( pugin_buff , 0 , sizeof(pugin_buff) );
	memset( real_path , 0 , sizeof(real_path) );
	memset( display_str , 0 , sizeof(display_str) );
	memset( plugin_stat , 0 , sizeof(plugin_stat) );
	memset( plugin_pack , 0 , sizeof(plugin_pack) );

	for(j = 0;j< TEXT_CNT;j++)
	{
		total_cnt = 0;
		for(i=0;i<TEXT_CNT;i++)
		{
			total_cnt += plug_cnt[i];
		}

//		printf("total_cnt %d\n", total_cnt);

		fd= sceIoOpen( plugin_path[j], PSP_O_RDONLY, 0777);
		if (fd >= 0)
		{
			if( ( pugin_buff[j] = p_malloc( 2048 ) ) != 0 )
			{
				int max_size = sceIoRead(fd , pugin_buff[j] , 1024);//

				dispstr_offset = max_size + 1;
				buff_offset = 0;

				for(i = 0; (i + total_cnt) < PLUGIN_MAX_CNT ; i++)
				{
					display_str[total_cnt+i] = pugin_buff[j] + dispstr_offset;

					ret = ReadLine( pugin_buff[j] + buff_offset , max_size 
						, &(real_path[total_cnt+i]), display_str[total_cnt+i], j /* menu_plugins[j] */ , &(plugin_stat[total_cnt+i]) );

//					printf("cnt %d \n", (i + total_cnt));	
					if(ret > 0)
					{
						max_size -= ret;
						buff_offset += ret;

						dispstr_offset += strlen( display_str[total_cnt+i] ) + 1;

						plug_cnt[j]++;
					}
					else
					{
//						printf("return 0\n");
						break;
					}

					if( dispstr_offset >= 2048)
						break;
				}	
			}
			sceIoClose(fd);
		}
	}
	
	total_cnt = 0;
	for(i=0;i<TEXT_CNT;i++) {
		total_cnt += plug_cnt[i];
	}

//	total_cnt = (plug_cnt[0]+plug_cnt[1]+plug_cnt[2]);
//	printf("total_cnt:%d\n" ,total_cnt);

	memcpy( plugin_stat_backup , plugin_stat , sizeof(plugin_stat) );

	for(i=0;i<total_cnt;i++)
	{
				
//		printf("%s\n",real_path[i]);
//		printf("%s\n",display_str[i]);

//		sceKernelDelayThread(2 *1000*1000);

//		plugin_pack[i] = { display_str[i] , TMENU_SWITCH , &(plugin_stat[i])};
		plugin_pack[i].path = display_str[i];
		plugin_pack[i].type = TMENU_SWITCH;
		plugin_pack[i].value = &(plugin_stat[i]);

	}

	plugin_pack[i].path = NULL ;
	plugin_pack[i].value = "";


	DrawMenu( plugin_pack , 1 , recovery_str.Plugins );

	send_msg( recovery_str.Saving );
	printf(" ... ");

	
	int save_cnt = 0;	
	int change_flag;
	for(j=0;j < TEXT_CNT;j++)
	{
		if( plug_cnt[j] > 0 )//&& (nos+plug_no[j] >cpos) && nos<=cpos )		
		{
			change_flag=0;
		
			for(k=0;k<plug_cnt[j];k++)		
			{		
				if( plugin_stat[save_cnt+k] != plugin_stat_backup[save_cnt+k])
				{		
					change_flag=1;
					break;
				}
			}
				
			if(change_flag)
			{
				SceUID fd = sceIoOpen(plugin_path[j], PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
				for(i=0;i<plug_cnt[j];i++)
				{
					my_sprintf(buffer,"%s %d\r\n", real_path[save_cnt+i],plugin_stat[save_cnt+i]);
					sceIoWrite(fd, buffer, strlen(buffer));
				}
				sceIoClose(fd);
			}

			save_cnt += plug_cnt[j];
		}
	}

	for(j = 0;j<TEXT_CNT;j++)
	{
		if(pugin_buff[j] != 0)
			p_mfree( pugin_buff[j] );
	}

	printf( recovery_str.Done );
//	sceKernelDelayThread(1*1000*1000);
//	myDebugScreenClear();
	return NULL;
}


static int FixLen(char *str)
{
	int i;
	int len = strlen(str);
	
	if (len <=0)
		return len;

	int pos=len-1;

	for(i=0;i<pos;i++)
	{
		if(str[pos-i]==0x20 || str[pos-i]==0x09)
			str[pos-i]='\0';
		else
			break;
	}


	return len-i;

}


static int ReadLine(char *read_buff , int max_size , char** real_path ,char* display_str , int api_type , int *flag)
{
	char ch = 0;
	int read_point = 0;
//	int return_size = 0;
	int output_len=0;
	//a3=0;

	*flag=0;
	*real_path = NULL;

	while(1)
	{	
		
//		if (sceIoRead(fd, &ch, 1) != 1)	
//			break;

		if( max_size <= read_point )
		{
			break;
		}

		ch = read_buff[ read_point ];

		if (ch < 0x20 )
		{
			if( output_len ==0)
			{
				read_point++;
//				return_size++;
				continue;		
			}

			if(ch != 0x09)//tab
				break;
		}

		if( output_len ==0 )
			*real_path = read_buff + read_point;

//		output[output_len]=ch;

		output_len++;
		read_point++;
//		return_size++;
	}

	if(output_len)
	{
		char* edit_buff = real_path[0];
		edit_buff[output_len] = '\0';

		char ten = edit_buff[output_len - 1];
		if( ten == 0x31)
		{
			*flag=1;
			edit_buff[output_len - 1]='\0';
		}else if( ten == 0x30)
		{
			edit_buff[output_len - 1]='\0';
		}

		//fix len
		FixLen(edit_buff);
		
		char* p = strrchr( edit_buff, '/');	
		if (p)					
		{
		//	strcpy( display_str , p+1);						
			my_sprintf(display_str,( api_type > 2 )?"%s [%s-ef0:]":"%s [%s]",p + 1,  menu_plugins[ api_type % 3 ] );	
		}					

	}
	else
	{
		read_point = 0;
	}

	return read_point;

}
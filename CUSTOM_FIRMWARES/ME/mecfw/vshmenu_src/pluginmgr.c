
#include "common.h"

const char *text_back = "Back";

extern char * enable_disable[];
extern const int xyPoint[];
extern const int xyPoint2[];
extern int pwidth;

static int menu_sel = 0;
static int menus = 0;
static u8 *plugcon;

#define MAX_PAGE_CNT 20

int max_menu;
int menu_offset;
int multipage_flag = 0;

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

static const char * menu_plugins[] =
{
	"VSH",
	"GAME",
	"POPS"
};

static int pmenu_ctrl(u32 buttons,u32 button_on)
{
	int direction;

	if( (button_on&PSP_CTRL_SELECT) ||
		(button_on&PSP_CTRL_HOME))
	{
		menu_sel = menus;
		return -1;
	}

	//L & R
	direction = 0;
	if(button_on&PSP_CTRL_RTRIGGER) direction++;
	if(button_on&PSP_CTRL_LTRIGGER) direction--;
	menu_offset = limit(menu_offset + direction , 0 , multipage_flag );
	u32 i = max_menu - (menu_offset * MAX_PAGE_CNT);
	if( i < MAX_PAGE_CNT )
		menus = i;
	else
		menus = MAX_PAGE_CNT;

	// change menu select
	direction = 0;
	if(button_on&PSP_CTRL_DOWN) direction++;
	if(button_on&PSP_CTRL_UP) direction--;
	menu_sel = limit(menu_sel+direction , 0 , menus);


	// LEFT & RIGHT
	direction = -2;
	if(button_on&PSP_CTRL_LEFT)   direction = -1;
	if(button_on&PSP_CTRL_CROSS) direction = 0;
	if(button_on&PSP_CTRL_RIGHT)  direction = 1;
	if(direction>-2)
	{
		if(menu_sel ==menus)	
		{
			if(direction==0)
			{
				return -1;
			}
		}
		else
		{
			u32 offset = (menu_offset * MAX_PAGE_CNT);
			plugcon[menu_sel + offset ]= (plugcon[menu_sel + offset ])? 0:1;			
		}
	}

	return 0;
}

static int FixLen(char *str , u8* cnf)
{
	int i;
	int len = scePaf_strlen(str);

			
	char ten = str[len - 1];
		
	if( ten == 0x31)	
	{
		*cnf = 1;	
		str[len - 1] ='\0';	
	}		
	else if( ten == 0x30)			
	{
		str[len - 1] = '\0';	
	}

	len = scePaf_strlen(str);
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

static int ReadBuff( char *buff , u32 size , char **fullpath , u8 *cnf )
{
	char *buff_end = (char *)(buff + size);
	u8 ch;
	int flag = 0;

	while( buff <  buff_end)
	{
		ch = buff[0];
			
		if (ch < 0x20 && ch != 0x09)
		{
			if( flag == 1)
			{			
				buff[0] = 0;

				//fix len
				FixLen(fullpath[0] , cnf);
				fullpath ++;
				cnf ++;

				flag = 0;
			}

		}
		else
		{
			if( flag == 0)
			{
				flag = 1;
				fullpath[0] = (char *)buff;
				//fullpath ++;
			}
		}

		buff ++;
	}

	if( flag == 1)
		FixLen(fullpath[0] , cnf);

	return 0;
}


static u32 get_plugin_cnt(u8 *buff ,u32 max )
{
	u8 *buff_end = buff + max;
	u32 cnt = 0;
	u8 ch;
	int flag = 0;

	while( buff <  buff_end)
	{
		ch = buff[0];
			
		if (ch < 0x20 && ch != 0x09)
		{
			if( flag == 1)
			{
				flag = 0;
			}
		}
		else
		{
			if( flag == 0)
			{
				flag = 1;
				cnt ++;
			}
		}

		buff ++;
	}

	return cnt;
}

int menu_plugin(u32 *buttons,u32 *button_on)
{
	SceUID fd; 
	u8 i,j,k;
	int p,p2;
	u8	nos=0;
	char buffer[128];
	u32 plug_no[TEXT_CNT] = {0,};
	int *pointer;

	char *buff_list[TEXT_CNT];// = { vsh_buff , game_buff , pops_buff };

	for(j=0;j<TEXT_CNT;j++)
		buff_list[j] = (char *)scePaf_malloc( 1024 );

	for(j=0;j<TEXT_CNT;j++)
	{
		if( !buff_list[j] )
			return 0;
	}

	for(j=0;j<TEXT_CNT;j++)
		scePaf_6439FDBC_memset( buff_list[j] , 0, 1024 );

	u32 buffsize_list[TEXT_CNT];

	for(j = 0;j<TEXT_CNT;j++)
	{
		fd= sceIoOpen(plugin_path[j], PSP_O_RDONLY, 0777);
		if (fd >= 0)		
		{
			buffsize_list[j] = sceIoRead( fd , buff_list[j] , 1024 );
			sceIoClose(fd);

			if( buffsize_list[j] > 0)
				plug_no[j] = get_plugin_cnt( (u8 *)(buff_list[j]) , buffsize_list[j]);
			//else
			//	plug_no[j] = 0;

		}

	}
	
	menus = 0;
	for(j=0;j<TEXT_CNT;j++)
	{
		menus += plug_no[j];
	}

//	menus=(plug_no[0]+plug_no[1]+plug_no[2]);

	plugcon		= (u8*)scePaf_malloc( menus  );		
	u8 *plugcon_old	= (u8*)scePaf_malloc( menus  );
	char **plugins_full = (char **)scePaf_malloc( menus * 4 );
	char *plugins = (char *)scePaf_malloc( menus * 64 );

	scePaf_memset(plugcon, 0, menus );
	scePaf_memset(plugins, 0, menus * 64 );
	scePaf_memset(plugins_full,0, menus * 4 );

	u32 cnt= 0;

	for(j = 0;j<TEXT_CNT;j++)
	{
		if(plug_no[j])
		{		
			ReadBuff( buff_list[j] , buffsize_list[j] , plugins_full + cnt , plugcon + cnt );
		}

		cnt += plug_no[j];
	}


	j = 0;
	cnt = plug_no[0];

	for(i=0;i<menus;i++)
	{			

/*		if( i >= cnt )
		{
			j++;
			cnt += plug_no[j];
		}
*/
		while( i >= cnt )
		{
			j++;
			cnt += plug_no[j];
		}

		p = scePaf_strlen(plugins_full[i]);
		p2=0;

		for(k=0;k<p;k++)
		{
			if(plugins_full[i][p-k-1] == '/')		
			{
				p2=p-k-1;			
				break;
			}
		}
				
		if (p2)	
		{
			char *path_offset = plugins + (i*64);
			scePaf_strcpy( path_offset, &plugins_full[i][p2+1]);		
			scePaf_sprintf(buffer,( j > 2)?"%s [%s-ef0:]":"%s [%s]", path_offset , menu_plugins[j % 3]);
			scePaf_98DE3BA6_strcpy( path_offset, buffer);
		}

	}

	menu_sel=0;


	scePaf_6BD7452C_memcpy(plugcon_old, plugcon , menus );

	u32 fc,bc;
	max_menu  = menus;

	menu_offset = 0;
	if( MAX_PAGE_CNT < max_menu )
	{
		multipage_flag = (max_menu / MAX_PAGE_CNT);
		menus = MAX_PAGE_CNT;
	}
	else
	{
		menus = max_menu;
	}

	while(1)
	{

		if( sceDisplayWaitVblankStart() < 0)
			break; // end of VSH ?


		if( blit_setup() < 0) return -1;

		if(pwidth==720)
		{
			pointer = (int *)xyPoint;
		}
		else
		{
			pointer = (int *)xyPoint2;
		}
	

		blit_set_color(0xffffff,0x8000ff00);
		blit_string( /*pointer[0]-*/pointer[4] -32, 0x30  ,"Plugin Manager v2.1");

		int offset = menu_offset * MAX_PAGE_CNT;
		for(cnt = 0;cnt <  menus +1 ;cnt++)
		{
			fc = 0xffffff;
			bc = (cnt==menu_sel) ? 0xff8080 : 0xc00000ff;
			blit_set_color(fc,bc);

			if( cnt == menus)
			{
				blit_string( pointer[4] -32 ,(pointer[5] + cnt +1 ) << 3, text_back);
			}
			else
			{

				blit_string( pointer[4] -32  ,(pointer[5] + cnt ) << 3 , plugins + ( (cnt + offset) << 6 ) );
				//blit_string( pointer[4] ,(pointer[5] + cnt)*8, plugins_full[ cnt ]);
				
				//blit_set_color(item_fcolor[cnt],bc);
				blit_string( (pointer[6] << 3) + 128+ 64 , (pointer[5] + cnt ) << 3 , enable_disable[ 1 - plugcon[cnt + offset]] );	
			}

		}
		
		blit_set_color(0x00ffffff, 0xc00000ff );

		if(  multipage_flag )	
		{
			blit_string( pointer[4] + 0x118 , 0x96 , "R ->");
			blit_string( pointer[4] - 0x64 , 0x96 , "<- L");
		}

		if(pmenu_ctrl( buttons[0], button_on[0]) < 0)
		{

			//save
			nos=0;
			int change_flag;
			for(j=0;j<TEXT_CNT;j++)
			{
				if(plug_no[j] > 0 )//&& (nos+plug_no[j] >cpos) && nos<=cpos )
				{
					change_flag=0;

					for(k=0;k<plug_no[j];k++)
					{
						if(plugcon[nos+k] != plugcon_old[nos+k])
						{
							change_flag=1;
							break;
						}
					}

					if(change_flag)
					{
						SceUID fd = sceIoOpen(plugin_path[j], PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);	
						for(i=0;i<plug_no[j];i++)
						{
							scePaf_sprintf(buffer,"%s %d\r\n",plugins_full[nos+i],plugcon[nos+i]);
							sceIoWrite(fd, buffer, scePaf_strlen(buffer));
						}
						sceIoClose(fd);			
					}	

					nos+=plug_no[j];		
				}		
			}
			
			break;
			
		}

	}


	for(j=0;j<TEXT_CNT;j++)
		scePaf_free( buff_list[j]);

	scePaf_free( plugcon_old );
	scePaf_free( plugcon );

	scePaf_free( plugins_full );
	scePaf_free( plugins );
//	scePaf_free();
//	scePaf_free();

	return 0;
}
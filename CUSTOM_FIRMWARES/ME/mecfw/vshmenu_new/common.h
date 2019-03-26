#include <pspkernel.h>
//#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "systemctrl_se.h"
#include "systemctrl_me.h"
#include "kubridge.h"


#include "ui.h"
#include "text.h"

#include "blit.h"

enum{
	TMENU_DUMMY,
	TMENU_FUNCTION,
	TMENU_FUNCTION_VALUE,
	TMENU_FUNCTION_EXIT,
//	TMENU_SUB_MENU,
	TMENU_SWITCH,
	TMENU_EXIT,
};

enum{
	FUNC_GET_STR,
	FUNC_CHANGE_VALUE
};

typedef struct
{
	char *path;
	int type;
	void *value;
	int draw_type;
}Menu_pack;


typedef struct
{
	char isofile[72];//	
	int type;
	ScePspDateTime mtime;//
} ISO_cache;

typedef struct {
	char FileName[13];
	char LongName[256];
}SceFatMsDirentPrivate;

typedef struct {
	SceSize size;
	char FileName[16];
	char LongName[1024];
}SceFatMsDirentPrivateKernel;

int menu_draw( const Menu_pack *menu_list, int menu_max);
int menu_setup(void);
int menu_ctrl( const Menu_pack *menu_list, int *switch_list,int menu_max_cnt);

u32 get_buttons();

int menu_plugin();

char *GetVideoName(void);
int GetVideoType(void);
void GetVideoPath(char *out, const char *input);
void *change_umd_video(int type ,int dir );
int init_videoCache(const char *iso_name);


void *change_xmb_clock(int type, int dir );
void *change_game_clock(int type, int dir );
void *change_usb(int type, int dir );
void *change_umd_mode(int type ,int dir );
void change_plugins(int dir , int flag);

void *change_back_color(int type ,int dir );

void request_shutdown();
void request_suspend();
void request_reset_device();
void request_reset_vsh();
void request_enter_recovery();


int scePowerRequestColdReset(int);


/*
#define scePaf_967A56EF_strlen strlen
#define scePaf_6439FDBC_memset memset
#define scePaf_B6ADE52D_memcmp memcmp
#define scePaf_11EFC5FD_sprintf sprintf
#define scePaf_15AFC8D3_snprintf snprintf
#define scePaf_6BD7452C_memcpy memcpy
#define scePaf_98DE3BA6_strcpy strcpy
*/

#define scePaf_967A56EF_strlen scePaf_strlen
#define scePaf_6439FDBC_memset scePaf_memset
#define scePaf_B6ADE52D_memcmp scePaf_memcmp
#define scePaf_11EFC5FD_sprintf scePaf_sprintf
#define scePaf_15AFC8D3_snprintf scePaf_snprintf
#define scePaf_6BD7452C_memcpy scePaf_memcpy
#define scePaf_98DE3BA6_strcpy scePaf_strcpy


int scePaf_967A56EF_strlen(const char *path);
int scePaf_6439FDBC_memset(void *buff ,int c ,int size);
int scePaf_B6ADE52D_memcmp(const void *path , const void *name , int c);
int scePaf_11EFC5FD_sprintf(char *buffer , const char *format , ...);
int scePaf_15AFC8D3_snprintf(char *buffer,int c , const char *format, ...);
int scePaf_6BD7452C_memcpy(void *path , const void *name , int size);
int scePaf_98DE3BA6_strcpy(char *path , const char *name);

char *scePaf_strtok_r(char *str, const char *delim, char **saveptr);

void *scePaf_malloc(int size);
void scePaf_free(void *);


#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "systemctrl_me.h"

#include "kernel/bridge_func.h"
#include "mydebug.h"

#define printf myDebugScreenPrintf
#define normal_color	0x00FFFFFF


enum{
	TMENU_DUMMY,
	TMENU_FUNCTION,
	TMENU_FUNCTION_DISPLAY,
	TMENU_FUNCTION_VALUE,
	TMENU_FUNCTION_VALUE_DISPLAY,
	TMENU_SUB_MENU,
	TMENU_SWITCH,
	TMENU_SWITCH2
};

enum{
	FUNC_GET_STR,
	FUNC_GET_STR2,
	FUNC_CHANGE_VALUE,
	FUNC_GET_INT
};

typedef struct
{
	char *path;
	int type;
	void *value;
}Menu_pack;


//main.c
void save_config();
u32 ctrlRead();

//menu.c
void set_select_color(u32 color );
void send_msg(const char *msg );
void draw_init();
int DrawMenu(Menu_pack *menu_desu ,u8 sub ,const char *sub_title);

//lib.c
int limit(int val,int min,int max);
int limit2(int val,int min,int max);
int my_vsprintf(char *out,const char *format, va_list arg);
int my_sprintf(char *s, const char *format, ... );
int itoa(int n, char* buf );
char *my_strtok_r(char *s1, const char *s2 , char **s3 );

int ReadFile(const char *file, void *buf, int size);
int WriteFile(const char *file, void *buf, int size);

int vshLflashFatfmtStartFatfmt(int argc, char *argv[]);
int vshKernelGetModel(void);
int vshKernelExitVSHVSH(void *);

int getRegistryValue(const char *dir, const char *name, u32 *val);
int setRegistryValue(const char *dir, const char *name, u32 val);


#endif

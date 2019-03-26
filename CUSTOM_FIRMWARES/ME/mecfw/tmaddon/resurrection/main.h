#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "systemctrl_me.h"

#include "mydebug.h"

#define printf myDebugScreenPrintf
#define normal_color	0x00FFFFFF


enum{
	TMENU_FUNCTION_RET,
	TMENU_FUNCTION,
	TMENU_SUB_MENU
};

typedef struct
{
	char *path;
	int type;
	void *value;
}Menu_pack;

typedef struct U64
{
	u32 low;
	u32 high;
} U64;

//main.c
u32 ctrlRead();

void sub_01544 ( void );

//menu.c
void set_select_color(u32 color );
void send_msg(const char *msg );
void draw_init();
int DrawMenu(Menu_pack *menu_desu ,u8 sub ,char *sub_title);

//lib.c
int limit(int val,int min,int max);
int limit2(int val,int min,int max);

int ReadFile(const char *file, void *buf, int size);
int WriteFile(const char *file, void *buf, int size);

void *malloc64(u32 size);
extern s32 p_mfree(void *ptr);

int mallocate_buffer();
void free_buffer(void);
void install_fw(const char *update_path, int ofw);

//missing in psppower.h
int scePowerRequestColdReset(int);

#endif

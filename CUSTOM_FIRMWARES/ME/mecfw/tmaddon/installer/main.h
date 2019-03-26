#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "systemctrl_me.h"

#define normal_color	0x00FFFFFF

enum{
	TMENU_FUNCTION
};

typedef struct
{
	char *path;
	int type;
	void *value;
}Menu_pack;

//main.c
u32 ctrlRead();

void sub_01544 ( void );

//menu.c
int DrawMenu(Menu_pack *menu_desu ,u8 sub);

//lib.c
int limit(int val,int min,int max);
int limit2(int val,int min,int max);

int ReadFile(const char *file, void *buf, int size);
int WriteFile(const char *file, void *buf, int size);

#endif

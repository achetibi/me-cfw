/*****************************************
 * Recovery - mydebug			 *
 * 			by harleyg :)	 *
 *****************************************/

#ifndef __MYDEBUG_H__
#define __MYDEBUG_H__

#include <psptypes.h>
#include <pspmoduleinfo.h>

#ifdef __cplusplus
extern "C" {
#endif

void myDebugScreenInit(void);

void myDebugScreenPrintf(const char *fmt, ...) __attribute__((format(printf,1,2)));

void myDebugScreenSetBackColor(u32 color);
void myDebugScreenSetTextColor(u32 color, u32 back);

void myDebugScreenPutChar(int x, int y, u32 color, u8 ch);

void myDebugScreenSetXY(int x, int y);
int myDebugScreenGetX(void);
int myDebugScreenGetY(void);

void myDebugScreenSetOffset(int offset);

void myDebugScreenClear(void);
void myDebugScreenClearLineWithColor(u32 color);

int myDebugScreenPrintData(const char *buff, int size);

int myDebugLoadExtraFont(const char *path);

/*
#define pspDebugScreenInit myDebugScreenInit
#define pspDebugScreenPrintf myDebugScreenPrintf
#define pspDebugScreenSetBackColor myDebugScreenSetBackColor
#define pspDebugScreenSetTextColor myDebugScreenSetTextColor
#define pspDebugScreenPutChar myDebugScreenPutChar
#define pspDebugScreenSetXY myDebugScreenSetXY
#define pspDebugScreenSetOffset myDebugScreenSetOffset
#define pspDebugScreenGetX myDebugScreenGetX
#define pspDebugScreenGetY myDebugScreenGetY
#define pspDebugScreenClear myDebugScreenClear
*/
#ifdef __cplusplus
}
#endif

#endif

#ifndef __VLFUTILS_H__
#define __VLFUTILS_H__

void MainMenu(int sel);
int OnBackToMainMenu(int enter);

void ErrorReturn(int handler, int pg_ctrl, char *fmt, ...);
void SetStatus(int x, int y, int alignment, char *fmt, ...);
void SetProgress(int percentage, int force);
void SetGenericProgress(int value, int max, int force);
void SetInstallProgress(int value, int max, int force, int ofw);
void InitProgress(int pb, int pt, int st, int x, int y, int alignement, char *fmt, ...);
void ResetScreen(int showmenu, int showback, int sel);
void AddWaitIcon();
void RemoveWaitIcon();
void SetTitle(char *src, char *name, char *fmt, ...);
void RemoveBackgroundHandler(int (* func)(void *), void *param);
void AddBackgroundHandler(int button, int (* func)(void *), void *param);
int SetBackground(void *param);
VlfText vlfGuiAddUnicodeText(int x, int y, char *fmt, ...);

#endif


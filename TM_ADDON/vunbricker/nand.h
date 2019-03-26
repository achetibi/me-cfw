#ifndef __NAND_H__
#define __NAND_H__

void *malloc64(int size);
int OnBackToMainMenu(int enter);

void DumpNand(char *fmt, ...);
void RestoreNand(char *fmt, ...);
void CheckNand();
void FormatNandPage();
int OnEditSpinOrBack(int enter);
int CallbackThread(SceSize args, void *argp);
void CreateIdstoragePage();
void ChangeRegion(int region);
void FixMac();
void DumpIdstorage(char *fmt, ...);
void RestoreIdstorage(char *fmt, ...);

#endif


/****************************************************************************
	memstkro.h

	PSP IPL MemoryStick Driver (read only)

note:
	Supported MsProDuo Only.

****************************************************************************/
#ifndef _MEMSTKRO_H
#define _MEMSTKRO_H

int pspMsInit(void);
int pspMsReadSector(unsigned int sector, void *addr);
int pspMsWriteSector(int sector, void *addr);

#endif
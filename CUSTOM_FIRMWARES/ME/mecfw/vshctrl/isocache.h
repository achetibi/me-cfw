#ifndef __ISO_CACHE_H__
#define __ISO_CACHE_H__

#include "virtualpbpmgr.h"

int ReadCache(int dev);
int SaveCache(int dev);
int IsCached(char *isofile, ScePspDateTime *mtime, VirtualPbp *res);
int Cache(VirtualPbp *pbp);

#endif

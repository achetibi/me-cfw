#include <pspsdk.h>
#include <psploadcore.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include <systemctrl_me.h>

#include "main.h"

static char *g_blacklist[] = {
	"iso",
	"seplugins",
	"isocache.bin",
	"irshell",
	"game150",
};

int strcase_cmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char *p1 = (const unsigned char *) s1;
	const unsigned char *p2 = (const unsigned char *) s2;
	unsigned char c1, c2;

	if (p1 == p2 || n == 0)
	{
		return 0;
	}

	do
	{
		c1 = tolower(*p1);
		c2 = tolower(*p2);

		if (--n == 0 || c1 == '\0')
		{
			break;
		}

		++p1;
		++p2;
	} while (c1 == c2);

	return c1 - c2;
}

static inline int IsBlacklisted(const char *dname)
{
	int i;

	for(i = 0; i < NELEMS(g_blacklist); ++i)
	{
		if(strcase_cmp(dname, g_blacklist[i], (u32)-1) == 0)
		{
			return 1;
		}
	}

	return 0;
}

int sceIoDreadPatched(SceUID fd, SceIoDirent * dir)
{
	int result = sceIoDread(fd, dir);

	if(result > 0 && IsBlacklisted(dir->d_name))
	{
		result = sceIoDread(fd, dir);
	}

	return result;
}

void HideCFWDirs(SceModule * mod)
{
	sctrlHookImportByNid(mod, "IoFileMgrForUser", 0xE3EB004C, &sceIoDreadPatched, 1);
}

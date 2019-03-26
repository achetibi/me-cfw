/*
	nid resolver
*/

#ifndef __NID_RESOLDER__
#define __NID_RESOLDER__


typedef struct
{
	u32 receive;
	u32 send;
} Nid_list;


typedef struct
{
	char *name;
	Nid_list *buf;
	int count;

} Module_list;

int LinkLibraryEntriesPatch(char* szMod, void *szLib );
int LinkLibraryEntriesUserPatch(char* szMod, void *szLib ,int a2 , int a3 );

void create_DNR_list(const char *libname , u32 text_addr );


#endif

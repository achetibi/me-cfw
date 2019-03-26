#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psploadexec_kernel.h>
#include <pspcrypt.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include "main.h"

#include "nid_resolver.h"

char *my_strtok_r(char *s1, const char *s2 , char **s3 );

void *malloc(size_t size)
{
	return sctrlKernelMalloc(size); 
}

void free(void *p)
{
	sctrlKernelFree(p);
}

int sctrlKernelUncompress(u8 *dest, int destLen, const u8 *source, int sourceLen)
{
	unsigned long dLen = destLen;
	return uncompress(dest, &dLen, source, (unsigned long)sourceLen);
}

int LoadStartModule(const char *module)
{
	SceUID modid = sceKernelLoadModule(module, 0, NULL);
	if (modid < 0)
	{
		return modid;
	}

#if PSP_MODEL == 4
	if(module[0] == 'e' && module[1] == 'f')
	{
		SceModule2 *mod = sceKernelFindModuleByUID(modid);
		if(mod != NULL)
		{
			int i;
			for(i = 0; i < mod->nsegment; i++)
			{
				u32 offset = mod->segmentaddr[i];
				u32 end = offset + mod->segmentsize[i];

				while(offset <  end)
				{
					char *str = (char *)offset;
					if (memcmp(str, "ms0", 3) == 0)
					{
						str[0] = 'e';
						str[1] = 'f';
					}
					else if (memcmp(str, "fatms", 5) == 0)
					{
						str[3] = 'e';
						str[4] = 'f';
					}

					offset += 4;
				}
			}

			ClearCaches();
		}
	}
#endif
	return sceKernelStartModule(modid, strlen(module) + 1, (void *)module, NULL, NULL);
}

void *search_module_export(SceModule2 *pMod, const char *szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	void *entTab;
	int entLen;
	int i = 0;

	entTab = pMod->ent_top;
	entLen = pMod->ent_size;

	while(i < entLen)
	{
		int count;
		int total;
		unsigned int *vars;

		entry = (struct SceLibraryEntryTable *) (entTab + i);

		if((entry->libname == szLib) || (entry->libname && strcmp(entry->libname, szLib) == 0))
		{
			total = entry->stubcount + entry->vstubcount;
			vars = entry->entrytable;

			if(total > 0)
			{
				for(count = 0; count < total ; count++)
				{
					if (vars[count] == nid)
					{
						return (void *)(vars[count+total]);
					}
				}
			}
		}

		i += (entry->len * 4);
	}

	return NULL;
}

u32 sctrlHENFindFunction(const char *szMod, const char *szLib, u32 nid)
{
	SceModule2 *pMod;
	u32 ret = 0;
	int k1 = pspSdkSetK1(0);

	pMod = sceKernelFindModuleByName(szMod);
	if (!pMod)
	{
		pMod = sceKernelFindModuleByAddress((u32 )szMod);
		if(!pMod)
		{
			pspSdkSetK1(k1);
			return 0;
		}
	}

	u32 get = sctrlKernelResolveNid(szLib, nid);
	if(get)
	{
		nid = get;
	}

	ret = (u32)search_module_export(pMod, szLib, nid);
	pspSdkSetK1(k1);

	return ret;
}

void sctrlHENPatchSyscall(u32 funcaddr, void *newfunc)
{
	u32	*ptr;
	int i;
	asm("cfc0 %0, $12\n" : "=r"(ptr));

	// printf("syscall 0x%08X  \n",funcaddr );
	funcaddr &= 0x0FFFFFFF;

	while(ptr[1])
	{
		u32	*tbl = ptr + 0x10/4;
		u32 size = 0x1000;//ptr[2] / 4;
		for (i = 0; i < size; i++)
		{
			if ((tbl[i] & 0x0FFFFFFF) == funcaddr)
			{
				// printf("syscall found cnt = 0x%08X ,0x%08X -> 0x%08X \n", i ,tbl[i] , newfunc);
				tbl[i] = (u32)newfunc;	
			}
		}

		ptr = (u32 *)(ptr[0]);
	}
} 

u32 sctrlKernelRand(void)
{
	u32 k1 = pspSdkSetK1(0);

	u8 *alloc = sctrlKernelMalloc(20 + 4);
	if(alloc == NULL)
	{
		asm("break");
	}

	/* output ptr has to be 4 bytes aligned */
	u8 *ptr = (void*)(((u32)alloc & (~(4-1))) + 4);
	sceUtilsBufferCopyWithRange(ptr, 20, NULL, 0, 0xE);
	u32 result = *(u32*)ptr;
	sctrlKernelFree(alloc);
	pspSdkSetK1(k1);

	return result;
}

static int FixLen(char *str, int len)
{
	int i;
	int pos = len - 1;

	for(i = 0; i < pos; i++)
	{
		if(str[pos - i] == 0x20 || str[pos - i] == 0x09)
		{
			str[pos - i] = '\0';
		}
		else
		{
			break;
		}
	}

	return len - i;
}

static int LoadPlugins(const char * txt_path)
{			
	SceUID fd;
	int i = 0;

	while((fd = sceIoOpen(txt_path, PSP_O_RDONLY, 0)) < 0)
	{
		if(i > 16)
		{
			return -1;
		}

		sceKernelDelayThread(20000);
		i++;
	}

	char *list_buff;

	int fpl =sceKernelCreateFpl("", 1, 0, 1024, 1, NULL);
	if(fpl >= 0)
	{
		sceKernelAllocateFpl(fpl, (void *)&list_buff, NULL);
		memset(list_buff, 0, 1024);
		
		sceIoRead(fd, list_buff, 1024);
		list_buff[1024 - 1] = '\0';

		char *token;
		char *saveptr1 = NULL;

		for(token = my_strtok_r(list_buff, "\r\n", &saveptr1); token != NULL; token = my_strtok_r(NULL, "\r\n", &saveptr1))
		{
			int size = strlen(token);
			if(token[size - 1] == '1')
			{
				token[size - 1] = '\0';
				size --;

				FixLen(token, size);
				LoadStartModule(token);
			}
		}
	}

	sceIoClose(fd);

	sceKernelFreeFpl(fpl, list_buff);
	sceKernelDeleteFpl(fpl);

	return 0;
}

static int ms_callback = 0;
//int np_flag = 0;
int sub_0000684C(int arg1, int arg2, void *arg)
{
//	np_flag = arg2;
	sceKernelSetEventFlag(ms_callback, 1);
	return 0;
}

int WaitDevice(const char *device, const char *fat, const char *text_path)
{
	int ret;
	int value;

	if(fat)
	{
		if((ret = sceIoDevctl(device, 0x02025806, NULL, 0, &value, sizeof(int))) >= 0 && value == 1)
		{
			if(sceKernelFindModuleByName("sceNp9660_driver") == NULL)
			{
				ms_callback =sceKernelCreateEventFlag("", 0, 0, 0);		
				int call_handle = sceKernelCreateCallback("", sub_0000684C, NULL);

				if((ret = sceIoDevctl(fat, 0x02415821, &call_handle, 4, NULL, 0)) >= 0)
				{
					sceKernelWaitEventFlagCB(ms_callback, 1 , 17, 0 , 0);	
					sceIoDevctl(fat, 0x2415822, &call_handle, 4, NULL, 0);
				}

				sceKernelDeleteCallback(call_handle);
				sceKernelDeleteEventFlag(ms_callback);
			}
			
		}
		else
		{
			return -1;
		}
	}

	return LoadPlugins(text_path);
}

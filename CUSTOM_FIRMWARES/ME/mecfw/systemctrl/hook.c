#include <pspkernel.h>
#include <string.h>

#include <systemctrl_me.h>

u32 sceKernelQuerySystemCall(void *func);

PspModuleImport *sctrlFindImportLib(SceModule *mod, char *library)
{
	if(mod == NULL)
	{
		return NULL;
	}

	void *stubTab = mod->stub_top;
	int stubLen = mod->stub_size;
	int i = 0;

	while(i < stubLen)
	{
		PspModuleImport *pImp = (PspModuleImport*)(stubTab + i);

		if((pImp->name) && (strcmp(pImp->name, library) == 0))
		{
			return pImp;
		}

		i += (pImp->entLen * 4);
	}

	return NULL;
}

u32 sctrlFindImportByNid(SceModule *mod, char *library, u32 nid)
{
	PspModuleImport *pImp = sctrlFindImportLib(mod, library);

	if(pImp)
	{
		int i;

		for(i = 0; i < pImp->funcCount; i++)
		{
			if(pImp->fnids[i] == nid)
			{
				return (u32) &pImp->funcs[i * 2];
			}
		}
	}

	return 0;
}

int sctrlHookImportByNid(SceModule *mod, char *library, u32 nid, void *func, int syscall)
{
	if(mod == NULL)
	{
		return -1;
	}

	void *stubTab = mod->stub_top;
	int stubLen = mod->stub_size;
	int i = 0;

	while(i < stubLen)
	{
		PspModuleImport *pImp = (PspModuleImport*)(stubTab+i);

		if((pImp->name) && (strcmp(pImp->name, library) == 0))
		{
			int j;

			for(j = 0; j < pImp->funcCount; j++)
			{
				if(pImp->fnids[j] == nid)
				{
					void *addr = (void *)(&pImp->funcs[j * 2]);

					if(func == NULL)
					{
						_sw(0x03E00008, (u32)addr);
						_sw(0, (u32)(addr + 4));
					}
					else
					{
						if(syscall)
						{
							u32 syscall_num;

							syscall_num = sceKernelQuerySystemCall(func);

							if(syscall_num == (u32)-1)
							{
								return -1;
							}

							_sw(0x03E00008, (u32)addr);
							_sw(((syscall_num << 6) | 12), (u32)(addr + 4));
						}
						else
						{
							_sw((0x08000000 | (((u32)(func) >> 2) & 0x03FFFFFF)), (u32)addr);
							_sw(0, (u32)(addr + 4));
						}
					}

					sceKernelDcacheWritebackInvalidateRange(addr, 8);
					sceKernelIcacheInvalidateRange(addr, 8);
				}
			}
		}

		i += (pImp->entLen * 4);
	}

	return 0;
}

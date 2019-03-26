#include <pspkernel.h>
#include <vlf.h>

extern char *ebootpath;
extern int app_main(int argc, char *argv[]);

void LoadStartModule(char *path)
{
    SceUID mod = sceKernelLoadModule(path, 0, NULL);
	mod = sceKernelStartModule(mod, 0, NULL, NULL, NULL);
}

int start_thread(SceSize args, void *argp)
{
	ebootpath = (char *)argp;
	int last_trail = -1;
	int i;
	if(ebootpath)
	{
		for (i = 0; ebootpath[i]; i++)
		{
			if (ebootpath[i] == '/')
			{
				last_trail = i;
			}
		}
	}

	if(last_trail >= 0)
	{
		ebootpath[last_trail] = 0;
	}

	sceIoChdir(ebootpath);

	LoadStartModule("modules/iop.prx");
	LoadStartModule("modules/intraFont.prx");
	LoadStartModule("modules/vlf.prx");
	LoadStartModule("modules/kpspident.prx");

	vlfGuiInit(-1, app_main);

	return sceKernelExitDeleteThread(0);
}

int module_start(SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("start_thread", start_thread, 0x10, 0x4000, 0, NULL);
	if (thid < 0)
	{
		return thid;
	}

	sceKernelStartThread(thid, args, argp);
	
	return 0;
}
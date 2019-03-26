#include <pspkernel.h>

PSP_MODULE_INFO("installer", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#include "action.h"
#include "menu_struct.h"

void free_buffer(void);
int mallocate_buffer();

int main()
{
	pspDebugScreenInit();

	if (mallocate_buffer() < 0)
	{
		pspDebugScreenPrintf("buffer init error\n");
		Exit();
	}

	DrawMenu( menu_main , 0 );
	
	free_buffer();

	return 0;
}

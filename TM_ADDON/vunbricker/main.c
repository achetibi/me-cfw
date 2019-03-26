#include <pspsdk.h>
#include <pspkernel.h>
#include <psppower.h>
#include <pspctrl.h>
#include <systemctrl.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <vlf.h>

#include "nand.h"
#include "dcman.h"
#include "trans.h"
#include "common.h"
#include "install.h"
#include "pspinfo.h"
#include "vlfutils.h"
#include "../kpspident/main.h"

PSP_MODULE_INFO("VUnbricker", 0x800, 2, 5);
PSP_MAIN_THREAD_ATTR(0);

#define VLF_ITEMS 20
#define SMALL_BUFFER_SIZE 2100000
#define NELEMS(a) sizeof(a) / sizeof(a[0])

extern const char **g_messages;
extern char st_text[256];
extern VlfText vlf_texts[VLF_ITEMS];
extern VlfSpin vlf_spins[VLF_ITEMS];

extern int page_control;
extern int showback_prev;
extern int showenter_prev;

char file_path[64];
u8 *big_buffer, *sm_buffer1, *sm_buffer2;
int BIG_BUFFER_SIZE, selitem, selected_region, mode = 0, MEorPRO = 0, sel_reg;
int flash_sizes[4];
int totalflash_size;
int macrandom;

int PageControl(int page)
{
	vlfGuiSetPageControlEnable(1);
	vlfGuiNextPageControl(PageControl);
	vlfGuiPreviousPageControl(PageControl);

	// Install an CFW pagination
	if (mode == 0x00000001 || mode == 0x00000101 || mode == 0x00000201 || mode == 0x00000301)
	{
		switch(page)
		{	
			case 0:
				mode = 0x00000101;
				vlfGuiPreviousPageControl(NULL);
				vlfGuiCancelPreviousPageControl();
				ResetScreen(1, 1, 0);
				break;
				
			case 1:
				mode = 0x00000201;
				ResetScreen(1, 1, 0);
				break;
				
			case 2:
				mode = 0x00000301;
				vlfGuiNextPageControl(NULL);
				vlfGuiCancelNextPageControl();
				ResetScreen(1, 1, 0);
				break;
		}
	}

	// Install an OFW pagination
	else if (mode == 0x00000002 || mode == 0x00000102 || mode == 0x00000202 || mode == 0x00000302 || mode == 0x00000402 || mode == 0x00000502)
	{
		switch(page)
		{
			case 0:
				mode = 0x00000102;
				vlfGuiPreviousPageControl(NULL);
				vlfGuiCancelPreviousPageControl();
				ResetScreen(1, 1, 0);
				break;
			
			case 1:
				mode = 0x00000202;
				ResetScreen(1, 1, 0);
				break;
			
			case 2:
				mode = 0x00000302;
				ResetScreen(1, 1, 0);
				break;
			
			case 3:
				mode = 0x00000402;
				ResetScreen(1, 1, 0);
				break;
				
			case 4:
				mode = 0x00000502;
				vlfGuiNextPageControl(NULL);
				vlfGuiCancelNextPageControl();
				ResetScreen(1, 1, 0);
				break;
		}
	}

	// Hardware information pagination
	else if (mode == 0x00000004 || mode == 0x00000104 || mode == 0x00000204 || mode == 0x00000304 || mode == 0x00000404)
	{
		switch(page)
		{
			case 0:
				mode = 0x00000104;
				vlfGuiPreviousPageControl(NULL);
				vlfGuiCancelPreviousPageControl();
				ResetScreen(0, 1, 0);
				SystemInfoPage();
				break;
			
			case 1:
				mode = 0x00000204;
				ResetScreen(0, 1, 0);
				SystemInfoPage();
				break;
			
			case 2:
				mode = 0x00000304;
				ResetScreen(0, 1, 0);
				SystemInfoPage();
				break;

			case 3:
				mode = 0x00000404;
				vlfGuiNextPageControl(NULL);
				vlfGuiCancelNextPageControl();
				ResetScreen(0, 1, 0);
				SystemInfoPage();
				break;
		}
	}

	// Format Flash pagination
	else if (mode == 0x00000403 || mode == 0x00010403 || mode == 0x00020403)
	{
		switch(page)
		{
			case 0:
				mode = 0x00010403;
				vlfGuiPreviousPageControl(NULL);
				vlfGuiCancelPreviousPageControl();
				ResetScreen(0, 1, 0);
				FormatNandPage();
				break;
			
			case 1:
				mode = 0x00020403;
				vlfGuiNextPageControl(NULL);
				vlfGuiCancelNextPageControl();
				ResetScreen(0, 1, 0);
				FormatNandPage();
				break;
		}
	}

	// Create IdStorage pagination
	else if(mode == 0x00010503 || mode == 0x01010403 || mode == 0x02010403 || mode == 0x03010403)
	{
		switch(page)
		{
			case 0:
				mode = 0x01010403;
				vlfGuiPreviousPageControl(NULL);
				vlfGuiCancelPreviousPageControl();
				ResetScreen(0, 1, 0);
				CreateIdstoragePage();
				break;
			
			case 1:
				if(mode == 0x01010403)
				{
					selected_region = vlfGuiCentralMenuSelection();
					sel_reg = selected_region;
				}
				mode = 0x02010403;
				ResetScreen(0, 1, 0);
				CreateIdstoragePage();
				break;
			
			case 2:
				mode = 0x03010403;
				vlfGuiNextPageControl(NULL);
				vlfGuiCancelNextPageControl();
				ResetScreen(0, 1, 0);
				CreateIdstoragePage();
				break;
		}
	}

	return 0;
}

int OnBackToMainMenu(int enter)
{
	if(!enter)
	{
		ResetScreen(1, 1, 0);

		// Install an CFW
		     if(mode == 0x00000001){mode = 0x00000000;}      // back to main menu from install an CFW
		else if(mode == 0x00000101){mode = 0x00000000;} // back to main menu from install an CFW page 1
		else if(mode == 0x00000201){mode = 0x00000000;} // back to main menu from install an CFW page 2
		else if(mode == 0x00000301){mode = 0x00000000;} // back to main menu from install an CFW page 3
		else if(mode == 0x00010101){mode = 0x00000101;} // back to install an CFW page 1 from installation
		else if(mode == 0x00010201){mode = 0x00000201;} // back to install an CFW page 2 from installation
		else if(mode == 0x00010301){mode = 0x00000301;} // back to install an CFW page 3 from installation

		// Install an OFW
		else if(mode == 0x00000002){mode = 0x00000000;} // back to main menu from install an OFW
		else if(mode == 0x00000102){mode = 0x00000000;} // back to main menu from install an OFW page 1
		else if(mode == 0x00000202){mode = 0x00000000;} // back to main menu from install an OFW page 2
		else if(mode == 0x00000302){mode = 0x00000000;} // back to main menu from install an OFW page 3
		else if(mode == 0x00000402){mode = 0x00000000;} // back to main menu from install an OFW page 4
		else if(mode == 0x00000502){mode = 0x00000000;} // back to main menu from install an OFW page 5
		else if(mode == 0x00010102){mode = 0x00000102;} // back to install an OFW page 1 from installation
		else if(mode == 0x00010202){mode = 0x00000202;} // back to install an OFW page 2 from installation
		else if(mode == 0x00010302){mode = 0x00000302;} // back to install an OFW page 3 from installation
		else if(mode == 0x00010402){mode = 0x00000402;} // back to install an OFW page 3 from installation
		else if(mode == 0x00010502){mode = 0x00000502;} // back to install an OFW page 3 from installation

		// Nand tools Menu
		else if(mode == 0x00000003){mode = 0x00000000;} // back to main menu from nand tools menu
		else if(mode == 0x00000103){mode = 0x00000003;} // back to nand tools menu from nand dump
		else if(mode == 0x00000203){mode = 0x00000003;} // back to nand tools menu from nand restore
		else if(mode == 0x00000303){mode = 0x00000003;} // back to nand tools menu from check bad block
		else if(mode == 0x00000403){mode = 0x00000003;} // back to nand tools menu from flash format
		else if(mode == 0x00010403){mode = 0x00000003;} // back to nand tools menu from flash format page 1
		else if(mode == 0x00020403){mode = 0x00000003;} // back to nand tools menu from flash format page 2

		// IdStorage Menu
		else if(mode == 0x00000503){mode = 0x00000003;} // back to nand tools menu from idstorage menu
		else if(mode == 0x00010503){mode = 0x00000503;} // back to idstorage menu from create new idstorage
		else if(mode == 0x01010403){mode = 0x00000503;} // back to idstorage menu from create new idstorage page 1
		else if(mode == 0x02010403){mode = 0x00000503;} // back to idstorage menu from create new idstorage page 2
		else if(mode == 0x03010403){mode = 0x00000503;} // back to idstorage menu from create new idstorage page 3
		else if(mode == 0x00020503){mode = 0x00000503;} // back to idstorage menu from change region
		else if(mode == 0x00030503){mode = 0x00000503;} // back to idstorage menu from fix mac
		else if(mode == 0x00040503){mode = 0x00000503;} // back to idstorage menu from dump idstorage
		else if(mode == 0x00050503){mode = 0x00000503;} // back to idstorage menu from restore idstorage
	
		// Hardware Info
		else if(mode == 0x00000004){mode = 0x00000000;} // back to main menu from hardware information
		else if(mode == 0x00000104){mode = 0x00000000;} // back to main menu from hardware information page 1
		else if(mode == 0x00000204){mode = 0x00000000;} // back to main menu from hardware information page 2
		else if(mode == 0x00000304){mode = 0x00000000;} // back to main menu from hardware information page 3
		else if(mode == 0x00000404){mode = 0x00000000;} // back to main menu from hardware information page 4

		else{mode = 0x00000000;}

		if (!page_control)
		{
			vlfGuiNextPageControl(NULL);
			vlfGuiPreviousPageControl(NULL);
			vlfGuiCancelPreviousPageControl();
			vlfGuiCancelNextPageControl();
			vlfGuiSetPageControlEnable(0);
		}
		else
		{
			vlfGuiSetPageControlEnable(1);
		}

		if(mode == 0x00000000)
		{
			ResetScreen(1, 0, selitem);
		}
		else
		{
			ResetScreen(1, 1, selitem);
		}

		page_control = 0;
		AddBackgroundHandler(PSP_CTRL_SELECT, SetBackground, NULL);
		vlfGuiSetRectangleFade(0, VLF_TITLEBAR_HEIGHT, 480, 272 - VLF_TITLEBAR_HEIGHT, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0, NULL, NULL, 0);
	}

	return 0;
}

int OnMainMenuSelect(int sel)
{
	ResetScreen(0, 0, 0);
	selitem = sel;

	// Main Menu Select
	if(mode == 0x00000000)
	{
		switch (sel)
		{
			case 0:	// Install an CFW Menu
				mode = 0x00000001;
				ResetScreen(1, 1, 0);
				PageControl(0);
				break;
				
			case 1:	// Install an OFW Menu
				mode = 0x00000002;
				ResetScreen(1, 1, 0);
				PageControl(0);
				break;
				
			case 2:	// Nand Operation Menu
				mode = 0x00000003;
				ResetScreen(1, 1, 0);
				break;
			
			case 3:	// Hardware Information
				mode = 0x00000004;
				ResetScreen(0, 1, 0);
				PageControl(0);
				break;
			
			case 4:	// Test M33
				AddWaitIcon();
				sctrlKernelExitVSH(NULL);
				break;
			
			case 5:	// Shutdown
				AddWaitIcon();
				scePowerRequestStandby();
				break;
			
			case 6:	// Reboot
				AddWaitIcon();
				scePowerRequestColdReset(0);
				break;
		}
	}
	
	// Install CFW Page 1 Menu
	else if(mode == 0x00000101)
	{
		mode = 0x00010101;
		switch (sel)
		{
			case 0:	// Install CFW 371 M33
				Install(0, 0x0173);
				break;
			
			case 1:	// Install CFW 380 M33
				Install(0, 0x017C);
				break;
			
			case 2:	// Install CFW 390 M33
				Install(0, 0x0186);
				break;
			
			case 3:	// Install CFW 393 GEN
				Install(0, 0x0189);
				break;
			
			case 4:	// Install CFW 395 GEN
				Install(0, 0x018B);
				break;
			
			case 5:	// Install CFW 401 M33
				Install(0, 0x0191);
				break;
		}
	}

	// Install CFW Page 2 Menu
	else if(mode == 0x00000201)
	{
		mode = 0x00010201;
		switch (sel)
		{
			case 0:	// Install CFW 500 M33
				Install(0, 0x01F4);
				break;
			
			case 1:	// Install CFW 502 GEN
				Install(0, 0x01F6);
				break;
			
			case 2:	// Install CFW 503 GEN
				Install(0, 0x01F7);
				break;
			
			case 3:	// Install CFW 550 GEN
				Install(0, 0x0226);
				break;
			
			case 4:	// Install CFW 620 PRO
				Install(0, 0x026C);
				break;
			
			case 5:	// Install CFW 635 PRO
				Install(0, 0x027B);
				break;
		}
	}

	// Install CFW Page 3 Menu
	else if(mode == 0x00000301)
	{
		mode = 0x00010301;
		switch (sel)
		{
			case 0:	// Install CFW 637 LME
				Install(0, 0x027D);
				break;
			
			case 1:	// Install CFW 638 LME
				Install(0, 0x027E);
				break;
			
			case 2:	// Install CFW 639 LME
				MEorPRO = 1;
				Install(0, 0x027F);
				break;
			
			case 3:	// Install CFW 639 PRO
				MEorPRO = 0;
				Install(0, 0x027F);
				break;
			
			case 4:	// Install CFW 660 LME
				MEorPRO = 1;
				Install(0, 0x0294);
				break;
			
			case 5:	// Install CFW 660 PRO
				MEorPRO = 0;
				Install(0, 0x0294);
				break;
		}			
	}

	// Install OFW Page 1 Menu
	else if(mode == 0x00000102)
	{
		mode = 0x00010102;
		switch (sel)
		{
			case 0:	// Install OFW 3.71
				Install(1, 0x0173);
				break;
			
			case 1:	// Install OFW 3.72
				Install(1, 0x0174);
				break;
			
			case 2:	// Install OFW 3.73
				Install(1, 0x0175);
				break;
			
			case 3:	// Install OFW 3.80
				Install(1, 0x017C);
				break;
			
			case 4:	// Install OFW 3.90
				Install(1, 0x0186);
				break;
			
			case 5:	// Install OFW 3.93
				Install(1, 0x0189);
				break;
		}					
	}

	// Install OFW Page 2 Menu
	else if(mode == 0x00000202)
	{
		mode = 0x00010202;
		switch (sel)
		{
			case 0:	// Install OFW 3.95
				Install(1, 0x018B);
				break;
			
			case 1:	// Install OFW 3.96
				Install(1, 0x018C);
				break;
			
			case 2:	// Install OFW 4.00
				Install(1, 0x0190);
				break;
			
			case 3:	// Install OFW 4.01
				Install(1, 0x0191);
				break;
			
			case 4:	// Install OFW 4.05
				Install(1, 0x0195);
				break;
			
			case 5:	// Install OFW 5.00
				Install(1, 0x01F4);
				break;
		}						
	}

	// Install OFW Page 3 Menu
	else if(mode == 0x00000302)
	{
		mode = 0x00010302;
		switch (sel)
		{
			case 0:	// Install OFW 5.01
				Install(1, 0x01F5);
				break;
			
			case 1:	// Install OFW 5.02
				Install(1, 0x01F6);
				break;
			
			case 2:	// Install OFW 5.03
				Install(1, 0x01F7);
				break;
			
			case 3:	// Install OFW 5.05
				Install(1, 0x01F9);
				break;
			
			case 4:	// Install OFW 5.50
				Install(1, 0x0226);
				break;
			
			case 5:	// Install OFW 5.51
				Install(1, 0x0227);
				break;
		}						
	}
	
	// Install OFW Page 4 Menu
	else if(mode == 0x00000402)
	{
		mode = 0x00010402;
		switch (sel)
		{
			case 0:	// Install OFW 5.55
				Install(1, 0x022B);
				break;
			
			case 1:	// Install OFW 6.00
				Install(1, 0x0258);
				break;
			
			case 2:	// Install OFW 6.10
				Install(1, 0x0262);
				break;
			
			case 3:	// Install OFW 6.20
				Install(1, 0x026C);
				break;
			
			case 4:	// Install OFW 6.30
				Install(1, 0x0276);
				break;
			
			case 5:	// Install OFW 6.31
				Install(1, 0x0277);
				break;
		}						
	}
	
	// Install OFW Page 5 Menu
	else if(mode == 0x00000502)
	{
		mode = 0x00010502;
		switch (sel)
		{
			case 0:	// Install OFW 6.35
				Install(1, 0x027B);
				break;
			
			case 1:	// Install OFW 6.36
				Install(1, 0x027C);
				break;
			
			case 2:	// Install OFW 6.37
				Install(1, 0x027D);
				break;
			
			case 3:	// Install OFW 6.38
				Install(1, 0x027E);
				break;
			
			case 4:	// Install OFW 6.39
				Install(1, 0x027F);
				break;
			
			case 5:	// Install OFW 6.60
				Install(1, 0x0294);
				break;
		}						
	}

	// Nand Tools Menu
	else if(mode == 0x00000003)
	{
		u32 nandsize;
		switch (sel)
		{
			case 0:	// Dump Flash
				mode = 0x00000103;
				DumpNand("ms0:/nand-dump_%08X.bin", pspNandGetScramble());
				break;
			
			case 1:	// Restore Flash
				mode = 0x00000203;
				RestoreNand("ms0:/nand-dump_%08X.bin", pspNandGetScramble());
				break;

			case 2:	// Check Bad Block
				mode = 0x00000303;
				CheckNand();
				break;
			
			case 3:	// Format Flash
				mode = 0x00000403;				
				dcGetHardwareInfo(NULL, NULL, NULL, NULL, NULL, NULL, &nandsize);
				if (nandsize < 0x4000000)
				{
					flash_sizes[0] = 0x6000;
					flash_sizes[1] = 0x1000;
					flash_sizes[2] = 0x0400;
					flash_sizes[3] = 0x03C0;
				}
				else
				{
					flash_sizes[0] = 0xA400;
					flash_sizes[1] = 0x1400;
					flash_sizes[2] = 0x1000;
					flash_sizes[3] = 0x2480;
				}
				totalflash_size = flash_sizes[0] + flash_sizes[1] + flash_sizes[2] + flash_sizes[3];
				PageControl(0);
				break;

			case 4:	// IdStorge Menu
				mode = 0x00000503;
				ResetScreen(1, 1, 0);
				break;
		}
	}

	// IdStorage Menu
	else if(mode == 0x00000503)
	{
		switch (sel)
		{
			case 0:	// Create New IdStorage
				mode = 0x00010503;
				sel_reg = 0;
				macrandom = 1;
				ResetScreen(0, 1, 0);
				PageControl(0);
				break;

			case 1:	// Change Region
				mode = 0x00020503;
				ResetScreen(1, 1, 0);
				break;

			case 2:	// Fix Mac
				mode = 0x00030503;
				FixMac();
				return VLF_EV_RET_NOTHING;

			case 3:	// Dump IdStorage
				mode = 0x00040503;
				DumpIdstorage("ms0:/id-storage_%08X.bin", pspNandGetScramble());
				break;

			case 4:	// Restore IdStorage
				mode = 0x00050503;
				RestoreIdstorage("ms0:/id-storage_%08X.bin", pspNandGetScramble());
				break;
		}
	}

	// Region Menu (New IdStorage/Change Region)
	else if (mode == 0x00020503)					
	{
		switch (sel)
		{
			case 0:	// Region #1 : Europe
				ChangeRegion(0);
				break;
			
			case 1:	// Region #2 : Japan
				ChangeRegion(1);
				break;
			
			case 2:	// Region #3 : America
				ChangeRegion(2);
				break;
		}			
	}

	return 0;
}

void MainMenu(int sel)
{
	int i;

	if(mode == 0x00000000)
	{
		char *items[] =
		{
			STRING[STR_CUSTOM_FIRMWARES],
			STRING[STR_OFFICIAL_FIRMWARES],
			STRING[STR_NAND_OPERATION],
			STRING[STR_HARDWARE_INFO],
			STRING[STR_TEST_M33],
			STRING[STR_SHUTDOWN],
			STRING[STR_REBOOT],
		};
		
		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], unicode);
		}

		SetTitle("update_plugin", "tex_update_icon", STRING[STR_UNIVERSAL_UNBRICKER]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000101)
	{
		char *items[] = { "3.71 M33-4", "3.80 M33-5", "3.90 M33-3", "3.93 GEN-2", "3.95 GEN-2", "4.01 M33-2" };
		char *items_tmp[NELEMS(items)];

		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			sprintf(unicode, STRING[STR_INSTALL_CFW_VER], items[i]);
			items_tmp[i] = unicode;
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], items_tmp[i]);
		}

		selitem = 0;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_CUSTOM_FIRMWARES]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000201)
	{
		char *items[] = { "5.00 M33-6", "5.02 GEN-A", "5.03 GEN-C", "5.50 GEN-D3", "6.20 PRO-C2", "6.35 PRO-C2" };
		char *items_tmp[NELEMS(items)];

		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			sprintf(unicode, STRING[STR_INSTALL_CFW_VER], items[i]);
			items_tmp[i] = unicode;
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], items_tmp[i]);
		}

		selitem = 0;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_CUSTOM_FIRMWARES]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000301)
	{
		char *items[] = { "6.37 ME-8", "6.38 ME-3", "6.39 ME-9.8", "6.39 PRO-C2", "6.60 ME-1.8", "6.60 PRO-C2" };
		char *items_tmp[NELEMS(items)];

		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			sprintf(unicode, STRING[STR_INSTALL_CFW_VER], items[i]);
			items_tmp[i] = unicode;
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], items_tmp[i]);
		}

		selitem = 0;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_CUSTOM_FIRMWARES]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000102)
	{
		char *items[] = { "3.71", "3.72", "3.73", "3.80", "3.90", "3.93" };
		char *items_tmp[NELEMS(items)];

		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			sprintf(unicode, STRING[STR_INSTALL_OFW_VER], items[i]);
			items_tmp[i] = unicode;
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], items_tmp[i]);
		}

		selitem = 1;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_OFFICIAL_FIRMWARES]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000202)
	{
		char *items[] = { "3.95", "3.96", "4.00", "4.01", "4.05", "5.00" };
		char *items_tmp[NELEMS(items)];

		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			sprintf(unicode, STRING[STR_INSTALL_OFW_VER], items[i]);
			items_tmp[i] = unicode;
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], items_tmp[i]);
		}

		selitem = 1;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_OFFICIAL_FIRMWARES]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000302)
	{
		char *items[] = { "5.01", "5.02", "5.03", "5.05", "5.50", "5.51" };
		char *items_tmp[NELEMS(items)];

		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			sprintf(unicode, STRING[STR_INSTALL_OFW_VER], items[i]);
			items_tmp[i] = unicode;
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], items_tmp[i]);
		}

		selitem = 1;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_OFFICIAL_FIRMWARES]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000402)
	{
		char *items[] = { "5.55", "6.00", "6.10", "6.20", "6.30", "6.31" };
		char *items_tmp[NELEMS(items)];

		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			sprintf(unicode, STRING[STR_INSTALL_OFW_VER], items[i]);
			items_tmp[i] = unicode;
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], items_tmp[i]);
		}

		selitem = 1;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_OFFICIAL_FIRMWARES]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000502)
	{
		char *items[] = { "6.35", "6.36", "6.37", "6.38", "6.39", "6.60" };
		char *items_tmp[NELEMS(items)];

		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			sprintf(unicode, STRING[STR_INSTALL_OFW_VER], items[i]);
			items_tmp[i] = unicode;
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], items_tmp[i]);
		}

		selitem = 1;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_OFFICIAL_FIRMWARES]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000003)
	{
		char *items[] =
		{
			STRING[STR_DUMP_NAND],
			STRING[STR_RESTORE_NAND],
			STRING[STR_CHECK_NAND],
			STRING[STR_FORMAT_FLASH],
			STRING[STR_IDSTORAGE_TOOLS],
		};
		
		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], unicode);
		}

		selitem = 2;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_NAND_OPERATION]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if(mode == 0x00000503)
	{
		char *items[] =
		{
			STRING[STR_NEW_IDSTORAGE],
			STRING[STR_CHANGE_REGION],
			STRING[STR_FIX_MAC_ADDRESS],
			STRING[STR_BACKUP_IDSTORAGE],
			STRING[STR_RESTORE_IDSTORAGE],
		};
		
		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], unicode);
		}

		selitem = 4;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_IDSTORAGE_TOOLS]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}
	else if (mode == 0x00020503)
	{
		char *items[] =
		{
			STRING[STR_REGION_EUROPE],
			STRING[STR_REGION_JAPAN],
			STRING[STR_REGION_AMERICA],
		};
		
		for(i = 0; i < NELEMS(items); i++)
		{
			char unicode[128];
			UTF82Unicode(items[i], unicode);
			items[i] = malloc(strlen(unicode) + 1);
			strcpy(items[i], unicode);
		}
		

		selitem = 1;
		SetTitle("update_plugin", "tex_update_icon", STRING[STR_CHANGE_REGION]);
		vlfGuiCentralMenu(NELEMS(items), items, sel, OnMainMenuSelect, 0, 0);
	}

	if (!showenter_prev)
	{
		vlfGuiBottomDialog(-1, VLF_DI_ENTER, 1, 0, VLF_DEFAULT, OnBackToMainMenu);
		showenter_prev = 1;
	}
	
	vlfGuiSetRectangleFade(0, VLF_TITLEBAR_HEIGHT, 480, 272 - VLF_TITLEBAR_HEIGHT, VLF_FADE_MODE_IN, VLF_FADE_SPEED_FAST, 0, NULL, NULL, 0);
}

int app_main(SceSize args, void *argp)
{
	SceUID thid = sceKernelCreateThread("VUnbrickerCallbackThread", CallbackThread, 0x11, 0xFA0, 0, 0);
	sceKernelStartThread(thid, 0, 0);

	sm_buffer1 = malloc64(SMALL_BUFFER_SIZE);
	sm_buffer2 = malloc64(SMALL_BUFFER_SIZE);
	BIG_BUFFER_SIZE = (25*1024*1024);
	big_buffer = malloc64(BIG_BUFFER_SIZE);

	if (!big_buffer)
	{
		BIG_BUFFER_SIZE = (10*1024*1024);
		big_buffer = malloc64(BIG_BUFFER_SIZE);
	}

	if (!sm_buffer1 || !sm_buffer2 || !big_buffer)
	{
		asm("break\n");
		while (1);
	}

	vpl_init();
	select_language();
	vlfGuiSystemSetup(1, 1, 0);
	SetBackground(NULL);
	ResetScreen(1, 0, 0);
	AddBackgroundHandler(PSP_CTRL_SELECT, SetBackground, NULL);

	while(1)
	{
		vlfGuiDrawFrame();
	}

   	return 0;
}